target_sources(taller_server
    PRIVATE
    # Punto de entrada
    main.cpp

    # Configuración
    game/config/config_juego.cpp
    game/config/lector_config_toml.cpp
    game/config/parser_toml.cpp

    # Modelo del juego
    game/jugador.cpp
    game/juego.cpp

    # Objetos
    game/objeto/inventario.cpp
    game/objeto/equipamiento.cpp
    game/objeto/catalogo_items.cpp

    # Gameloop
    gameloop/gameloop.cpp
    gameloop/monitor_clientes.cpp

    # Protocolo servidor
    protocolo/protocolo_servidor.cpp

    PUBLIC
    # Modelos planos
    game/modelo/posicion.h
    game/modelo/raza.h
    game/modelo/clase_personaje.h

    # Configuración
    game/config/config_juego.h
    game/config/lector_config_toml.h
    game/config/parser_toml.h

    # Objetos
    game/objeto/item.h
    game/objeto/inventario.h
    game/objeto/equipamiento.h
    game/objeto/catalogo_items.h

    # Juego
    game/clan.h
    game/criatura.h
    game/jugador.h
    game/juego.h

    # Gameloop
    gameloop/comando_cliente.h
    gameloop/evento_sesion.h
    gameloop/gameloop.h
    gameloop/mensaje_salida.h
    gameloop/monitor_clientes.h

    # Protocolo servidor
    protocolo/protocolo_servidor.h
)
