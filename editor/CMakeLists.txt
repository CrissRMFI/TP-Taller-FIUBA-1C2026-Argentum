target_sources(taller_editor
    PRIVATE
    # .cpp files
    main.cpp
    editor_mapa.cpp
    mapa_canvas.cpp
    editor_window.cpp
    catalogo_editor.cpp
    panel_elementos.cpp
    # Qt resources (embebe bmp, flechas e imagenes de criaturas/npc).
    recursos.qrc
    PUBLIC
    # .h files
    editor_mapa.h
    mapa_canvas.h
    editor_window.h
    catalogo_editor.h
    panel_elementos.h
    )
