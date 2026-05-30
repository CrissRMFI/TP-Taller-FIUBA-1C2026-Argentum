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
    property string raza: "Humano"
    property string clase: "Mago"
    // {2000, 2001, 2002} para Humano
    property int pngCabezaIndexHumano: 2000
    // {2010, 2011, 2012} para Elfo
    property int pngCabezaIndexElfo: 2010
    // {2020, 2021, 2022} para Enano
    property int pngCabezaIndexEnano: 2020
    // {2030, 2031} para Gnomo
    property int pngCabezaIndexGnomo: 2030

    // {2100} para Humano
    property int pngCuerpoIndexHumano: 2100
    // {2110} para Elfo
    property int pngCuerpoIndexElfo: 2110
    // {2120} para Enano
    property int pngCuerpoIndexEnano: 2120
    // {2130} para Gnomo
    property int pngCuerpoIndexGnomo: 2130

    property int pngCuerpoIndex: {
        switch (raza) {
            case "Elfo": return pngCuerpoIndexElfo;
            case "Enano": return pngCuerpoIndexEnano;
            case "Gnomo": return pngCuerpoIndexGnomo;
            case "Humano":
            default: return pngCuerpoIndexHumano;
        }
    }
    property int pngCabezaIndex: {
        switch (raza) {
            case "Elfo": return pngCabezaIndexElfo;
            case "Enano": return pngCabezaIndexEnano;
            case "Gnomo": return pngCabezaIndexGnomo;
            case "Humano":
            default: return pngCabezaIndexHumano;
        }
    }

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
        // property bool checked
        // source: `../graficos/${sourceBaseName}${checked ? "-Checked" : "@2x"}.png`    }
        source: `../graficos/${sourceBaseName}.png`    }

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

    Text {
        id: nombreLabel
        x: nickInput.x - width / 2 + nickInput.width / 2
        y: nickInput.y - height - 5
        text: qsTr("NUEVO NOMBRE:")
        color: "white"
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

    Text {
        id: passwordLabel
        x: passwordInput.x - width / 2 + passwordInput.width / 2
        y: passwordInput.y - height - 5
        text: qsTr("NUEVA CONTRASEÑA:")
        color: "white"
    }

    Text {
        id: razaLabel
        x: parent.width / 4 - width / 2
        y: passwordInput.y + (passwordInput.y - nickInput.y)
        text: qsTr("RAZA:")
        color: "white"
    }

    // Menu desplegable para seleccionar la raza
    ComboBox {
        id: razaDesplegable
        x: razaLabel.x + razaLabel.width / 2 - width / 2
        y: razaLabel.y + razaLabel.height + 5
        width: 150
        model: ["Humano", "Elfo", "Enano", "Gnomo"]
        onCurrentTextChanged: {
            raza = currentText;
        }
    }

    Text {
        id: claseLabel
        x: parent.width / 4 - width / 2
        y: razaLabel.y + (razaLabel.y - passwordInput.y)
        text: qsTr("CLASE:")
        color: "white"
    }

    // Menu desplegable para seleccionar la clase
    ComboBox {
        id: claseDesplegable
        x: claseLabel.x + claseLabel.width / 2 - width / 2
        y: claseLabel.y + claseLabel.height + 5
        width: 150
        model: ["Mago", "Paladín", "Clérigo", "Guerrero"]
        onCurrentTextChanged: {
            clase = currentText;
        }
    }

    Image {
        id: characterHead
        x: parent.width / 4 * 3 - width / 2 - 1
        y: razaDesplegable.y
        source: {
            switch (raza) {
                case "Elfo": return `../../assets/imags/cabezas/Elfo/${pngCabezaIndexElfo}.png`;
                case "Enano": return `../../assets/imags/cabezas/Enano/${pngCabezaIndexEnano}.png`;
                case "Gnomo": return `../../assets/imags/cabezas/Gnomo/${pngCabezaIndexGnomo}.png`;
                case "Humano":
                default: return `../../assets/imags/cabezas/Humano/${pngCabezaIndexHumano}.png`;
            }
        }
        sourceClipRect: Qt.rect(0, 0, 16, 15)
        width: 32
        height: 30
        smooth: false
    }

    ImageButton {
        id: headLeftButton
        x: characterHead.x - 30
        y: characterHead.y + characterHead.height / 2 - 15
        width: 20
        height: 30
        source: "../graficos/leftArrow.png"
        TapHandler {
            onTapped: {
                switch (raza) {
                    case "Elfo":
                        pngCabezaIndexElfo = pngCabezaIndexElfo > 2010 ? pngCabezaIndexElfo - 1 : 2012;
                        break;
                    case "Enano":
                        pngCabezaIndexEnano = pngCabezaIndexEnano > 2020 ? pngCabezaIndexEnano - 1 : 2022;
                        break;
                    case "Gnomo":
                        pngCabezaIndexGnomo = pngCabezaIndexGnomo > 2030 ? pngCabezaIndexGnomo - 1 : 2031;
                        break;
                    case "Humano":
                    default:
                        pngCabezaIndexHumano = pngCabezaIndexHumano > 2000 ? pngCabezaIndexHumano - 1 : 2002;
                        break;
                }
            }
        }
    }

    ImageButton {
        id: headRightButton
        x: characterHead.x + characterHead.width + 10
        y: characterHead.y + characterHead.height / 2 - 15
        width: 20
        height: 30
        source: "../graficos/rightArrow.png"
        TapHandler {
            onTapped: {
                switch (raza) {
                    case "Elfo":
                        pngCabezaIndexElfo = pngCabezaIndexElfo < 2012 ? pngCabezaIndexElfo + 1 : 2010;
                        break;
                    case "Enano":
                        pngCabezaIndexEnano = pngCabezaIndexEnano < 2022 ? pngCabezaIndexEnano + 1 : 2020;
                        break;
                    case "Gnomo":
                        pngCabezaIndexGnomo = pngCabezaIndexGnomo < 2031 ? pngCabezaIndexGnomo + 1 : 2030;
                        break;
                    case "Humano":
                    default:
                        pngCabezaIndexHumano = pngCabezaIndexHumano < 2002 ? pngCabezaIndexHumano + 1 : 2000;
                        break;
                }
            }
        }
    }

    Image {
        id: characterBody
        x: parent.width / 4 * 3 - width / 2
        y: characterHead.y + characterHead.height
        source: {
            switch (raza) {
                case "Elfo": return `../../assets/imags/cuerpos/Elfo/${pngCuerpoIndexElfo}.png`;
                case "Enano": return `../../assets/imags/cuerpos/Enano/${pngCuerpoIndexEnano}.png`;
                case "Gnomo": return `../../assets/imags/cuerpos/Gnomo/${pngCuerpoIndexGnomo}.png`;
                case "Humano":
                default: return `../../assets/imags/cuerpos/Humano/${pngCuerpoIndexHumano}.png`;
            }
        }
        sourceClipRect: raza == "Enano" || raza == "Gnomo" ? Qt.rect(0, 15, 25, 28) : Qt.rect(0, 7, 25, 38)
        width: 50
        height: raza == "Enano" || raza == "Gnomo" ? 50 : 64
        smooth: false
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
                    personajeController.setCabeza(pngCabezaIndex);
                    personajeController.setCuerpo(pngCuerpoIndex);
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
}