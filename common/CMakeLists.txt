target_sources(taller_common
    PRIVATE
    # .cpp files
    foo.cpp
    mensajes/mensajes_error_accion.cpp
    mensajes/mensajes_error_audio.cpp
    mensajes/mensajes_error_cliente.cpp
    mensajes/mensajes_error_persistencia.cpp
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
    game/mapa/mundo.cpp

    # Persistencia binaria de mapas (.bin)
    persistencia/error_persistencia.cpp
    persistencia/escritor_mapa.cpp
    persistencia/lector_mapa.cpp
    persistencia/catalogo_criaturas.cpp

    PUBLIC
    # .h files
    foo.h

    mensajes/codigo_error_accion.h
    mensajes/codigo_error_audio.h
    mensajes/codigo_error_cliente.h
    mensajes/codigo_error_persistencia.h
    mensajes/codigo_error_protocolo.h
    mensajes/mensajes_error_accion.h
    mensajes/mensajes_error_audio.h
    mensajes/mensajes_error_cliente.h
    mensajes/mensajes_error_persistencia.h
    mensajes/mensajes_error_protocolo.h

    protocolo/comando_jugador.h
    protocolo/dato_sesion_cliente.h
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
    game/mapa/mapa.h
    game/mapa/mundo.h
    game/mapa/portal.h

    # Persistencia de mapas (escenario en TOML)
    persistencia/error_persistencia.h
    persistencia/escritor_mapa.h
    persistencia/lector_mapa.h
    persistencia/catalogo_criaturas.h

    # Persistencia binaria de jugadores
    persistencia/formato_jugador.h)
