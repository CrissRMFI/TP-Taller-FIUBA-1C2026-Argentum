#include "mensajes_error_protocolo.h"

const char* MensajesErrorProtocolo::mensaje(CodigoErrorProtocolo codigo) {
    switch (codigo) {
        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_ENVIAR_BYTES:
            return "Conexion cerrada al enviar bytes";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_RECIBIR_BYTES:
            return "Conexion cerrada al recibir bytes";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_ENVIAR_UN_BYTE:
            return "Conexion cerrada al enviar un byte";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_ENVIAR_DOS_BYTES:
            return "Conexion cerrada al enviar dos bytes";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_ENVIAR_CUATRO_BYTES:
            return "Conexion cerrada al enviar cuatro bytes";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_RECIBIR_UN_BYTE:
            return "Conexion cerrada al recibir un byte";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_RECIBIR_DOS_BYTES:
            return "Conexion cerrada al recibir dos bytes";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_RECIBIR_CUATRO_BYTES:
            return "Conexion cerrada al recibir cuatro bytes";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_ENVIAR_CADENA:
            return "Conexion cerrada al enviar una cadena";

        case CodigoErrorProtocolo::CONEXION_CERRADA_AL_RECIBIR_CADENA:
            return "Conexion cerrada al recibir una cadena";

        case CodigoErrorProtocolo::ENVIO_INCOMPLETO:
            return "Envio incompleto de datos";

        case CodigoErrorProtocolo::RECEPCION_INCOMPLETA:
            return "Recepcion incompleta de datos";

        case CodigoErrorProtocolo::OPCODE_CLIENTE_INVALIDO:
            return "Opcode invalido recibido del cliente";

        case CodigoErrorProtocolo::OPCODE_SERVIDOR_INVALIDO:
            return "Opcode invalido recibido del servidor";

        case CodigoErrorProtocolo::OPCODE_DESCONOCIDO:
            return "Opcode desconocido";

        case CodigoErrorProtocolo::CADENA_DEMASIADO_LARGA:
            return "Cadena demasiado larga para el protocolo";

        case CodigoErrorProtocolo::CADENA_RECIBIDA_SUPERA_MAXIMO:
            return "Cadena recibida supera el maximo permitido";

        case CodigoErrorProtocolo::CAMPO_INVALIDO:
            return "Campo invalido en el mensaje recibido";

        case CodigoErrorProtocolo::DIRECCION_INVALIDA:
            return "Direccion de movimiento invalida";

        case CodigoErrorProtocolo::INDICE_ITEM_INVALIDO:
            return "Indice de item invalido";

        case CodigoErrorProtocolo::TIPO_ENTIDAD_INVALIDO:
            return "Tipo de entidad invalido";

        case CodigoErrorProtocolo::ESTADO_ENTIDAD_INVALIDO:
            return "Estado de entidad invalido";

        case CodigoErrorProtocolo::CODIGO_ERROR_ACCION_INVALIDO:
            return "Codigo de error de accion invalido";

        case CodigoErrorProtocolo::ERROR_DESCONOCIDO:
            return "Error de protocolo desconocido";

        default:
            return "Error de protocolo desconocido";
    }
}
