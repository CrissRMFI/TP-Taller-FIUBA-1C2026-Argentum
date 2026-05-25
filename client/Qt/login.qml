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

    Image {
        id: menu
        source: "graficos/menu.png"
        anchors.fill: parent
    }

    TextField {
        id: hostInput
        x: parent.width / 2 - width / 2
        y: 218
        width: 230
        height: 30
        placeholderText: qsTr("Host")
        text: ""
    }
    TextField {
        id: puertoInput
        x: parent.width / 2 - width / 2
        y: hostInput.y + hostInput.height + 47
        width: 230
        height: 30
        placeholderText: qsTr("Puerto")
        text: ""
    }


    Image {
        TapHandler {
            onTapped: {
                if (puertoInput.text === "" || hostInput.text === "") {
                    console.log("Por favor, complete todos los campos");
                } else {
                    if (!loginController.esHostValido(hostInput.text)) {
                        console.log("El host no es válido (prueba con localhost o 127.0.0.1)");
                        return;
                    }
                    if (!loginController.esPuertoValido(puertoInput.text)) {
                        console.log("El puerto no coincide con el del servidor (prueba con 7666)");
                        return;
                    }
                    loginController.setPuerto(puertoInput.text);
                    loginController.setHost(hostInput.text);
                    console.log("Intentando unirse a la partida...");
                }
            }
        }
        id: unirseButton
        x: parent.width / 2 - width / 2
        y: puertoInput.y + puertoInput.height + 13
        width: 150
        height: 40
        source: "graficos/unirseButton.png"
    }

}


