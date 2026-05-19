# Argentum Online — Gameloop: Arquitectura e Implementación

## Qué es el gameloop

El gameloop es el núcleo del servidor. Es el hilo que mantiene el estado del mundo, recibe los comandos de los clientes, los aplica al estado y envía las respuestas. **Todo el estado del juego vive en un único hilo**, sin concurrencia interna. Los hilos de red depositan comandos en una cola y leen mensajes de salida de otra; el gameloop consume y produce de esas estructuras thread-safe.

---

## Diagrama de arranque

```
main()
  └─ LectorConfigToml::cargar(ruta)  →  ConfigCompleta { juego, items }
       └─ ParserTOML::parsear()           (un único parse del TOML)
  └─ Gameloop(monitor, ConfigCompleta)
       └─ Juego(ConfigJuego, CatalogoItems)
  └─ Gameloop::run()                  →  hilo del servidor
```

El servidor arranca leyendo una sola vez el archivo `config/game_config.toml`. De ese único parse se construyen dos cosas: la configuración numérica del juego (`ConfigJuego`) y el catálogo de items (`CatalogoItems`). Ambas se pasan al `Gameloop` que las transfiere a `Juego`.

---

## Carga de configuración

### ConfigJuego

Struct plano con todos los parámetros numéricos del juego: factores de vida/maná por clase y raza, fórmulas de experiencia y oro, umbrales de combate, parámetros de clan, cheats de testing. Ver [`config/config_juego.h`](../server/game/config/config_juego.h).

No tiene lógica de carga propia. Quien la carga es `LectorConfigToml`.

### ParserTOML

Parser mínimo propio para el subconjunto TOML que usa el proyecto: secciones `[tabla]`, subsecciones `[padre.hijo]` y valores float/int/bool/string. Se implementó en lugar de una librería externa para evitar dependencias en el build. Ver [`config/parser_toml.h`](../server/game/config/parser_toml.h).

### CatalogoItems

Registro de todos los tipos de items del juego, cargado desde la sección `[items.*]` del mismo TOML. Mapea `uint16_t id → Item*`. Los IDs son estables (definidos en el TOML con el campo `id`), lo que garantiza que el mismo número identifica siempre el mismo tipo de item entre reinicios del servidor.

Cada item tiene un `tipo` en el TOML (`"arma"`, `"baculo"`, `"armadura"`, `"casco"`, `"escudo"`, `"pocion_vida"`, `"pocion_mana"`) que determina qué subclase de `Item` se construye. Ver [`objeto/catalogo_items.h`](../server/game/objeto/catalogo_items.h).

### ConfigCompleta y parseo único

`LectorConfigToml::cargar` devuelve `ConfigCompleta { ConfigJuego juego; CatalogoItems items; }`. El archivo se abre y parsea una sola vez; el mismo `ParserTOML` sirve para construir ambas estructuras. Esto evita la doble lectura que habría si `CatalogoItems` tuviera su propio `cargar(ruta)`.

---

## El hilo Gameloop

[`server/gameloop/gameloop.h`](../server/gameloop/gameloop.h)

```
mientras (servidor activo):
    1. Procesar todos los comandos pendientes en la cola
    2. Llamar a Juego::actualizar()   ← tick del mundo
    3. Despachar mensajes de salida a los clientes correspondientes
    4. Dormir 200 ms
```

### Cola de comandos (`Queue<ComandoCliente>`)

Los hilos de red reciben paquetes del cliente y los convierten en `ComandoCliente { idCliente, ComandoJugador }`. Los depositan en `colaComandos` (thread-safe). El gameloop los consume con `try_pop` (sin bloqueo) al inicio de cada tick.

### Despacho de mensajes (`MonitorClientes`)

Cada método de `Juego` retorna `std::list<MensajeSalida>`. Un `MensajeSalida` tiene un destino: `TipoDestino::UNO` (un cliente específico) o `TipoDestino::TODOS` (broadcast). `Gameloop::despachar` itera esa lista y llama a `MonitorClientes` que tiene las colas de salida de cada cliente conectado.

