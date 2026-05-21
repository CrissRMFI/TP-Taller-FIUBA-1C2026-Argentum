target_sources(taller_common
    PRIVATE
    # .cpp files
    foo.cpp
    mensajes/mensajes_error_accion.cpp
    mensajes/mensajes_error_protocolo.cpp
    protocolo/protocolo.cpp
    socket/liberror.cpp
    socket/resolver.cpp
    socket/resolvererror.cpp
    socket/socket.cpp
    PUBLIC
    # .h files
    foo.h
    mensajes/codigo_error_accion.h
    mensajes/codigo_error_protocolo.h
    mensajes/mensajes_error_accion.h
    mensajes/mensajes_error_protocolo.h
    protocolo/comando_jugador.h
    protocolo/mensaje_servidor.h
    protocolo/opcode.h
    protocolo/protocolo.h
    socket/liberror.h
    socket/resolver.h
    socket/resolvererror.h
    socket/socket.h
    )
