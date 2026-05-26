#ifndef DATOS_CONEXION_H
#define DATOS_CONEXION_H

#include <string>
#include "server/game/modelo/clase_personaje.h"
#include "server/game/modelo/raza.h"

enum class MensajeError {
    CuentaNoEncontrada,
    NickYaExistente,
    PuertoHostInvalidos
};

struct DatosLogin {
    std::string puerto;
    std::string host;
};

struct DatosNuevoPersonaje {
    std::string nick;
    std::string password;
    Raza raza;
    ClasePersonaje clase;
};

struct DatosPersonaje {
    std::string nick;
    std::string password;
};

class DatosConexion {
public:
    void setDatosLogin(const std::string& puerto, const std::string& host) {
        datosLogin.puerto = puerto;
        datosLogin.host = host;
        esNuevoPersonaje = false;
    }

    void setDatosNuevoPersonaje(const std::string& nick, const std::string& password, Raza raza, ClasePersonaje clase) {
        datosNuevoPersonaje.nick = nick;
        datosNuevoPersonaje.password = password;
        datosNuevoPersonaje.raza = raza;
        datosNuevoPersonaje.clase = clase;
        esNuevoPersonaje = true;
    }

    void setDatosPersonaje(const std::string& nick, const std::string& password) {
        datosPersonaje.nick = nick;
        datosPersonaje.password = password;
        esNuevoPersonaje = false;
    }

    void setEsNuevoPersonaje(bool nuevo) {
        esNuevoPersonaje = nuevo;
    }

    void setErrorLogin(MensajeError mensaje) {
        errorLogin = true;
        switch (mensaje) {
            case MensajeError::CuentaNoEncontrada:
                mensajeError = "Cuenta no encontrada. Por favor, verifique su nick/contraseña y vuelva a iniciar sesion.";
                break;
            case MensajeError::NickYaExistente:
                mensajeError = "El nick ingresado ya existe. Por favor, vuelva a iniciar sesion.";
                break;
            case MensajeError::PuertoHostInvalidos:
                mensajeError = "Puerto o host invalidos. Por favor, vuelva a iniciar sesion.";
                break;
            default:
                mensajeError = "Error desconocido. Por favor, vuelva a iniciar sesion.";
                break;
        }
    }

    void clearError() {
        errorLogin = false;
        mensajeError = "";
    }

    const DatosLogin& getDatosLogin() const {
        return datosLogin;
    }

    const DatosNuevoPersonaje& getDatosNuevoPersonaje() const {
        return datosNuevoPersonaje;
    }

    const DatosPersonaje& getDatosPersonaje() const {
        return datosPersonaje;
    }

    const std::string& getMensajeError() const {
        return mensajeError;
    }

    bool esConexionNuevoPersonaje() const {
        return esNuevoPersonaje;
    }

    bool tieneErrorLogin() const {
        return errorLogin;
    }
    
    

private:
    DatosLogin datosLogin;
    DatosNuevoPersonaje datosNuevoPersonaje;
    DatosPersonaje datosPersonaje;
    bool esNuevoPersonaje = false;
    bool errorLogin = false;
    std::string mensajeError = "";
};
#endif