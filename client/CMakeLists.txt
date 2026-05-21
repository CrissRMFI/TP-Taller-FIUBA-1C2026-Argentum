target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    client__.cpp
    client_business.cpp
	    client_manager.cpp
	    client_receiver.cpp
	    client_sender.cpp
	    client_game_loop.cpp
	    handlers/client_input_handler.cpp
	    protocolo/protocolo_cliente.cpp
    PUBLIC
    # .h files
    client__.h
    client_business.h
	    client_manager.h
	    client_receiver.h
	    client_sender.h
	    client_game_loop.h
	    handlers/client_input_handler.h
	    protocolo/protocolo_cliente.h
	    )
