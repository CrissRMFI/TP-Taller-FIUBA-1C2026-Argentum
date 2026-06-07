#include "mensajes_error_cliente.h"

const char* MensajesErrorCliente::mensaje(CodigoErrorCliente codigo) {
    switch (codigo) {
        case CodigoErrorCliente::CONFIG_NO_LEIDA:
            return "No se pudo leer la configuracion del cliente; se usan valores por defecto";

        case CodigoErrorCliente::FUENTE_NO_CARGADA:
            return "No se pudo cargar la fuente del chat; el chat no se mostrara";

        case CodigoErrorCliente::FONDO_CHAT_NO_CARGADO:
            return "No se pudo cargar el fondo del chat; se dibuja sin panel";

        case CodigoErrorCliente::ITEMS_NO_LEIDOS:
            return "No se pudo leer el catalogo de items; el inventario quedara sin nombres";

        case CodigoErrorCliente::ICONO_ITEM_NO_CARGADO:
            return "No se pudo cargar el icono de un item; se dibuja el slot sin imagen";

        case CodigoErrorCliente::PANEL_NO_CARGADO:
            return "No se pudo cargar una imagen del panel; se dibuja con fondo plano";

        default:
            return "Error del cliente desconocido";
    }
}
