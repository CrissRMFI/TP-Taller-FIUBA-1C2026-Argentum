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

    property int pngCabezaIndexHumano: 2000
    property int pngCabezaIndexElfo: 2010
    property int pngCabezaIndexEnano: 2020
    property int pngCabezaIndexGnomo: 2030

    property int pngCuerpoIndexHumano: 2100
    property int pngCuerpoIndexElfo: 2110
    property int pngCuerpoIndexEnano: 2120
    property int pngCuerpoIndexGnomo: 2130

    property int pngCabezaIndex: {
        switch (raza) {
            case "Elfo": return pngCabezaIndexElfo
            case "Enano": return pngCabezaIndexEnano
            case "Gnomo": return pngCabezaIndexGnomo
            case "Humano":
            default: return pngCabezaIndexHumano
        }
    }

    property int pngCuerpoIndex: {
        switch (raza) {
            case "Elfo": return pngCuerpoIndexElfo
            case "Enano": return pngCuerpoIndexEnano
            case "Gnomo": return pngCuerpoIndexGnomo
            case "Humano":
            default: return pngCuerpoIndexHumano
        }
    }

    function rotarCabeza(delta) {
        switch (raza) {
            case "Elfo":
                pngCabezaIndexElfo = delta < 0 ?
                        (pngCabezaIndexElfo > 2010 ? pngCabezaIndexElfo - 1 : 2012) :
                        (pngCabezaIndexElfo < 2012 ? pngCabezaIndexElfo + 1 : 2010)
                break
            case "Enano":
                pngCabezaIndexEnano = delta < 0 ?
                        (pngCabezaIndexEnano > 2020 ? pngCabezaIndexEnano - 1 : 2022) :
                        (pngCabezaIndexEnano < 2022 ? pngCabezaIndexEnano + 1 : 2020)
                break
            case "Gnomo":
                pngCabezaIndexGnomo = delta < 0 ?
                        (pngCabezaIndexGnomo > 2030 ? pngCabezaIndexGnomo - 1 : 2031) :
                        (pngCabezaIndexGnomo < 2031 ? pngCabezaIndexGnomo + 1 : 2030)
                break
            case "Humano":
            default:
                pngCabezaIndexHumano = delta < 0 ?
                        (pngCabezaIndexHumano > 2000 ? pngCabezaIndexHumano - 1 : 2002) :
                        (pngCabezaIndexHumano < 2002 ? pngCabezaIndexHumano + 1 : 2000)
                break
        }
    }

    Image {
        id: background
        source: "../graficos/ElecciónPersonaje.png"
        anchors.fill: parent
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

    component LabelText: Text {
        color: "white"
    }

    TextField {
        id: nickInput
        x: parent.width / 5 * 2 - width / 2
        y: parent.height / 4 - height / 2
        width: 230
        height: 30
        placeholderText: qsTr("Ingrese su nick")
        background: Rectangle {
            color: "transparent"
            border.color: "white"
        }
        text: ""
    }

    LabelText {
        x: nickInput.x
        y: nickInput.y - 24
        text: qsTr("NUEVO NOMBRE:")
    }

    LabelText {
        x: nickInput.x
        y: 202
        text: qsTr("RAZA:")
    }

    ComboBox {
        id: razaDesplegable
        x: nickInput.x
        y: 226
        width: 150
        model: ["Humano", "Elfo", "Enano", "Gnomo"]
        currentIndex: 0
        onCurrentTextChanged: raza = currentText
    }

    LabelText {
        x: nickInput.x
        y: 286
        text: qsTr("CLASE:")
    }

    ComboBox {
        id: claseDesplegable
        x: nickInput.x
        y: 310
        width: 150
        model: ["Mago", "Paladín", "Clérigo", "Guerrero"]
        currentIndex: 0
        onCurrentTextChanged: clase = currentText
    }

    LabelText {
        x: 430
        y: 202
        text: qsTr("SKIN:")
    }

    Image {
        id: characterHead
        x: 457
        y: 236
        width: 32
        height: 30
        smooth: false
        sourceClipRect: Qt.rect(0, 0, 16, 15)
        source: `../../assets/imgs/${pngCabezaIndex}.png`
    }

    ImageButton {
        id: headLeftButton
        x: characterHead.x - 30
        y: characterHead.y
        width: 20
        height: 30
        source: "../graficos/leftArrow.png"
        TapHandler {
            onTapped: rotarCabeza(-1)
        }
    }

    ImageButton {
        id: headRightButton
        x: characterHead.x + characterHead.width + 10
        y: characterHead.y
        width: 20
        height: 30
        source: "../graficos/rightArrow.png"
        TapHandler {
            onTapped: rotarCabeza(1)
        }
    }

    Image {
        id: characterBody
        x: 449
        y: characterHead.y + characterHead.height
        width: 50
        height: raza === "Enano" || raza === "Gnomo" ? 50 : 64
        smooth: false
        sourceClipRect: raza === "Enano" || raza === "Gnomo" ?
                            Qt.rect(0, 15, 25, 28) :
                            Qt.rect(0, 7, 25, 38)
        source: `../../assets/imgs/${pngCuerpoIndex}.png`
    }

    ErrorText {
        id: nickErrorText
        x: nickInput.x + nickInput.width / 2 - width / 2
        y: nickInput.y + nickInput.height - 2
        text: ""
    }

    ErrorText {
        id: generalErrorText
        x: parent.width / 2 - width / 2
        y: crearPersonajeButton.y - height - 5
        text: ""
    }

    ImageButton {
        id: crearPersonajeButton
        x: 369
        y: 420
        width: 205
        height: 50
        source: "../graficos/crearPersonajeButton.png"
        TapHandler {
            onTapped: {
                if (nickInput.text !== "" && raza !== "" && clase !== "") {
                    const esNickValido = personajeController.esNickValido(nickInput.text)

                    nickErrorText.text = !esNickValido ? "El nick no puede tener espacios y debe ser menor o igual a 32 bytes." : ""
    
                    if (!esNickValido) {
                        return
                    }

                    generalErrorText.text = ""
                    personajeController.setRaza(raza)
                    personajeController.setClase(clase)
                    personajeController.setNick(nickInput.text)
                    personajeController.setCabeza(pngCabezaIndex)
                    personajeController.setCuerpo(pngCuerpoIndex)
                } else {
                    generalErrorText.text = "Por favor, complete todos los campos para crear el personaje"
                }
            }
        }
    }

    ImageButton {
        id: volverButton
        x: crearPersonajeButton.x - parent.width / 2 + 2
        y: 420
        width: 210
        height: 47
        source: "../graficos/VolverButton.png"
        TapHandler {
            onTapped: personajeController.volverAlMenu()
        }
    }
}
