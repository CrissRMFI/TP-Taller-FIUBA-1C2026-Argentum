#!/usr/bin/env bash
#
#   1. Descarga/instala las dependencias del sistema.
#   2. Compila el proyecto.
#   3. Corre los tests unitarios.
#   4. Copia los binarios a ~/.local/bin, los assets a ~/.local/share/NAME y la configuracion a ~/.config/NAME.
#
# Las dependencias de C++ (SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, tomlplusplus, googletest) las baja CMake por FetchContent al compilar; este script solo instala las dependencias "del sistema" (Qt6, herramientas de build y las libs que SDL_mixer/ttf necesitan).
#
# Uso:
#   ./install.sh                 # instala todo
#   ./install.sh --no-deps       # saltea el apt install
#   ./install.sh --no-tests      # saltea los tests
#   NAME=otronombre ./install.sh # cambia el nombre de instalacion
#
set -euo pipefail

# ------------------------------------------------------------------
# Configuracion
# ------------------------------------------------------------------
NAME="${NAME:-argentum}"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

BIN_DIR="${HOME}/.local/bin"
DATA_DIR="${HOME}/.local/share/${NAME}"
CONFIG_DIR="${HOME}/.config/${NAME}"

INSTALL_DEPS=1
RUN_TESTS=1
for arg in "$@"; do
    case "$arg" in
        --no-deps)  INSTALL_DEPS=0 ;;
        --no-tests) RUN_TESTS=0 ;;
        -h|--help)
            grep '^#' "${BASH_SOURCE[0]}" | grep -v '^#!' | sed 's/^# \{0,1\}//'
            exit 0 ;;
        *) echo "Opcion desconocida: $arg" >&2; exit 1 ;;
    esac
done

log()  { printf '\033[1;34m[install]\033[0m %s\n' "$*"; }
die()  { printf '\033[1;31m[install] ERROR:\033[0m %s\n' "$*" >&2; exit 1; }

# ------------------------------------------------------------------
# 1. Dependencias del sistema
# ------------------------------------------------------------------
install_deps() {
    log "Instalando dependencias del sistema (requiere sudo)..."
    sudo apt-get update
    sudo apt-get install -y \
        build-essential cmake git pkg-config \
        qt6-base-dev qt6-declarative-dev \
        libopus-dev libopusfile-dev libxmp-dev \
        libfluidsynth-dev fluidsynth \
        libwavpack-dev wavpack \
        libfreetype-dev
}

# ------------------------------------------------------------------
# 2. Compilacion (con las rutas de instalacion horneadas)
# ------------------------------------------------------------------
build() {
    log "Configurando y compilando (Release)..."
    
    cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DTALLER_MAKE_WARNINGS_AS_ERRORS=OFF \
        -DARGENTUM_CONFIG_DIR="${CONFIG_DIR}" \
        -DCLIENT_ASSETS_DIR="${DATA_DIR}/assets"
    cmake --build "${BUILD_DIR}" \
        --target taller_server taller_client taller_editor taller_tests \
        -j"$(nproc)"
}

# ------------------------------------------------------------------
# 3. Tests unitarios
# ------------------------------------------------------------------
run_tests() {
    log "Corriendo tests unitarios..."
    "${BUILD_DIR}/taller_tests"
}

# ------------------------------------------------------------------
# 4. Instalacion de binarios, assets y config
# ------------------------------------------------------------------
install_files() {
    log "Instalando binarios en ${BIN_DIR}"
    install -d "${BIN_DIR}"
    install -m 755 "${BUILD_DIR}/taller_server" "${BIN_DIR}/${NAME}-server"
    install -m 755 "${BUILD_DIR}/taller_client" "${BIN_DIR}/${NAME}-client"
    install -m 755 "${BUILD_DIR}/taller_editor" "${BIN_DIR}/${NAME}-editor"

    log "Instalando assets en ${DATA_DIR}/resources"
    install -d "${DATA_DIR}/resources"
    cp -r "${PROJECT_ROOT}/client/resources/." "${DATA_DIR}/resources/"

    log "Instalando configuracion en ${CONFIG_DIR}"
    install -d "${CONFIG_DIR}"
    install -m 644 "${PROJECT_ROOT}/config/client_config.toml" "${CONFIG_DIR}/"
    install -m 644 "${PROJECT_ROOT}/config/game_config.toml"   "${CONFIG_DIR}/"
    install -m 644 "${PROJECT_ROOT}/config/mapa.toml"          "${CONFIG_DIR}/"


    install -d "${DATA_DIR}"
    local gc="${CONFIG_DIR}/game_config.toml"
    sed -i "s#^[[:space:]]*archivo[[:space:]]*=.*#archivo = \"${CONFIG_DIR}/mapa.toml\"#" "${gc}"
    sed -i "s#^[[:space:]]*archivo_jugadores[[:space:]]*=.*#archivo_jugadores = \"${DATA_DIR}/jugadores.bin\"#" "${gc}"
    sed -i "s#^[[:space:]]*archivo_indice[[:space:]]*=.*#archivo_indice = \"${DATA_DIR}/jugadores_indice.bin\"#" "${gc}"
}

# ------------------------------------------------------------------
final_msg() {
    printf '\n\033[1;32m[install] Listo.\033[0m\n\n'
    cat <<EOF
  Binarios:  ${BIN_DIR}/${NAME}-server, ${NAME}-client, ${NAME}-editor
  Config:    ${CONFIG_DIR}
  Assets:    ${DATA_DIR}/resources

Asegurate de tener ${BIN_DIR} en el PATH. Para correr:
  ${NAME}-server 7777        # en una terminal
  ${NAME}-client             # en otra
EOF
}

main() {
    command -v cmake >/dev/null 2>&1 || [ "${INSTALL_DEPS}" -eq 1 ] || \
        die "cmake no esta instalado y se uso --no-deps."

    [ "${INSTALL_DEPS}" -eq 1 ] && install_deps
    build
    [ "${RUN_TESTS}" -eq 1 ] && run_tests
    install_files
    final_msg
}

main "$@"
