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

        default:
            return "Error de accion desconocido";
    }
}
