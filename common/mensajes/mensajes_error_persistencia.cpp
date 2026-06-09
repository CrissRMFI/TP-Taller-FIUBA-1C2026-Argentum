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

        case CodigoErrorPersistencia::DIMENSIONES_INVALIDAS:
            return "Dimensiones de mapa invalidas";

        case CodigoErrorPersistencia::REGISTRO_INVALIDO:
            return "Registro invalido (pared o ciudad)";

        case CodigoErrorPersistencia::NPC_DUPLICADO_O_INVALIDO:
            return "NPC duplicado o invalido";

        case CodigoErrorPersistencia::TOML_MAL_FORMADO:
            return "TOML mal formado: no se pudo parsear el archivo de mapa";

        case CodigoErrorPersistencia::CLAVE_FALTANTE:
            return "Falta una clave obligatoria o tiene tipo invalido";

        default:
            return "Error de persistencia desconocido";
    }
}
