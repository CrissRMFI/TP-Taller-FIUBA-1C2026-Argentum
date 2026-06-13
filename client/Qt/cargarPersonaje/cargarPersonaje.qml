import QtQuick
import QtQuick.Controls
import QmlCppExample

// Root que llena la QQuickView; el contenido (640x480) se escala a la ventana.
Item {
    id: root
    property bool errorLogin: cargarPersonajeController.huboErrorLogin()
    Component.onCompleted: {
        if (root.errorLogin) {
            audioMenu.reproducirEfecto("error")
        }
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

    Item {
        id: contenido
        width: 640
        height: 480
        transformOrigin: Item.TopLeft
        scale: Math.min(root.width / width, root.height / height)

        Image {
            id: seleccionPersonaje
            source: "../graficos/InicioSesion.png"
            anchors.fill: parent
            z: 0
        }

        TextField {
            id: nickInput
            x: parent.width / 2 - width / 2
            y: parent.height / 2 - height / 2
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

        TitleText {
            text: qsTr("NICKNAME")
            x: parent.width / 2 - width / 2
            y: nickInput.y - height * 1.5
        }

        ErrorText {
            id: errorMessageNick
            x: (nickInput.x + nickInput.width / 2) - (width / 2)
            y: nickInput.y + nickInput.height
            text: ""
        }

        ErrorText {
            id: errorLoginMessage
            x: parent.width / 2 - width / 2
            y: crearCuentaButton.y - height * 1.5
            z: 2
            visible: root.errorLogin && text !== ""
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
                    root.errorLogin = false;
                    if (nickInput.text === "") {
                        audioMenu.reproducirEfecto("error");
                        errorMessageNick.text = "";
                        errorUnirseMessage.text = "Por favor, complete el campo de nick.";

                    } else {
                        const nickValido = cargarPersonajeController.esNickValido(nickInput.text);
                        if (!nickValido) {
                            audioMenu.reproducirEfecto("error");
                            errorMessageNick.text = "El nick no debería tener espacios y debe ser menor o igual a 32 bytes.";
                            errorUnirseMessage.text = "";
                            return;
                        }

                        errorMessageNick.text = "";
                        errorUnirseMessage.text = "";
                        cargarPersonajeController.setNick(nickInput.text);
                        console.log("Intentando unirse a la partida...");

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
                    audioMenu.reproducirEfecto("volver");
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
                    audioMenu.reproducirEfecto("click");
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
}
