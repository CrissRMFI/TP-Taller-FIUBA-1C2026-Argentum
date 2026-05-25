#include "jugador.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "../../common/game/aleatorio.h"
#include "objeto/catalogo_items.h"
#include "reglas/reglas_juego.h"
#include "../../common/protocolo/tipo_entidad.h"

static void aplicarRecuperacion(float& pendiente, uint16_t& actual, uint16_t maximo, float delta) {
    if (actual >= maximo) {
        pendiente = 0.0f;
        return;
    }

    pendiente += delta;
    uint32_t puntos = static_cast<uint32_t>(std::floor(pendiente));
    if (puntos == 0) {
        return;
    }

    pendiente -= static_cast<float>(puntos);
    actual = static_cast<uint16_t>(std::min<uint32_t>(actual + puntos, maximo));

    if (actual >= maximo) {
        pendiente = 0.0f;
    }
}

Jugador::Jugador(uint16_t id, const std::string& nombre, ClasePersonaje clase, Raza raza, Posicion posicion, const ConfigJuego& config) :
        idJugador(id),
        idClan(0),
        nombre(nombre),
        nivel(1),
        experiencia(0),
        vidaActual(0),
        vidaMax(0),
        manaActual(0),
        manaMax(0),
        recuperacionVidaPendiente(0.0f),
        recuperacionManaPendiente(0.0f),
        meditacionManaPendiente(0.0f),
        oroMano(0),
        oroExceso(0),
        oroPerdidoPendiente(0),
        oroBanco(0),
        fuerza(0),
        agilidad(0),
        inteligencia(0),
        constitucion(0),
        posicion(posicion),
        posicionResurreccion(posicion),
        cfg(config),
        inventario(config.inventarioMaxItems),
        idItemsBanco(),
        clase(clase),
        estado(Estado::Vivo),
        raza(raza),
        fundadoClan(false),
        tiempoRestanteInmovilizado(0.0f) {
    const StatsRaza& sr = cfg.statsRaza(raza);

    fuerza = static_cast<uint8_t>(sr.fuerza);
    agilidad = static_cast<uint8_t>(sr.agilidad);
    inteligencia = static_cast<uint8_t>(sr.inteligencia);
    constitucion = static_cast<uint8_t>(sr.constitucion);

    vidaMax = ReglasJuego::calcularVidaMaxima(
            cfg, raza, clase, nivel, constitucion);

    manaMax = ReglasJuego::calcularManaMaximo(
            cfg, raza, clase, nivel, inteligencia);

    vidaActual = vidaMax;
    manaActual = manaMax;
}

void Jugador::recibir_danio(uint16_t cantidad) {
    if (cfg.invulnerable || !estaVivo()) {
        return;
    }

    if (vidaActual <= cantidad) {
        vidaActual = 0;
        morir();
    } else {
        vidaActual -= cantidad;
    }
}

