cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS ON)

project(taller_tp
    VERSION 1.0
    HOMEPAGE_URL "https://github.com/URL-HERE"
    LANGUAGES CXX
    )

option(TALLER_TESTS "Enable / disable tests." ON)
option(TALLER_CLIENT "Enable / disable client program." ON)
option(TALLER_SERVER "Enable / disable server program." ON)
option(TALLER_EDITOR "Enable / disable editor program." ON)
option(TALLER_MAKE_WARNINGS_AS_ERRORS "Enable / disable warnings as errors." ON)

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Define target: lib common between client, server and editor programs
# HEY TODO XXX: change the name to something that you like
add_library(taller_common STATIC)

# add the source files that make the lib
# HEY!! TODO XXX: add more subfolders here
add_subdirectory(common/)


# set compiler warnings to the targets
include(cmake/CompilerWarnings.cmake)
set_project_warnings(taller_common ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

# Say where the headers will be found required by library target
# Make this PUBLIC so any dependant linking lib will inherit this
# location (so our dependants will not have to explicitly request this)
target_include_directories(taller_common PUBLIC .)

# HEY!! TODO XXX: you need to install some runtime and dev libraries *before*
# compiling the client/editor code.
#
# In xubuntu 24.04 these are:
#
# sudo apt-get install libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack
#

# Install libSDL2 and libSDL2pp (for client or editor)
# HEY!! TODO XXX: add more dependendies (either for client, server or editor)
if(TALLER_CLIENT OR TALLER_EDITOR)
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-2.8.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-2.22.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        libSDL2pp
        OVERRIDE_FIND_PACKAGE
        URL https://github.com/libSDL2pp/libSDL2pp/archive/cc198c9a5657048bee67ece82de620b2d5661084.zip
    )

    FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer SDL2_ttf libSDL2pp)
endif()

# Program section
# ---------------
if(TALLER_CLIENT)
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    add_executable(taller_client
            client/protocolo/protocolo_cliente.h
            client/protocolo/protocolo_cliente.cpp
            client/handlers/client_input_handler.h
            client/handlers/client_input_handler.cpp
            client/client__.h
            client/client__.cpp
            client/client_business.h
            client/client_business.cpp
            client/client_manager.h
            client/client_manager.cpp
            client/client_receiver.h
            client/client_receiver.cpp
            client/client_sender.h
            client/client_sender.cpp
            client/client_game_loop.h
            client/client_game_loop.cpp
    )

    # Make them depend on taller_common lib
    add_dependencies(taller_client taller_common SDL2pp::SDL2pp)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(client)
    qt_add_qml_module(taller_client
        URI QmlCppExample
        VERSION 1.0
        QML_FILES
            client/Qt/login/login.qml
            client/Qt/elegirPersonaje/elegirPersonaje.qml
            client/Qt/cargarPersonaje/cargarPersonaje.qml
        SOURCES
            client/Qt/login/loginController.cpp
            client/Qt/login/loginController.h
            client/Qt/elegirPersonaje/elegirPersonajeController.cpp
            client/Qt/elegirPersonaje/elegirPersonajeController.h
            client/Qt/cargarPersonaje/cargarPersonajeController.cpp
            client/Qt/cargarPersonaje/cargarPersonajeController.h
        RESOURCES
            client/Qt/graficos/menu.png
            client/Qt/graficos/LED.png
            client/Qt/graficos/LED@2x.png
            client/Qt/graficos/LED-Checked.png
            client/Qt/graficos/LED-Checked@2x.png
            client/Qt/graficos/unirseButton.png
            client/Qt/graficos/crearButton.png
            client/Qt/graficos/ventanalauncher.png
            client/Qt/graficos/VolverButton.png
            client/Qt/graficos/crearPersonajeButton.png
            client/Qt/graficos/ElecciónPersonaje.png
            client/Qt/graficos/InicioSesion.png
            client/Qt/graficos/jugarButton.png
    )

    target_link_libraries(taller_client PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

    set_project_warnings(taller_client ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(taller_client PRIVATE ${libSDL2pp_SOURCE_DIR})
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt)
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt/login)
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt/elegirPersonaje)
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt/cargarPersonaje)

    # Link the taller_common lib target
    target_link_libraries(taller_client PRIVATE taller_common SDL2pp::SDL2pp)
endif()

if(TALLER_SERVER)
    include(FetchContent)
    FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG        v3.4.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(tomlplusplus)

    add_executable(taller_server
            server/server_.cpp
            server/server_.h
            server/enviador.cpp
            server/enviador.h
            server/recibidor.cpp
            server/recibidor.h
            server/aceptador/aceptador.cpp
            server/cliente/cliente.cpp)

    add_dependencies(taller_server taller_common)

    add_subdirectory(server)

    set_project_warnings(taller_server ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    target_link_libraries(taller_server taller_common tomlplusplus::tomlplusplus)
endif()

if(TALLER_EDITOR)
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(taller_editor)

    # Make them depend on taller_common lib
    add_dependencies(taller_editor taller_common SDL2pp::SDL2pp)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(editor)

    set_project_warnings(taller_editor ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(taller_editor PRIVATE ${libSDL2pp_SOURCE_DIR})

    # Link the taller_common lib target
    target_link_libraries(taller_editor taller_common SDL2pp::SDL2pp)
endif()

# Testing section
# ---------------

if(TALLER_TESTS)
    # Let's use google test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    # (borrowed from https://google.github.io/googletest/quickstart-cmake.html)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    # target for testing
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(taller_tests)

    # HEY!! TODO XXX: I'm assuming that the code under testing is in taller_common
    # Change this if you have to.
    add_dependencies(taller_tests taller_common)

    # Say where to find the xoz headers
    # HEY!! TODO XXX: add more subfolders here
    target_include_directories(taller_tests PUBLIC .)

    # Add the source files to the taller_tests target
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(tests/)

    # Set the compiler warnings to the tests target (but be a little lesser strict)
    set_project_warnings(taller_tests ${TALLER_MAKE_WARNINGS_AS_ERRORS} TRUE)

    # Say where googletest headers can be found
    target_include_directories(taller_tests
        PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        )

    # HEY!! TODO XXX: I'm assuming that the code under testing is in taller_common
    # Change this if you have to.
    target_link_libraries(taller_tests
        taller_common
        GTest::gtest_main
    )
endif()
