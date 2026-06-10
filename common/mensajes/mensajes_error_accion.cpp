#include "mensajes_error_accion.h"

const char* MensajesErrorAccion::mensaje(CodigoErrorAccion codigo) {
    switch (codigo) {
        case CodigoErrorAccion::INVENTARIO_LLENO:
            return "Inventario lleno";

        case CodigoErrorAccion::ORO_INSUFICIENTE:
            return "Oro insuficiente";

        case CodigoErrorAccion::NIVEL_INSUFICIENTE:
            return "Nivel insuficiente";

        case CodigoErrorAccion::ACCION_NO_PERMITIDA:
            return "Accion no permitida";

        case CodigoErrorAccion::OBJETIVO_INVALIDO:
            return "Objetivo invalido";

        case CodigoErrorAccion::CLAN_LLENO:
            return "Clan lleno";

        case CodigoErrorAccion::MANA_INSUFICIENTE:
            return "Mana insuficiente";

        case CodigoErrorAccion::FUERA_DE_RANGO:
            return "El objetivo esta fuera de alcance";

        case CodigoErrorAccion::ZONA_SEGURA:
            return "No podes atacar en una zona segura";

        case CodigoErrorAccion::CLASE_SIN_MAGIA:
            return "Tu clase no puede usar magia";

        case CodigoErrorAccion::HECHIZO_YA_CONOCIDO:
            return "Ya conoces ese hechizo";

        case CodigoErrorAccion::COOLDOWN_ATAQUE:
            return "Todavia no podes volver a atacar";

        case CodigoErrorAccion::OBJETIVO_MUERTO:
            return "El objetivo no esta vivo";

        case CodigoErrorAccion::USUARIO_YA_CONECTADO:
            return "El usuario ya esta conectado";

        case CodigoErrorAccion::SIN_POSICION_LIBRE:
            return "No hay una posicion libre donde aparecer";

        case CodigoErrorAccion::ID_EN_USO:
            return "El identificador de personaje ya esta en uso";

        case CodigoErrorAccion::NO_SE_PUDO_CARGAR_PERSONAJE:
            return "No se pudo cargar tu personaje guardado";

        case CodigoErrorAccion::ESTAS_MUERTO:
            return "No podes hacer eso estando muerto";

        case CodigoErrorAccion::NO_TENES_CLAN:
            return "No perteneces a ningun clan";

        case CodigoErrorAccion::YA_TENES_CLAN:
            return "Ya perteneces a un clan";

        case CodigoErrorAccion::CLAN_NOMBRE_EN_USO:
            return "Ya existe un clan con ese nombre";

        case CodigoErrorAccion::NO_SOS_LIDER_CLAN:
            return "Solo el lider del clan puede hacer eso";

        case CodigoErrorAccion::BANEADO_DEL_CLAN:
            return "Estas baneado de ese clan";

        case CodigoErrorAccion::NO_PODES_RESUCITAR:
            return "No podes resucitar en este momento";

        case CodigoErrorAccion::ESTAS_INMOVILIZADO:
            return "Estas inmovilizado";

        case CodigoErrorAccion::ATAQUE_NEWBIE:
            return "No se puede atacar a personajes nuevos";

        case CodigoErrorAccion::DIFERENCIA_NIVEL_EXCESIVA:
            return "La diferencia de nivel es demasiado grande";

        case CodigoErrorAccion::ATAQUE_ALIADO:
            return "No podes atacar a un companero de clan";

        case CodigoErrorAccion::HECHIZO_NO_OFENSIVO:
            return "Ese hechizo no sirve para atacar";

        case CodigoErrorAccion::HECHIZO_NO_CONOCIDO:
            return "No conoces ese hechizo";

        default:
            return "Error de accion desconocido";
    }
}