ResultadoDefensa Jugador::recibir_ataque_fisico(uint16_t danio,
                                                bool esCritico,
                                                const CatalogoItems& catalogo,
                                                Aleatorio& aleatorio,
                                                float multiplicadorDefensa) {
    // Defensor muerto o flag debug `invulnerable`: el ataque no impacta pero
    // tampoco fue esquivado. Reportamos Golpeado{0} para que el caller no
    // confunda este caso con una evasión real.
    if (!estaVivo() || cfg.invulnerable) {
        return { ResultadoDefensa::Tipo::Golpeado, 0 };
    }

    // Regla 5.2: el crítico omite la fase de evasión. La absorción (regla 5.5)
    // se sigue aplicando porque la regla 5.2 sólo habla de "fase de evasión".
    if (!esCritico && esquiva_ataque(aleatorio)) {
        return { ResultadoDefensa::Tipo::Esquivo, 0 };
    }

    uint16_t absorcion = 0;

    const uint16_t idDefensa = inventario.getDefensaEquipada();
    if (idDefensa != 0) {
        const Defensa* defensa = catalogo.comoDefensa(idDefensa);
        if (defensa != nullptr) {
            absorcion += aleatorio.enteroEnRango<uint16_t>(
                    defensa->getDefMin(), defensa->getDefMax());
        }
    }

    const uint16_t idCasco = inventario.getCascoEquipado();
    if (idCasco != 0) {
        const Defensa* casco = catalogo.comoDefensa(idCasco);
        if (casco != nullptr) {
            absorcion += aleatorio.enteroEnRango<uint16_t>(
                    casco->getDefMin(), casco->getDefMax());
        }
    }

    const uint16_t idEscudo = inventario.getEscudoEquipado();
    if (idEscudo != 0) {
        const Defensa* escudo = catalogo.comoDefensa(idEscudo);
        if (escudo != nullptr) {
            absorcion += aleatorio.enteroEnRango<uint16_t>(
                    escudo->getDefMin(), escudo->getDefMax());
        }
    }

    const uint16_t absorcionFinal = ReglasJuego::aplicarMultiplicadorCombate(
            absorcion, multiplicadorDefensa);
    const uint16_t danioFinal = danio > absorcionFinal ? danio - absorcionFinal : 0;
    recibir_danio(danioFinal);

    return { ResultadoDefensa::Tipo::Golpeado, danioFinal };
}

void Jugador::curar(uint16_t cantidad) {
    if (!estaVivo()) {
        return;
    }

    vidaActual = static_cast<uint16_t>(
            std::min<uint32_t>(vidaActual + cantidad, vidaMax));
}

void Jugador::recuperar_mana(uint16_t cantidad) {
    manaActual = static_cast<uint16_t>(
            std::min<uint32_t>(manaActual + cantidad, manaMax));
}

void Jugador::recuperar(float segundos) {
    if (estaInmovilizado()) {
        tiempoRestanteInmovilizado -= segundos;
        if (tiempoRestanteInmovilizado < 0.0f) {
            tiempoRestanteInmovilizado = 0.0f;
        }
    }
    if (!estaVivo()) {
        return;
    }

    if (vidaActual < vidaMax) {
        aplicarRecuperacion(
                recuperacionVidaPendiente,
                vidaActual,
                vidaMax,
                ReglasJuego::calcularRecuperacionNatural(cfg, raza, segundos));
    } else {
        recuperacionVidaPendiente = 0.0f;
    }

    if (manaActual < manaMax) {
        aplicarRecuperacion(
                recuperacionManaPendiente,
                manaActual,
                manaMax,
                ReglasJuego::calcularRecuperacionNatural(cfg, raza, segundos));
    } else {
        recuperacionManaPendiente = 0.0f;
    }

    if (estado == Estado::Meditando && manaActual < manaMax) {
        aplicarRecuperacion(
                meditacionManaPendiente,
                manaActual,
                manaMax,
                ReglasJuego::calcularRecuperacionMeditacion(
                        cfg, clase, inteligencia, segundos));

        if (manaActual >= manaMax) {
            manaActual = manaMax;
            estado = Estado::Vivo;
            meditacionManaPendiente = 0.0f;
        }
    } else if (estado != Estado::Meditando) {
        meditacionManaPendiente = 0.0f;
    }

    if (cfg.vidaInfinita) {
        vidaActual = vidaMax;
    }

    if (cfg.manaInfinito) {
        manaActual = manaMax;
    }

    if (cfg.suicidio) {
        morir();
    }
}

void Jugador::ganar_experiencia(uint32_t cantidad) {
    if (cfg.expX10) {
        const uint32_t maximo = std::numeric_limits<uint32_t>::max();
        cantidad = (cantidad > maximo / 10) ? maximo : cantidad * 10;
    }

    const uint32_t espacioDisponible =
            std::numeric_limits<uint32_t>::max() - experiencia;
    experiencia += std::min(cantidad, espacioDisponible);

    while (nivel < std::numeric_limits<uint8_t>::max()) {
        const uint32_t limite = ReglasJuego::calcularLimiteExperiencia(cfg, nivel);
        if (experiencia < limite) {
            break;
        }
        experiencia -= limite;
        subirNivel();
    }
}

