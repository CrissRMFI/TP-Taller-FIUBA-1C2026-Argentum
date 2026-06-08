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

set(SDL2PP_WITH_IMAGE ON CACHE BOOL "" FORCE)
set(SDL2PP_WITH_MIXER ON CACHE BOOL "" FORCE)
set(SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE)
option(TALLER_TESTS "Enable / disable tests." ON)
option(TALLER_CLIENT "Enable / disable client program." ON)
option(TALLER_SERVER "Enable / disable server program." ON)
option(TALLER_EDITOR "Enable / disable editor program." ON)
option(TALLER_MAKE_WARNINGS_AS_ERRORS "Enable / disable warnings as errors." ON)

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Define target: lib common between client, server and editor programs
# HEY TODO XXX: change the name to something that you like
add_library(taller_common)

# add the source files that make the lib
# HEY!! TODO XXX: add more subfolders here
add_subdirectory(common/)


# set compiler warnings to the targets
include(cmake/CompilerWarnings.cmake)
set_project_warnings(taller_common ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

target_include_directories(taller_common PUBLIC .)

include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(tomlplusplus)
target_link_libraries(taller_common PUBLIC tomlplusplus::tomlplusplus)

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

if(TALLER_CLIENT OR TALLER_SERVER)
    include(FetchContent)
    FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG        v3.4.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(tomlplusplus)
endif()

# Program section
# ---------------
if(TALLER_CLIENT)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
        add_executable(taller_client
                client/interface/sprite_manager.cpp
                client/interface/sprite_manager.h
                client/interface/client_game_world.cpp
                client/interface/client_game_world.h
                client/interface/renderer/client_renderer.cpp
                client/interface/renderer/client_renderer.h
                client/interface/object_animation.cpp
                client/interface/object_animation.h
                client/interface/sprites/sprite_catalog.cpp
                client/interface/sprites/sprite_catalog.h
                client/interface/texture_cache.cpp
                client/interface/texture_cache.h
                client/interface/renderer/character_renderer.cpp
                client/interface/renderer/character_renderer.h
                client/client_game_loop.cpp
                client/client_game_loop.h
                client/interface/sprites_resolver/character_sprite_resolver.cpp
                client/interface/sprites_resolver/character_sprite_resolver.h
                client/interface/renderer/criatura_renderer.cpp
                client/interface/renderer/criatura_renderer.h
                client/interface/renderer/npc_renderer.cpp
                client/interface/renderer/npc_renderer.h
                client/interface/sprites_resolver/criatura_sprite_resolver.cpp
                client/interface/sprites_resolver/criatura_sprite_resolver.h
                client/interface/sprites_resolver/npc_sprite_resolver.cpp
                client/interface/sprites_resolver/npc_sprite_resolver.h
                client/interface/sprites/sprite_catalog_helper.cpp
                client/interface/sprites/sprite_catalog_helper.h
                client/interface/sprites/sprite_catalog_parser.cpp
                client/interface/sprites/sprite_catalog_parser.h
                client/interface/sprites/catalog_resources.h
                client/camara/player_camera.cpp
                client/camara/player_camera.h
        )

    add_dependencies(taller_client taller_common SDL2pp::SDL2pp)

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
            client/Qt/graficos/emptybutton.png
            client/Qt/graficos/crearPersonajeButton.png
            client/Qt/graficos/ElecciónPersonaje.png
            client/Qt/graficos/InicioSesion.png
            client/Qt/graficos/jugarButton.png
            client/Qt/graficos/leftArrow.png
            client/Qt/graficos/rightArrow.png
            client/resources/imgs/personajes/cabezas/2000.png
            client/resources/imgs/personajes/cabezas/2001.png
            client/resources/imgs/personajes/cabezas/2002.png
            client/resources/imgs/personajes/cabezas/2010.png
            client/resources/imgs/personajes/cabezas/2011.png
            client/resources/imgs/personajes/cabezas/2012.png
            client/resources/imgs/personajes/cabezas/2020.png
            client/resources/imgs/personajes/cabezas/2021.png
            client/resources/imgs/personajes/cabezas/2022.png
            client/resources/imgs/personajes/cabezas/2030.png
            client/resources/imgs/personajes/cabezas/2031.png
            client/resources/imgs/personajes/cuerpos/2100.png
            client/resources/imgs/personajes/cuerpos/2110.png
            client/resources/imgs/personajes/cuerpos/2120.png
            client/resources/imgs/personajes/cuerpos/2130.png
    )

    target_link_libraries(taller_client PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

    set_project_warnings(taller_client ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(taller_client PRIVATE ${libSDL2pp_SOURCE_DIR})
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt)
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt/login)
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt/elegirPersonaje)
    target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/client/Qt/cargarPersonaje)

    target_link_libraries(taller_client PRIVATE taller_common SDL2pp::SDL2pp tomlplusplus::tomlplusplus)
endif()

if(TALLER_SERVER)
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

    if(NOT DEFINED ARGENTUM_CONFIG_DIR)
        set(ARGENTUM_CONFIG_DIR "config")
    endif()
    target_compile_definitions(taller_server PRIVATE
        SERVER_GAME_CONFIG_PATH="${ARGENTUM_CONFIG_DIR}/game_config.toml")

    target_link_libraries(taller_server taller_common tomlplusplus::tomlplusplus)
endif()

if(TALLER_EDITOR)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

    add_executable(taller_editor)
    set_target_properties(taller_editor PROPERTIES AUTOMOC ON)

    add_dependencies(taller_editor taller_common)

    add_subdirectory(editor)

    set_project_warnings(taller_editor ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    target_include_directories(taller_editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/editor)

    target_link_libraries(taller_editor taller_common Qt6::Core Qt6::Gui Qt6::Widgets)
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

    
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    
    add_executable(taller_tests)

    
    add_dependencies(taller_tests taller_common)

    
    target_include_directories(taller_tests PUBLIC .)

    add_subdirectory(tests/)

    set_project_warnings(taller_tests ${TALLER_MAKE_WARNINGS_AS_ERRORS} TRUE)

    target_include_directories(taller_tests
        PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        )

    target_link_libraries(taller_tests
        taller_common
        GTest::gtest_main
    )
endif()
