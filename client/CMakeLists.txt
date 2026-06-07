target_include_directories(taller_client PRIVATE
        ${sdl2_image_SOURCE_DIR}/include
        ${sdl2_SOURCE_DIR}/include
)
if(NOT DEFINED CLIENT_ASSETS_DIR)
    set(CLIENT_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/interface")
endif()
if(NOT DEFINED ARGENTUM_CONFIG_DIR)
    set(ARGENTUM_CONFIG_DIR "config")
endif()

target_compile_definitions(taller_client PRIVATE
        CLIENT_ASSETS_DIR="${CLIENT_ASSETS_DIR}"
        CLIENT_CONFIG_PATH="${ARGENTUM_CONFIG_DIR}/client_config.toml"
        CLIENT_GAME_CONFIG_PATH="${ARGENTUM_CONFIG_DIR}/game_config.toml"
        CLIENT_MAP_PATH="${ARGENTUM_CONFIG_DIR}/mapa.toml"
)

target_sources(taller_client
    PRIVATE
        main.cpp
        client__.cpp
        client_business.cpp
        client_game_loop.cpp
        client_manager.cpp
        client_receiver.cpp
        client_sender.cpp
        config/lector_config_cliente.cpp
        config/cargador_nombres_item.cpp
        handlers/parser_comando_chat.cpp
        Qt/conectionController.cpp
        Qt/menuController.cpp   
        Qt/login/loginController.cpp
        Qt/elegirPersonaje/elegirPersonajeController.cpp
        Qt/cargarPersonaje/cargarPersonajeController.cpp
        handlers/client_input_handler.cpp
        protocolo/protocolo_cliente.cpp
        audio/gestor_audio.cpp
        # .h
        client__.h
        client_business.h
        client_game_loop.h
        client_manager.h
        client_receiver.h
        client_sender.h
        handshake_error.h
        Qt/conectionController.h
        Qt/menuController.h
        Qt/login/loginController.h
        Qt/elegirPersonaje/elegirPersonajeController.h
        Qt/cargarPersonaje/cargarPersonajeController.h
        handlers/client_input_handler.h
        handlers/parser_comando_chat.h
        audio/gestor_audio.h
        protocolo/protocolo_cliente.h
        config/config_cliente.h
        config/lector_config_cliente.h
        config/cargador_nombres_item.h
    PUBLIC
        Qt/login/login.qml
        Qt/elegirPersonaje/elegirPersonaje.qml
        Qt/cargarPersonaje/cargarPersonaje.qml
)
