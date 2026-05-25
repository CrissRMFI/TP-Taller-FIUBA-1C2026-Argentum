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

    # Modelo del dominio compartido entre server, editor y persistencia
    game/aleatorio.cpp
    game/criatura.cpp
    game/npc/npc.cpp
    game/npc/sacerdote.cpp
    game/npc/comerciante.cpp
    game/npc/banquero.cpp
    game/mapa/mapa.cpp

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

    # Modelo del dominio compartido
    game/aleatorio.h
    game/criatura.h
    game/modelo/posicion.h
    game/npc/npc.h
    game/npc/sacerdote.h
    game/npc/comerciante.h
    game/npc/banquero.h
    game/mapa/mapa.h)