void Jugador::sumar_oro(uint32_t cantidad) {
    uint32_t totalActual = oroMano + oroExceso;
    uint32_t maximoTotal = ReglasJuego::calcularOroMaximoTotal(cfg, nivel);

    uint32_t espacioTotal = 0;
    if (totalActual < maximoTotal) {
        espacioTotal = maximoTotal - totalActual;
    }

    uint32_t cantidadAceptada = std::min(cantidad, espacioTotal);

    oroMano += cantidadAceptada;
    normalizarOro();

    // Si cantidad > cantidadAceptada, ese oro no entra en el máximo permitido.
    // Cuando exista mapa, Juego debería decidir si se dropea al suelo.
}

bool Jugador::gastar_oro(uint32_t cantidad) {
    if (oroMano + oroExceso < cantidad) {
        return false;
    }

    uint32_t total = oroMano + oroExceso - cantidad;

    oroMano = total;
    oroExceso = 0;
    normalizarOro();

    return true;
}

uint32_t Jugador::extraer_oro_perdido() {
    const uint32_t oro = oroPerdidoPendiente;
    oroPerdidoPendiente = 0;
    return oro;
}

bool Jugador::puede_recibir_oro(uint32_t cantidad) const {
    if (cantidad == 0) {
        return false;
    }

    const uint32_t maximoTotal = ReglasJuego::calcularOroMaximoTotal(cfg, nivel);
    if (oroMano > maximoTotal || oroExceso > maximoTotal - oroMano) {
        return false;
    }

    const uint32_t totalActual = oroMano + oroExceso;

    return totalActual <= maximoTotal && cantidad <= maximoTotal - totalActual;
}

void Jugador::mover_a(uint16_t x, uint16_t y) {
    posicion.x = x;
    posicion.y = y;
}

void Jugador::resucitar(uint16_t x, uint16_t y) {
    estado = Estado::Vivo;
    posicion.x = x;
    posicion.y = y;
    vidaActual = vidaMax / 2;
    manaActual = 0;
}

void Jugador::inmovilizar(uint16_t resucitarX, uint16_t resucitarY, float segundos) {
    tiempoRestanteInmovilizado = segundos;
    posicionResurreccion.x = resucitarX;
    posicionResurreccion.y = resucitarY;
    estado = Estado::Resucitando;
}

void Jugador::meditar() {
    estado = Estado::Meditando;
}

void Jugador::cancelarMeditacion() {
    if (estado == Estado::Meditando) {
        estado = Estado::Vivo;
    }
}

ResultadoDanio Jugador::calcular_danio(const CatalogoItems& catalogo, Aleatorio& aleatorio) {
    uint8_t danioArmaMin = 1;
    uint8_t danioArmaMax = 1;

    const uint16_t idArmaEquipada = inventario.getArmaEquipada();
    const uint16_t idBaculoEquipado = inventario.getBaculoEquipado();

    if (idArmaEquipada != 0) {
        if (const Arma* armaEquipada = catalogo.comoArma(idArmaEquipada)) {
            danioArmaMin = armaEquipada->getDanioMin();
            danioArmaMax = armaEquipada->getDanioMax();
        }
    } else if (idBaculoEquipado != 0) {
        if (const Baculo* baculoEquipado = catalogo.comoBaculo(idBaculoEquipado)) {
            danioArmaMin = baculoEquipado->getDanioMin();
            danioArmaMax = baculoEquipado->getDanioMax();
        }
    }

    if (danioArmaMax < danioArmaMin) {
        danioArmaMax = danioArmaMin;
    }

    const uint16_t danioBaseArma = aleatorio.enteroEnRango<uint16_t>(danioArmaMin, danioArmaMax);

    // Regla 5.1: Daño = Fuerza * rand(DañoArmaMin, DañoArmaMax)
    // Regla 5.2: el crítico duplica el daño. El flag se propaga al caller para
    // que el defensor pueda saltear su esquive.
    // Saturamos en uint16_t::max porque fuerza * danioBaseArma puede exceder
    // 65535 con stats altos (ej. 255 * 65535) y el cast directo truncaría.
    const bool esGolpeCritico = es_golpe_critico(aleatorio);
    const uint32_t danioCrudo = static_cast<uint32_t>(fuerza) * danioBaseArma;
    const uint32_t danioConCritico = esGolpeCritico ? danioCrudo * 2u : danioCrudo;
    const uint16_t danioCalculado = static_cast<uint16_t>(
            std::min<uint32_t>(danioConCritico, std::numeric_limits<uint16_t>::max()));

    return ResultadoDanio{ danioCalculado, esGolpeCritico };
}

