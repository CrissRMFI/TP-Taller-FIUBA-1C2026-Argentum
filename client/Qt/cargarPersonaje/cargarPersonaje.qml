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
    property bool errorLogin: cargarPersonajeController.huboErrorLogin()

    Image {
        id: seleccionPersonaje
        source: "../graficos/InicioSesion.png"
        anchors.fill: parent
        z: 0
    }

    component ImageButton: Image {
        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
    }

    component ErrorText: Text {
        width: 300
        height: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "red"
        visible: text !== ""
    }

    component ButtonText: Text {
        width: 205
        height: 50
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
    }

    component TitleText: Text {
        width: 205
        height: 50
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
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

    TitleText {
        text: qsTr("NICKNAME")
        x: parent.width / 2 - width / 2
        y: nickInput.y - height * 1.5
    }

    TitleText {
        text: qsTr("CONTRASEÑA")
        x: parent.width / 2 - width / 2
        y: passwordInput.y - height * 1.5
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

    ErrorText {
        id: errorLoginMessage
        x: parent.width / 2 - width / 2
        y: crearCuentaButton.y - height * 1.5
        z: 2
        visible: errorLogin && text !== ""
        text: cargarPersonajeController.getErrorLoginMessage()
    }

    ErrorText {
        id: errorUnirseMessage
        x: parent.width / 2 - width / 2
        y: crearCuentaButton.y - height * 1.5
        text: ""
    }

    ImageButton {
        TapHandler {
            onTapped: {
                errorLogin = false;
                if (nickInput.text === "" || passwordInput.text === "") {
                    errorMessageNick.text = "";
                    errorMessagePassword.text = "";
                    errorUnirseMessage.text = "Por favor, complete todos los campos";
                } else {
                    errorUnirseMessage.text = "";
                    const nickValido = cargarPersonajeController.esTextoValido(nickInput.text);
                    const passwordValido = cargarPersonajeController.esTextoValido(passwordInput.text);

                    errorMessageNick.text = !nickValido ? "El nick no debería tener espacios y debe ser menor o igual a 32 bytes." : "";
                    errorMessagePassword.text = !passwordValido ? "La contraseña no debería tener espacios y debe ser menor o igual a 32 bytes." : "";

                    if (nickValido && passwordValido) {
                        cargarPersonajeController.setNick(nickInput.text);
                        cargarPersonajeController.setPassword(passwordInput.text);
                        console.log("Intentando unirse a la partida...");
                    }
                }
            }
        }
        id: iniciarSesionButton
        x: parent.width - width
        y: parent.height - height
        width: 208
        height: 50
        source: "../graficos/emptybutton.png"
    }

    ImageButton {
        TapHandler {
            onTapped: {
                console.log("Volviendo al menú principal");
                cargarPersonajeController.volverAlMenu();
            }
        }
        id: volverButton
        x: 4
        y: parent.height - height
        width: 208
        height: 50
        source: "../graficos/emptybutton.png"
    }

    ImageButton {
        TapHandler {
            onTapped: {
                console.log("Rediriguiendo a la pantalla de creación de cuenta");
                cargarPersonajeController.volverACrearCuenta();
            }
        }
        id: crearCuentaButton
        x: (parent.width / 2) - width / 2 
        y: parent.height - height
        width: 208
        height: 50
        source: "../graficos/emptybutton.png"
    }

    ButtonText {
        text: qsTr("INICIAR SESIÓN")
        anchors.centerIn: iniciarSesionButton
    }

    ButtonText {
        text: qsTr("VOLVER")
        anchors.centerIn: volverButton
    }

    ButtonText {
        text: qsTr("CREAR CUENTA")
        anchors.centerIn: crearCuentaButton
    }
}
