#ifndef COMMON_PERSISTENCIA_ERROR_PERSISTENCIA_H
#define COMMON_PERSISTENCIA_ERROR_PERSISTENCIA_H

#include <stdexcept>
#include <string>

#include "../mensajes/codigo_error_persistencia.h"

class ErrorPersistencia: public std::runtime_error {
private:
    static std::string componerMensaje(CodigoErrorPersistencia codigo, const std::string& detalle);

    CodigoErrorPersistencia codigo;
  public:
    ErrorPersistencia(CodigoErrorPersistencia codigo, const std::string& detalle);

    CodigoErrorPersistencia getCodigo() const;

};

#endif