DescriptorAtaque Jugador::describir_ataque(const CatalogoItems& catalogo) const {
    const uint16_t idArmaEquipada = inventario.getArmaEquipada();
    const uint16_t idBaculoEquipado = inventario.getBaculoEquipado();

    // Regla 6.2: arma y báculo son mutuamente excluyentes. Si por alguna razón
    // ambos slots están ocupados (inconsistencia), priorizamos el arma física.
    if (idArmaEquipada != 0) {
        const Arma* armaEquipada = catalogo.comoArma(idArmaEquipada);
        if (armaEquipada != nullptr && armaEquipada->esArmaDistancia()) {
            return DescriptorAtaque{
                TipoAtaque::Distancia,
                cfg.rangoVisionAtaque,
                /*costoMana=*/0
            };
        }
        // Arma melee o registro corrupto: tratamos como melee.
        return DescriptorAtaque{
            TipoAtaque::CuerpoACuerpo,
            /*alcanceMaximo=*/1,
            /*costoMana=*/0
        };
    }

    if (idBaculoEquipado != 0) {
        if (!puedeUsarMagia()) {
            return DescriptorAtaque{
                TipoAtaque::HechizoNoOfensivo,
                cfg.rangoVisionAtaque,
                /*costoMana=*/0
            };
        }

        const Baculo* baculoEquipado = catalogo.comoBaculo(idBaculoEquipado);
        if (baculoEquipado != nullptr) {
            // El hechizo Curar no es un ataque ofensivo: no debe procesarse
            // por el comando ATACAR. `Juego` lo verá y devolverá error.
            if (baculoEquipado->getHechizo() == TipoHechizo::Curar) {
                return DescriptorAtaque{
                    TipoAtaque::HechizoNoOfensivo,
                    cfg.rangoVisionAtaque,
                    baculoEquipado->getCostoMana()
                };
            }
            return DescriptorAtaque{
                TipoAtaque::Hechizo,
                cfg.rangoVisionAtaque,
                baculoEquipado->getCostoMana()
            };
        }
    }

    // Sin arma ni báculo: puñetazo melee.
    return DescriptorAtaque{
        TipoAtaque::CuerpoACuerpo,
        /*alcanceMaximo=*/1,
        /*costoMana=*/0
    };
}

bool Jugador::agregar_item(uint16_t idItem) {
    return inventario.agregarItem(idItem);
}

bool Jugador::puede_agregar_item(uint16_t idItem) const {
    return idItem != 0 && inventario.tieneEspacioLibre();
}

bool Jugador::eliminar_item(uint16_t idItem) {
    return inventario.eliminarItem(idItem);
}

uint16_t Jugador::quitar_item_de_slot(uint8_t indice) {
    return inventario.quitarDeSlot(indice);
}

void Jugador::agregar_item_en_slot(uint16_t idItem, uint8_t indice) {
    inventario.agregarItemEnSlot(idItem, indice);
}

