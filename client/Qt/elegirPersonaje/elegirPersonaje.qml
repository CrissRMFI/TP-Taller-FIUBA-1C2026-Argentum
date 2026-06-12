import QtQuick
import QtQuick.Controls
import QmlCppExample

// Root que llena la QQuickView; el contenido (640x480) se escala a la ventana.
Item {
    id: root

    property string raza: "Humano"
    property string clase: "Mago"

    property var skinCatalog: [
        {
            raza: "Humano",
            cabezaInicio: 2000,
            cabezaFin: 2002,
            cuerpoInicio: 2100,
            cuerpoFin: 2100,
            cuerpoClipRect: Qt.rect(0, 7, 25, 38),
            cuerpoHeight: 64
        },
        {
            raza: "Elfo",
            cabezaInicio: 2010,
            cabezaFin: 2012,
            cuerpoInicio: 2110,
            cuerpoFin: 2110,
            cuerpoClipRect: Qt.rect(0, 7, 25, 38),
            cuerpoHeight: 64
        },
        {
            raza: "Enano",
            cabezaInicio: 2020,
            cabezaFin: 2022,
            cuerpoInicio: 2120,
            cuerpoFin: 2120,
            cuerpoClipRect: Qt.rect(0, 15, 25, 28),
            cuerpoHeight: 50
        },
        {
            raza: "Gnomo",
            cabezaInicio: 2030,
            cabezaFin: 2031,
            cuerpoInicio: 2130,
            cuerpoFin: 2130,
            cuerpoClipRect: Qt.rect(0, 15, 25, 28),
            cuerpoHeight: 50
        }
    ]

    property int cabezaIndex: 2000
    property int cuerpoIndex: 2100

    function skinParaRaza(nombreRaza) {
        for (let i = 0; i < skinCatalog.length; ++i) {
            if (skinCatalog[i].raza === nombreRaza) {
                return skinCatalog[i]
            }
        }

        return skinCatalog[0]
    }

    function circularIndex(actual, inicio, fin, delta) {
        if (delta < 0) {
            return actual > inicio ? actual - 1 : fin
        }

        return actual < fin ? actual + 1 : inicio
    }

    function resetSkinSeleccionada() {
        const skin = skinParaRaza(raza)
        cabezaIndex = skin.cabezaInicio
        cuerpoIndex = skin.cuerpoInicio
    }

    function rotarCabeza(delta) {
        const skin = skinParaRaza(raza)
        cabezaIndex = circularIndex(cabezaIndex, skin.cabezaInicio, skin.cabezaFin, delta)
    }

    function rotarCuerpo(delta) {
        const skin = skinParaRaza(raza)
        cuerpoIndex = circularIndex(cuerpoIndex, skin.cuerpoInicio, skin.cuerpoFin, delta)
    }

    Component.onCompleted: resetSkinSeleccionada()
    onRazaChanged: resetSkinSeleccionada()

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

    Item {
        id: contenido
        width: 640
        height: 480
        transformOrigin: Item.TopLeft
        scale: Math.min(root.width / width, root.height / height)

        Image {
            id: background
            source: "../graficos/ElecciónPersonaje.png"
            anchors.fill: parent
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
            onCurrentTextChanged: root.raza = currentText
            onActivated: audioMenu.reproducirEfecto("volver")
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
            onCurrentTextChanged: root.clase = currentText
            onActivated: audioMenu.reproducirEfecto("volver")
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
            source: personajeController.rutaCabezaPreview(root.cabezaIndex)
        }

        ImageButton {
            id: headLeftButton
            x: characterHead.x - 30
            y: characterHead.y
            width: 20
            height: 30
            source: "../graficos/leftArrow.png"
            TapHandler {
                onTapped: {
                    root.rotarCabeza(-1);
                    audioMenu.reproducirEfecto("volver");
                }
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
                onTapped: {
                    root.rotarCabeza(1);
                    audioMenu.reproducirEfecto("volver");
                }
            }
        }

        Image {
            id: characterBody
            x: 449
            y: characterHead.y + characterHead.height
            width: 50
            height: root.skinParaRaza(root.raza).cuerpoHeight
            smooth: false
            sourceClipRect: root.skinParaRaza(root.raza).cuerpoClipRect
            source: personajeController.rutaCuerpoPreview(root.cuerpoIndex)
        }

        ImageButton {
            id: bodyLeftButton
            x: headLeftButton.x
            y: characterBody.y + 12
            width: 20
            height: 30
            source: "../graficos/leftArrow.png"
            TapHandler {
                onTapped: {
                    root.rotarCuerpo(-1);
                    audioMenu.reproducirEfecto("volver");
                }
            }
        }

        ImageButton {
            id: bodyRightButton
            x: headRightButton.x
            y: characterBody.y + 12
            width: 20
            height: 30
            source: "../graficos/rightArrow.png"
            TapHandler {
                onTapped: {
                    root.rotarCuerpo(1);
                    audioMenu.reproducirEfecto("volver");
                }
            }
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
                    if (nickInput.text !== "" && root.raza !== "" && root.clase !== "") {
                        const esNickValido = personajeController.esNickValido(nickInput.text)
                        nickErrorText.text = !esNickValido ? "El nick no puede tener espacios y debe ser menor o igual a 32 bytes." : ""

                        if (!esNickValido) {
                            audioMenu.reproducirEfecto("error");
                            return
                        }

                        generalErrorText.text = ""
                        personajeController.setRaza(root.raza)
                        personajeController.setClase(root.clase)
                        personajeController.setNick(nickInput.text)
                        personajeController.setCabeza(root.cabezaIndex)
                        personajeController.setCuerpo(root.cuerpoIndex)
                    } else {
                        generalErrorText.text = "Por favor, complete todos los campos para crear el personaje";
                        audioMenu.reproducirEfecto("error");
                    }
                    audioMenu.reproducirEfecto("click")
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
                onTapped: {
                    personajeController.volverAlMenu();
                    audioMenu.reproducirEfecto("volver");
                }
            }
        }
    }
}
