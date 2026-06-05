#!/usr/bin/env bash
#
# Desinstalador de Argentum Online: borra lo que instalo install.sh.
# NO borra los datos de jugadores por defecto (usar --purge para eso).
#
# Uso:
#   ./uninstall.sh            # borra binarios, assets y config
#   ./uninstall.sh --purge    # ademas borra los datos de jugadores (jugadores.bin)
#   NAME=otronombre ./uninstall.sh
#
set -euo pipefail

NAME="${NAME:-argentum}"
BIN_DIR="${HOME}/.local/bin"
DATA_DIR="${HOME}/.local/share/${NAME}"
CONFIG_DIR="${HOME}/.config/${NAME}"

PURGE=0
[ "${1:-}" = "--purge" ] && PURGE=1

log() { printf '\033[1;34m[uninstall]\033[0m %s\n' "$*"; }

log "Borrando binarios..."
rm -f "${BIN_DIR}/${NAME}-server" "${BIN_DIR}/${NAME}-client" "${BIN_DIR}/${NAME}-editor"

log "Borrando configuracion (${CONFIG_DIR})..."
rm -rf "${CONFIG_DIR}"

if [ "${PURGE}" -eq 1 ]; then
    log "Borrando TODO el data dir, incluidos datos de jugadores (${DATA_DIR})..."
    rm -rf "${DATA_DIR}"
else
    log "Borrando assets pero conservando datos de jugadores..."
    rm -rf "${DATA_DIR}/resources"
    log "Datos de jugadores conservados en ${DATA_DIR} (usar --purge para borrarlos)."
fi

log "Listo."