std::vector<uint16_t> Jugador::vaciar_inventario() {
    return inventario.vaciar();
}

bool Jugador::equipar_item(uint8_t indice, const CatalogoItems& catalogo) {
    uint16_t idItem = inventario.getIdEnSlot(indice);

    if (idItem == 0) {
        return false;
    }

    const Item* item = catalogo.buscar(idItem);
    if (item == nullptr) {
        return false;
    }

    if (item->getTipo() == TipoItem::Pocion) {
        if (!estaVivo()) {
            return false;
        }

        const Pocion* pocion = catalogo.comoPocion(idItem);
        if (pocion == nullptr) {
            return false;
        }

        if (pocion->getTipoPocion() == TipoPocion::Vida) {
            curar(pocion->getCantidad());
        } else {
            recuperar_mana(pocion->getCantidad());
        }

        inventario.eliminarSlot(indice);
        return true;
    }

    if (item->getTipo() == TipoItem::Defensa) {
        const Defensa* defensa = catalogo.comoDefensa(idItem);
        if (defensa == nullptr) {
            return false;
        }

        return inventario.equiparPiezaSlot(indice, defensa->getSlot());
    }

    return inventario.equiparSlot(indice, item->getTipo());
}

bool Jugador::agregar_item_banco(uint8_t indice) {
    uint16_t idItem = inventario.quitarDeSlot(indice);
    if (idItem == 0) {
        return false;
    }

    idItemsBanco.push_back(idItem);
    return true;
}

bool Jugador::agregar_oro_banco(uint32_t cantidad) {
    if (!gastar_oro(cantidad)) {
        return false;
    }

    oroBanco += cantidad;
    return true;
}

bool Jugador::sacar_item_banco(uint16_t idItem) {
    auto it = std::find(idItemsBanco.begin(), idItemsBanco.end(), idItem);

    if (it == idItemsBanco.end()) {
        return false;
    }

    if (!inventario.agregarItem(idItem)) {
        return false;
    }

    idItemsBanco.erase(it);
    return true;
}

bool Jugador::sacar_oro_banco(uint32_t cantidad) {
    if (oroBanco < cantidad) {
        return false;
    }

    uint32_t maximoEnMano = ReglasJuego::calcularOroMaximoTotal(cfg, nivel);
    if (getOro() + cantidad > maximoEnMano) {
        return false;
    }

    oroBanco -= cantidad;
    sumar_oro(cantidad);
    return true;
}

void Jugador::asignarClan(uint16_t id) {
    idClan = id;
}

void Jugador::salirClan() {
    idClan = 0;
    fundadoClan = false;
}

void Jugador::marcarFundadorClan() {
    fundadoClan = true;
}

bool Jugador::puedeMeditar() const {
    return manaMax > 0;
}

bool Jugador::puedeUsarMagia() const {
    return manaMax > 0;
}

bool Jugador::estaVivo() const {
    return estado == Estado::Vivo || estado == Estado::Meditando;
}

bool Jugador::esFantasma() const {
    return estado == Estado::Fantasma || estado == Estado::Resucitando;
}

bool Jugador::enMeditacion() const {
    return estado == Estado::Meditando;
}

bool Jugador::tieneClan() const {
    return idClan != 0;
}

uint16_t Jugador::getId() const {
    return idJugador;
}

uint8_t Jugador::getNivel() const {
    return nivel;
}

uint32_t Jugador::getExperiencia() const {
    return experiencia;
}

uint16_t Jugador::getVidaActual() const {
    return vidaActual;
}

uint16_t Jugador::getVidaMax() const {
    return vidaMax;
}

uint16_t Jugador::getManaActual() const {
    return manaActual;
}

uint16_t Jugador::getManaMax() const {
    return manaMax;
}

uint32_t Jugador::getOro() const {
    return oroMano + oroExceso;
}

uint32_t Jugador::getOroMano() const {
    return oroMano;
}

