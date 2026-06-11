target_sources(taller_tests PRIVATE
    test_utils.cpp
    test_comandos.cpp
    test_mensajes.cpp
    test_errores.cpp
    ${CMAKE_SOURCE_DIR}/client/protocolo/protocolo_cliente.cpp
    ${CMAKE_SOURCE_DIR}/server/protocolo/protocolo_servidor.cpp
)