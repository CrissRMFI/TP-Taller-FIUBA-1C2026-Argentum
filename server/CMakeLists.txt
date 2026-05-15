target_sources(taller_server
    PRIVATE
    # .cpp files
    main.cpp
    aceptador/aceptador.cpp
    cliente/cliente.cpp
    gameloop/monitor_clientes.cpp
    protocolo/protocolo_servidor.cpp
    PUBLIC
    # .h files
    aceptador/aceptador.h
    cliente/cliente.h
    gameloop/comando_cliente.h
    gameloop/mensaje_salida.h
    gameloop/monitor_clientes.h
    protocolo/protocolo_servidor.h
    )