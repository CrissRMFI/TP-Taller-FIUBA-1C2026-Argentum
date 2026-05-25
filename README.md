# TP-Taller-FIUBA-1C2026-Argentum
# Correr la interfaz de QT

1. Instalar herramientas base de compilacion:

sudo apt install build-essential cmake

2. Instalar dependencias del sistema:

sudo apt install qt6-base-dev qt6-declarative-dev

3. Instalar dependencias de audio (requeridas por SDL_mixer):

sudo apt install libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack

4. Compilar:

cmake -S . -B build
make -C build taller_client -j$(nproc)

5. Correr:

./build/taller_client