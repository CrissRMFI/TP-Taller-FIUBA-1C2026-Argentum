#include "mensajes_error_persistencia.h"

const char* MensajesErrorPersistencia::mensaje(CodigoErrorPersistencia codigo) {
    switch (codigo) {
        case CodigoErrorPersistencia::NO_SE_PUEDE_ABRIR_ARCHIVO:
            return "No se puede abrir el archivo";

        case CodigoErrorPersistencia::NO_SE_PUEDE_ESCRIBIR:
            return "No se puede escribir en el archivo";

        case CodigoErrorPersistencia::NO_SE_PUEDE_RENOMBRAR:
            return "No se puede renombrar el archivo temporal";

        case CodigoErrorPersistencia::EOF_PREMATURO:
            return "EOF prematuro o lectura corta";

        case CodigoErrorPersistencia::BYTES_EXTRA:
            return "Bytes extra despues del ultimo registro";

        case CodigoErrorPersistencia::MAGIC_INVALIDO:
            return "Magic invalido: no es un .bin de mapa";

        case CodigoErrorPersistencia::VERSION_INCOMPATIBLE:
            return "Version de archivo no soportada por el lector";

        case CodigoErrorPersistencia::DIMENSIONES_INVALIDAS:
            return "Dimensiones de mapa invalidas";

        case CodigoErrorPersistencia::REGISTRO_INVALIDO:
            return "Registro invalido (pared o ciudad)";

        case CodigoErrorPersistencia::NPC_DUPLICADO_O_INVALIDO:
            return "NPC duplicado o invalido";

        case CodigoErrorPersistencia::CANTIDAD_EXCEDE_UINT32:
            return "Cantidad de registros excede uint32";

        default:
            return "Error de persistencia desconocido";
    }
}