---

## Estado del mundo: Juego

[`server/game/juego.h`](../server/game/juego.h)

`Juego` es la clase que centraliza todo el estado del servidor. Contiene:

| Miembro | Qué es |
|---|---|
| `jugadoresConectados` | `map<id, Jugador>` — jugadores con sesión activa |
| `jugadoresDesconectados` | `map<id, Jugador>` — jugadores que se desconectaron pero cuyo personaje persiste |
| `criaturasEnMapa` | `map<id, Criatura>` — NPCs enemigos vivos |
| `clanes` | `map<id, Clan>` — organizaciones de jugadores |
| `cfg` | `ConfigJuego` — parámetros del juego |
| `catalogo` | `CatalogoItems` — tipos de items disponibles |

### Flujo de un comando

```
Gameloop::procesarComando(ComandoCliente)
  └─ Juego::ejecutarComando(idCliente, ComandoJugador)
       └─ switch(opcode) → ejecutarMeditar / ejecutarAtacar / ejecutarFundarClan / ...
            └─ retorna list<MensajeSalida>
  └─ Gameloop::despachar(mensajes)
```

El `switch` sobre `Opcode` en `ejecutarComando` es el punto de entrada de toda la lógica de juego. Cada comando delega a un método privado de `Juego`.

### Tick del mundo (`actualizar`)

Llamado cada 200 ms. Actualmente:
- Llama a `jugador.recuperar(TICK_SEGUNDOS)` en todos los jugadores conectados para regenerar vida y maná.
- Envía `ESTADO_PERSONAJE` a cada jugador con sus stats actuales.

Pendiente: movimiento de criaturas, aggro, respawn, expiración de ítems en el suelo.

---

## Jugador

[`server/game/jugador.h`](../server/game/jugador.h)

Representa el personaje de un cliente conectado. Los stats base (fuerza, agilidad, inteligencia, constitución) se calculan a partir de la raza usando `ConfigJuego::statsRaza`. `vidaMax` y `manaMax` se derivan de esos stats combinados con los factores de clase.

### Estado del jugador

El campo `estado` es un `enum class Estado { Vivo, Fantasma, Meditando, Resucitando }`. Centraliza toda la lógica de estado sin flags booleanos independientes que pueden desincronizarse.

- `estaVivo()` → `Vivo || Meditando`
- `esFantasma()` → `Fantasma || Resucitando`

### Progresión

`ganar_experiencia(cantidad)` acumula exp y llama a `subirNivel()` cuando se supera el límite `expLimiteBase * nivel^expLimiteExp`. Al subir de nivel se recalculan `vidaMax` y `manaMax` y el personaje se cura completamente.

### Oro

Se divide en dos campos: `oroMano` (hasta el tope `100 * nivel^oroMaxExp`) y `oroExceso` (excedente que cae al suelo al morir). `getOro()` devuelve la suma de ambos. `sumar_oro` llena primero `oroMano`; si se supera el tope, el resto va a `oroExceso`.

### Banco interno

No existe una clase `Banco` separada. El banco es global por definición del juego (se accede desde cualquier sucursal), así que sus datos viven directamente en `Jugador`: `oroBanco` y `idItemsBanco`. Las operaciones `agregar_oro_banco` / `sacar_oro_banco` son transacciones atómicas: mueven el oro entre la mano del jugador y el banco en la misma llamada.

### Meditación

`meditar()` cambia el estado a `Meditando`. En cada tick, `recuperar(segundos)` regenera maná al ritmo `factorMeditacionClase(clase) * inteligencia` por segundo. Al llegar a `manaMax` el estado vuelve a `Vivo` automáticamente. Mover al jugador también cancela la meditación.

---

## Inventario y Equipamiento

[`server/game/objeto/inventario.h`](../server/game/objeto/inventario.h)

`Inventario` gestiona los slots de ítem (array de 20 `uint16_t`; `0` = slot vacío) y delega el equipamiento a `Equipamiento`. La separación existe porque tienen responsabilidades distintas:

- **`Inventario`**: almacenamiento, agregar/sacar/buscar por ID de ítem.
- **`Equipamiento`**: qué ítem está activo en cada slot de combate (arma, báculo, armadura, casco, escudo) e impone la invariante de que arma y báculo son mutuamente excluyentes (equipar uno desaloja al otro).

### Equipar un ítem

El comando `EQUIPAR` envía un **índice de slot** (posición en el inventario visible para el cliente), no un ID. `Jugador::equipar_item(indice, catalogo)` resuelve el ID con `inventario.getIdEnSlot(indice)`, consulta el catálogo para saber el tipo, y llama:
- `inventario.equiparItem(id, tipo)` para armas y báculos.
- `inventario.equiparPieza(id, TipoDefensa::slot)` para defensas, porque hay tres slots distintos (armadura, casco, escudo) que `TipoItem::Defensa` solo no puede discriminar.

`equiparItem` rechaza `TipoItem::Defensa` y `TipoItem::Pocion` — las pociones se usan, no se equipan; las defensas requieren conocer el sub-slot.

---

## Clan

[`server/game/clan.h`](../server/game/clan.h)

Organización de jugadores con tres estados por miembro: `Pendiente`, `Aceptado`, `Baneado`. Se usa un único `vector<MiembroClan>` con el estado como campo en lugar de tres listas separadas, para que la transición entre estados sea un cambio de valor en lugar de una remoción/inserción coordinada entre colecciones.

Comandos implementados: fundar, pedir unirse, aceptar/rechazar/ban/kick miembros, dejar el clan, revisar miembros y solicitudes pendientes.

Solo el fundador puede gestionar miembros. Si el fundador se desconecta, las solicitudes quedan en espera hasta que vuelva.

---

## Protocolo

El protocolo define qué mensajes se intercambian entre cliente y servidor.

- **`ComandoJugador`** (`common/protocolo/comando_jugador.h`): lo que envía el cliente. Tiene un `Opcode` y un `std::variant` con el payload específico del comando. Los comandos sin payload (MEDITAR, RESUCITAR, TOMAR, REVISAR_CLAN, DEJAR_CLAN) usan structs vacíos en el variant.

- **`MensajeServidor`** (`common/protocolo/mensaje_servidor.h`): lo que envía el servidor. Mismo patrón: `Opcode` + `std::variant` con el payload.

- **`MensajeSalida`** (`server/gameloop/mensaje_salida.h`): wrapper interno que agrega el destino (`TipoDestino::UNO` con `idCliente` o `TipoDestino::TODOS`) antes de pasar por `MonitorClientes`.

---

## Estado de implementación

| Componente | Estado |
|---|---|
| Config (TOML, ConfigJuego, ParserTOML) | ✅ Completo |
| CatalogoItems | ✅ Completo |
| Jugador | ✅ Completo (salvo daño con arma — requiere catálogo en calcular_danio) |
| Inventario / Equipamiento | ✅ Completo |
| Clan | ✅ Header + lógica de Juego completos — falta `clan.cpp` |
| Gameloop (hilo, cola, despacho) | ✅ Completo |
| Juego — chat global/privado | ✅ Completo |
| Juego — sistema de clanes | ✅ Completo |
| Juego — meditar | ✅ Completo |
| Juego — tick (recuperar + broadcast estado) | ✅ Completo |
| Juego — mover, atacar, equipar, comprar/vender | ⏳ Stub (requiere Mapa) |
| Juego — banco (depositar/retirar) | ⏳ Stub (requiere Mapa para verificar banquero cercano) |
| Juego — curar, tomar, tirar, resucitar | ⏳ Stub (requiere Mapa) |
| Criatura | ⏳ Solo header |
| Mapa | ❌ No implementado |

El patrón de los stubs es siempre el mismo: la verificación de proximidad a un NPC (banquero, comerciante, sacerdote) requiere el `Mapa`. La lógica de transferencia de items/oro ya está implementada en `Jugador`; los stubs solo necesitan la guardia de proximidad y luego delegar.
