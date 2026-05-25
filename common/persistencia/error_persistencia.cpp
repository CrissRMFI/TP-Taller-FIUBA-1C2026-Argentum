#include "error_persistencia.h"

#include "../mensajes/mensajes_error_persistencia.h"

std::string ErrorPersistencia::componerMensaje(CodigoErrorPersistencia codigo, const std::string& detalle) {

    std::string texto = MensajesErrorPersistencia::mensaje(codigo);
    if (!detalle.empty()) {
        texto += ": ";
        texto += detalle;
    }
    return texto;
}

ErrorPersistencia::ErrorPersistencia(CodigoErrorPersistencia codigo, const std::string& detalle)
    : std::runtime_error(componerMensaje(codigo, detalle)), codigo(codigo) {}

CodigoErrorPersistencia ErrorPersistencia::getCodigo() const {
    return codigo;
}
