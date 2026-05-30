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
    title: qsTr("Seleccion de Personaje")
    property string raza: ""
    property string clase: ""

    Image {
        id: seleccionPersonaje
        source: "../graficos/ElecciónPersonaje.png"
        anchors.fill: parent
    }

    component ImageButton: Image {
        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
    }

    component SwitchImage: Image {
        required property string sourceBaseName
        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
        property bool checked

        source: `../graficos/${sourceBaseName}${checked ? "-Checked" : "@2x"}.png`    }

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
        y: parent.height / 10
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
        y: nickInput.y * 2.9 - height / 2
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
        id: nickErrorText
        x: nickInput.x + nickInput.width / 2 - width / 2
        y: nickInput.y + nickInput.height - 2
        text: ""
    }

    ErrorText {
        id: passwordErrorText
        x: passwordInput.x + passwordInput.width / 2 - width / 2
        y: passwordInput.y + passwordInput.height
        text: ""
    }

    ErrorText {
        id: generalErrorText
        x: parent.width / 2 - width / 2
        y: crearPersonajeButton.y - height - 5
        text: ""
    }


    ImageButton {
        TapHandler {
            onTapped: {
                if (nickInput.text !== "" && raza != "" && clase != "") {
                    const esNickValido = personajeController.esTextoValido(nickInput.text);
                    const esPasswordValido = personajeController.esTextoValido(passwordInput.text);

                    nickErrorText.text = !esNickValido ? "El nick no puede tener espacios y debe ser menor o igual a 32 bytes." : "";
                    passwordErrorText.text = !esPasswordValido ? "La contraseña no puede tener espacios y debe ser menor o igual a 32 bytes." : "";
                    if (!esNickValido || !esPasswordValido) {
                        return;
                    }
                    personajeController.setRaza(raza);
                    personajeController.setClase(clase);
                    personajeController.setNick(nickInput.text);
                    personajeController.setPassword(passwordInput.text);
                    console.log("Personaje creado con éxito, entrando a la partida");
                } else {
                    generalErrorText.text = "Por favor, complete todos los campos para crear el personaje";
                }
            }
        }
        id: crearPersonajeButton
        x: 369
        y: 420
        width: 205
        height: 50
        source: "../graficos/crearPersonajeButton.png"
    }

    ImageButton {
        TapHandler {
            onTapped: {
                console.log("Volviendo al menú principal");
                personajeController.volverAlMenu();
            }
        }
        id: volverButton
        x: crearPersonajeButton.x - parent.width / 2 + 2
        y: 420
        width: 210
        height: 47
        source: "../graficos/VolverButton.png"
    }

    SwitchImage {
        TapHandler {
            onTapped: {
                switchHumano.checked = true;
                switchElfo.checked = false;
                switchEnano.checked = false;
                switchGnomo.checked = false;
                raza = "HUMANO";
            }
        }
        id: switchHumano
        x: 110
        y: 242
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {
        TapHandler {
            onTapped: {
                switchHumano.checked = false;
                switchElfo.checked = true;
                switchEnano.checked = false;
                switchGnomo.checked = false;
                raza = "ELFO";
            }
        }
        id: switchElfo
        x: switchHumano.x + 130
        y: switchHumano.y
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {
        TapHandler {
            onTapped: {
                switchHumano.checked = false;
                switchElfo.checked = false;
                switchEnano.checked = true;
                switchGnomo.checked = false;
                raza = "ENANO";
            }
        }
        id: switchEnano
        x: switchElfo.x + 130
        y: switchElfo.y
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {
        TapHandler {
            onTapped: {
                switchHumano.checked = false;
                switchElfo.checked = false;
                switchEnano.checked = false;
                switchGnomo.checked = true;
                raza = "GNOMO";
            }
        }
        id: switchGnomo
        x: switchEnano.x + 130
        y: switchEnano.y
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {        
        TapHandler {
            onTapped: {
                switchMago.checked = true;
                switchPaladin.checked = false;
                switchClerigo.checked = false;
                switchGuerrero.checked = false;
                clase = "MAGO";
            }
        }
        id: switchMago
        x: switchHumano.x
        y: switchHumano.y * 1.5
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {
        TapHandler {
            onTapped: {
                switchMago.checked = false;
                switchPaladin.checked = true;
                switchClerigo.checked = false;
                switchGuerrero.checked = false;
                clase = "PALADIN";
            }
        }
        id: switchPaladin
        x: switchMago.x + 130
        y: switchMago.y
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {
        TapHandler {
            onTapped: {
                switchMago.checked = false;
                switchPaladin.checked = false;
                switchClerigo.checked = true;
                switchGuerrero.checked = false;
                clase = "CLERIGO";
            }
        }
        id: switchClerigo
        x: switchPaladin.x + 130
        y: switchPaladin.y
        sourceBaseName: "LED"
        checked: false
    }
    SwitchImage {
        TapHandler {
            onTapped: {
                switchMago.checked = false;
                switchPaladin.checked = false;
                switchClerigo.checked = false;
                switchGuerrero.checked = true;
                clase = "GUERRERO";
            }
        }
        id: switchGuerrero
        x: switchClerigo.x + 130
        y: switchClerigo.y
        sourceBaseName: "LED"
        checked: false
    }
}
