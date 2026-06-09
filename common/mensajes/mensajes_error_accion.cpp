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

        default:
            return "Error de accion desconocido";
    }
}
