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
    title: qsTr("Argentum Online - Menu de Conexion")
    property bool errorLogin: loginController.huboErrorLogin()

    component ErrorText: Text {
        width: 300
        height: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "red"
        visible: text !== ""
    }

    component ImageButton: Image {
        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
    }

    component TextField_: TextField {
        width: 230
        height: 30
        placeholderText: qsTr("Escriba aqui")
    }

    component Text_: Text {
        width: 300
        height: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
    }

    Text {
        id: errorLoginMessage
        x: parent.width / 2 - width / 2
        y: unirseButton.y + unirseButton.height + 10
        width: 300
        height: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "red"
        z: 2
        visible: errorLogin && text !== ""
        text: loginController.getErrorLogin()
    }

    Image {
        id: menu
        source: "../graficos/menu.png"
        anchors.fill: parent
        z: 0
    }

    TextField_ {
        id: hostInput
        x: parent.width / 2 - width / 2
        y: 218
        text: ""
    }

    Text_ {
        id: hostLabel
        x: (hostInput.x + hostInput.width / 2) - (width / 2)
        y: hostInput.y - 30
        text: qsTr("DIRECCIÓN IP / HOST")
    }

    TextField_ {
        id: puertoInput
        x: parent.width / 2 - width / 2
        y: hostInput.y + hostInput.height + 60
        width: 230
        height: 30
        placeholderText: qsTr("Escriba aqui")
        text: ""
    }

    Text_ {
        id: puertoLabel
        x: (puertoInput.x + puertoInput.width / 2) - (width / 2)
        y: puertoInput.y - 30
        text: qsTr("PUERTO")
    }

    ErrorText {
        id: errorMessageHost
        x: (hostInput.x + hostInput.width / 2) - (width / 2)
        y: hostInput.y + hostInput.height
        text: ""
    }

    ErrorText {
        id: errorMessagePuerto
        x: (puertoInput.x + puertoInput.width / 2) - (width / 2)
        y: puertoInput.y + puertoInput.height
        text: ""
    }

    ErrorText {
        id: errorMessageUnirse
        x: (unirseButton.x + unirseButton.width / 2) - (width / 2)
        y: unirseButton.y + unirseButton.height + 10
        text: ""
    }

    ErrorText {
        id: errorMessageConexion
        x: (unirseButton.x + unirseButton.width / 2) - (width / 2)
        y: unirseButton.y + unirseButton.height + 10
        text: ""
    }

    ImageButton {
        TapHandler {
            onTapped: {
                errorLogin = false;
                if (puertoInput.text === "" || hostInput.text === "") {
                    errorMessageHost.text = "";
                    errorMessagePuerto.text = "";
                    errorMessageUnirse.text = "Por favor, complete todos los campos";
                } else {
                    errorMessageUnirse.text = "";
                    const hostValido = loginController.esHostIpValido(hostInput.text);
                    const puertoValido = loginController.esPuertoValido(puertoInput.text);

                    errorMessageHost.text = hostValido ? "" : "La dirección IP/Host no puede estar vacía";
                    errorMessagePuerto.text = puertoValido ? "" : "El puerto tiene que ser un número entre 1 y 65535";

                    if (hostValido && puertoValido) {
                        loginController.setPuerto(puertoInput.text);
                        loginController.setHost(hostInput.text);
                        console.log("Intentando unirse a la partida...");
                    }
                }
            }
        }
        id: unirseButton
        x: parent.width / 2 - width / 2
        y: puertoInput.y + puertoInput.height + 50
        width: 150
        height: 40
        source: "../graficos/unirseButton.png"
    }

}

