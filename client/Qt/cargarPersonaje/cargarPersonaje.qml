import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QmlCppExample

Window {
    minimumWidth: 640
    minimumHeight: 480
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    visible: true
    title: qsTr("Inicia Sesión")

    Image {
        id: seleccionPersonaje
        source: "../graficos/InicioSesion.png"
        anchors.fill: parent
    }

    component ErrorText: Text {
        width: 300
        height: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "red"
        visible: text !== ""
    }

    TextField {
        id: nickInput
        x: parent.width / 2 - width / 2
        y: parent.height / 3 - height / 2
        width: 230
        height: 30
        placeholderText: qsTr("Ingrese su nick")
        // Fondo transparente
        background: Rectangle {
            color: "transparent"
            border.color: "white"
        }
        text: ""
    }

    TextField {
        id: passwordInput
        x: nickInput.x
        y: parent.height / 4 * 2.5 - height / 2
        width: 230
        height: 30
        placeholderText: qsTr("Ingrese su contraseña")
        // Fondo transparente
        background: Rectangle {
            color: "transparent"
            border.color: "white"
        }
        text: ""
    }

    ErrorText {
        id: errorMessageNick
        x: (nickInput.x + nickInput.width / 2) - (width / 2)
        y: nickInput.y + nickInput.height
        text: ""
    }

    ErrorText {
        id: errorMessagePassword
        x: (passwordInput.x + passwordInput.width / 2) - (width / 2)
        y: passwordInput.y + passwordInput.height
        text: ""
    }

    Image {
        TapHandler {
            onTapped: {
                if (nickInput.text !== "" && passwordInput.text !== "") {
                    if (!cargarPersonajeController.esTextoValido(nickInput.text) || !cargarPersonajeController.esTextoValido(passwordInput.text)) {
                        console.log("El nick/contraseña no debe tener espacios y tiene que ser menor o igual a 32 bytes.");
                        return;
                    }
                    cargarPersonajeController.setNick(nickInput.text);
                    cargarPersonajeController.setPassword(passwordInput.text);
                    console.log("Verificando existencia de cuenta...");
                } else {
                    errorMessageNick.text = nickInput.text === "" ? "Por favor, ingrese su nick" : "";
                    errorMessagePassword.text = passwordInput.text === "" ? "Por favor, ingrese su contraseña" : "";
                }
            }
        }
        id: jugarButton
        x: 369
        y: 420
        width: 205
        height: 50
        source: "../graficos/jugarButton.png"
    }

    Image {
        TapHandler {
            onTapped: {
                console.log("Volviendo al menú principal");
                cargarPersonajeController.volverAlMenu();
            }
        }
        id: volverButton
        x: jugarButton.x - parent.width / 2 + 2
        y: 420
        width: 210
        height: 47
        source: "../graficos/VolverButton.png"
    }

    Image {
        TapHandler {
            onTapped: {
                console.log("Rediriguiendo a la pantalla de creación de personaje");
                cargarPersonajeController.volverACrearPersonaje();
            }
        }
        id: crearPersonajeButton
        x: nickInput.x
        y: parent.height / 9 * 7 - height / 4
        width: 215
        height: 57
        source: "../graficos/crearPersonajeButton.png"
    }
}
