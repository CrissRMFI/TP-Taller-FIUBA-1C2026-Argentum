target_sources(taller_server
    PRIVATE
    # Punto de entrada
    main.cpp

    # Configuración
    game/config/config_juego.cpp

    PUBLIC
    # Modelo
    game/modelo/posicion.h
    game/modelo/raza.h
    game/modelo/clase_personaje.h

    # Configuración
    game/config/config_juego.h

    # Juego
    game/juego.h

    # Gameloop
    gameloop/comando_cliente.h
    gameloop/gameloop.h
    gameloop/mensaje_salida.h
    gameloop/monitor_clientes.h

    # Protocolo servidor
    protocolo/protocolo_servidor.h
)

target_sources(taller_server
    PRIVATE
    game/juego.cpp
    gameloop/gameloop.cpp
    protocolo/protocolo_servidor.cpp
)
