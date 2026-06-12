#include "menuController.h"
#include "cargarPersonaje/cargarPersonajeController.h"
#include "login/loginController.h"
#include "elegirPersonaje/elegirPersonajeController.h"

#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QScreen>

#include "../../common/socket/socket.h"


MenuController::MenuController()
    : audio(std::make_unique<GestorAudioMenu>()) {
    configurarAudio();
}

void MenuController::configurarAudio() {
    audio->setVolumenMusica(0.5f);
    audio->setVolumenEfectos(0.7f);

    // Completar estas fuentes cuando agregues los archivos reales.
    audio->configurarMusicaPrincipal(QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/instrumental/38.wav")));
    audio->registrarEfecto("click", QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/instrumental/463.wav")));
    audio->registrarEfecto("volver", QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/instrumental/210.wav")));
    audio->registrarEfecto("error", QUrl(QStringLiteral("qrc:/QmlCppExample/client/Qt/instrumental/24.wav")));
}

void MenuController::centrarVentana(QQuickView* ventana_) {
    if (!ventana_) {
        return;
    }

    QScreen* pantalla = QGuiApplication::primaryScreen();
    if (!pantalla) {
        return;
    }

    const QRect areaDisponible = pantalla->availableGeometry();
    const int x = areaDisponible.x() + (areaDisponible.width() - ventana_->width()) / 2;
    const int y = areaDisponible.y() + (areaDisponible.height() - ventana_->height()) / 2;
    ventana_->setPosition(x, y);
}

void MenuController::setVentana(QQuickView* ventana_view) { this->ventana = ventana_view; }

void MenuController::run(DatosConexion& datos) {
    if (ventana && audio) {
        ventana->rootContext()->setContextProperty("audioMenu", audio.get());
    }
    centrarVentana(ventana);

    audio->reproducirMusicaPrincipal();

	while (!terminoRegistro) {
		if (!loginYaRealizado) {
			LoginController login;
			login.run(*ventana, datos);
			try {
				Socket preflight(datos.getDatosLogin().host.c_str(),
				                 datos.getDatosLogin().puerto.c_str());
				preflight.close();
			} catch (...) {
                audio->detenerMusica();
				datos = puertoHostInvalidos();
				return;
			}
			datos.clearError();
		}

		while (true) {
			CargarPersonajeController cargarPersonaje;
			CargarPersonajeResultado cargarResultado;
			cargarPersonaje.run(*ventana, datos, cargarResultado);

			if (cargarResultado == CargarPersonajeResultado::VolverAlMenu) {
				break;
			}

			if (cargarResultado == CargarPersonajeResultado::ContinuarConPersonajeExistente) {
				terminoRegistro = true;
				break;
			}

			ElegirPersonajeController elegirPersonaje;
			ElegirPersonajeResultado elegirResultado;
			elegirPersonaje.run(*ventana, datos, elegirResultado);

			if (elegirResultado == ElegirPersonajeResultado::VolverAlMenu) {
				continue;
			}

			terminoRegistro = true;
			break;
		}
	}
	terminoRegistro = false;
    audio->detenerMusica();
    return;
}

DatosConexion MenuController::nombreUsuarioNoEncontrado(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::NombreUsuarioNoEncontrado);
	loginYaRealizado = true;
    run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
	return datos;
}

DatosConexion MenuController::nickYaExistente(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::NickYaExistente);
	loginYaRealizado = true;
	run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
	return datos;
}

DatosConexion MenuController::puertoHostInvalidos() {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::PuertoHostInvalidos);
    run(datos);
	return datos;
}

DatosConexion MenuController::usuarioYaConectado(const DatosLogin& datosLogin) {
    DatosConexion datos;
    datos.setErrorLogin(MensajeError::UsuarioYaConectado);
	loginYaRealizado = true;
    run(datos);
	loginYaRealizado = false;
	datos.setDatosLogin(datosLogin.puerto, datosLogin.host);
    return datos;
}