uint32_t Jugador::getOroExceso() const {
    return oroExceso;
}

uint32_t Jugador::getOroBanco() const {
    return oroBanco;
}

uint32_t Jugador::getOroPerdidoPendiente() const {
    return oroPerdidoPendiente;
}

uint16_t Jugador::getClan() const {
    return idClan;
}

ClasePersonaje Jugador::getClase() const {
    return clase;
}

Raza Jugador::getRaza() const {
    return raza;
}

bool Jugador::estaInmovilizado() const {
    return tiempoRestanteInmovilizado > 0.0f;
}

bool Jugador::fundo_clan() const {
    return fundadoClan;
}

bool Jugador::es_newbie() const {
    return nivel <= static_cast<uint8_t>(cfg.nivelNewbie);
}

std::string Jugador::getNombre() const {
    return nombre;
}

Posicion Jugador::getPosicion() const {
    return posicion;
}

Posicion Jugador::getPosicionResurreccion() const {
    return posicionResurreccion;
}

Estado Jugador::getEstado() const {
    return estado;
}

std::vector<uint16_t> Jugador::getSlotsInventario() const {
    return inventario.getSlots();
}

uint16_t Jugador::getIdItemEnSlot(uint8_t indice) const {
    return inventario.getIdEnSlot(indice);
}

uint16_t Jugador::getArmaEquipada() const {
    return inventario.getArmaEquipada();
}

uint16_t Jugador::getBaculoEquipado() const {
    return inventario.getBaculoEquipado();
}

uint16_t Jugador::getDefensaEquipada() const {
    return inventario.getDefensaEquipada();
}

uint16_t Jugador::getCascoEquipado() const {
    return inventario.getCascoEquipado();
}

uint16_t Jugador::getEscudoEquipado() const {
    return inventario.getEscudoEquipado();
}

std::vector<uint16_t> Jugador::getIdItemsBanco() const {
    return idItemsBanco;
}

void Jugador::subirNivel() {
    nivel++;

    vidaMax = ReglasJuego::calcularVidaMaxima(
            cfg, raza, clase, nivel, constitucion);

    manaMax = ReglasJuego::calcularManaMaximo(
            cfg, raza, clase, nivel, inteligencia);

    vidaActual = vidaMax;
    manaActual = manaMax;
}

void Jugador::morir() {
    estado = Estado::Fantasma;
    vidaActual = 0;

    if (!es_newbie()) {
        uint32_t experienciaAPerder =
                ReglasJuego::calcularPerdidaExperienciaMuerte(cfg, experiencia);

        perder_experiencia(experienciaAPerder);
    }

    const uint32_t espacioPendiente =
            std::numeric_limits<uint32_t>::max() - oroPerdidoPendiente;
    oroPerdidoPendiente += std::min(oroExceso, espacioPendiente);
    oroExceso = 0;
}

void Jugador::perder_experiencia(uint32_t cantidad) {
    experiencia = (experiencia >= cantidad) ? experiencia - cantidad : 0;
}

bool Jugador::consumir_mana(uint16_t cantidad) {
    if (manaActual < cantidad) {
        return false;
    }

    manaActual -= cantidad;
    return true;
}

void Jugador::consumir_item(uint16_t idItem) {
    inventario.eliminarItem(idItem);
}

void Jugador::normalizarOro() {
    uint32_t total = oroMano + oroExceso;
    uint32_t oroSeguro = ReglasJuego::calcularOroSeguro(cfg, nivel);

    oroMano = std::min(total, oroSeguro);
    oroExceso = total - oroMano;
}

bool Jugador::esquiva_ataque(Aleatorio& aleatorio) {
    return ReglasJuego::esquivaAtaque(cfg, agilidad, aleatorio.uniforme());
}

bool Jugador::es_golpe_critico(Aleatorio& aleatorio) {
    return ReglasJuego::esGolpeCritico(cfg, aleatorio.uniforme());
}
