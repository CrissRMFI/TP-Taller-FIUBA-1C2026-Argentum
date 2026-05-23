target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    protocolo/protocolo_cliente.cpp
    red/conexion_servidor.cpp
    red/receptor.cpp
    red/emisor.cpp
    PUBLIC
    # .h files
    protocolo/protocolo_cliente.h
    red/conexion_servidor.h
    red/receptor.h
    red/emisor.h
    )
