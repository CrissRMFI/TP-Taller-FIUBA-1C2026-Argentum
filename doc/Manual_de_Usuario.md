# Manual del Usuario — Argentum Online (FIUBA)

Recreación del clásico _Argentum Online_: un **servidor** multijugador, un
**cliente gráfico** (SDL2) y un **editor de mapas** (Qt). Este manual está
pensado para alguien que **no conoce C++**: si seguís los pasos vas a poder
compilar, instalar, levantar el servidor, conectarte con el cliente y jugar.

---

## 1. Requisitos del sistema

### Sistema operativo

- **Linux** — probado en **Ubuntu 22.04 / 24.04 LTS** (y derivados Debian).
  El proyecto usa herramientas de la cátedra y librerías que se compilan para
  Linux; no está soportado Windows ni macOS de forma oficial.

### Herramientas de compilación (con versiones mínimas)

| Herramienta    | Versión mínima                        | Para qué                            |
| -------------- | ------------------------------------- | ----------------------------------- |
| **g++ / GCC**  | 11 o superior (con soporte **C++20**) | compilar el código                  |
| **CMake**      | **3.24** o superior                   | sistema de build                    |
| **Git**        | cualquiera reciente                   | CMake descarga dependencias por red |
| **pkg-config** | cualquiera                            | localizar librerías del sistema     |
| **Make**       | cualquiera                            | atajo opcional (`make`)             |

### Dependencias del sistema (paquetes `apt`)

- **Interfaz del editor:** `qt6-base-dev`, `qt6-declarative-dev` (Qt 6).
- **Audio y fuentes** (las necesita SDL_mixer / SDL_ttf):
  `libopus-dev`, `libopusfile-dev`, `libxmp-dev`, `libfluidsynth-dev`,
  `fluidsynth`, `libwavpack-dev`, `wavpack`, `libfreetype-dev`.

### Dependencias que se descargan solas

Las siguientes **las baja CMake automáticamente** al compilar (vía
_FetchContent_), no hay que instalarlas a mano:

| Librería     | Versión          | Uso                                |
| ------------ | ---------------- | ---------------------------------- |
| SDL2         | `release-2.30.8` | ventana, render, input del cliente |
| SDL2_image   | `release-2.8.2`  | cargar PNG/BMP                     |
| SDL2_mixer   | `release-2.8.0`  | sonido y música                    |
| SDL2_ttf     | `release-2.22.0` | texto del chat                     |
| libSDL2pp    | commit `cc198c9` | wrapper C++ de SDL2 (RAII)         |
| tomlplusplus | `v3.4.0`         | leer la configuración TOML         |
| googletest   | commit `03597a0` | tests unitarios                    |

La primera compilación tarda más porque descarga y compila estas librerías.

---

## 2. Instalación automática (recomendada) — el instalador

El proyecto incluye un **instalador** que hace todo de una sola vez: instala las
dependencias del sistema, compila, corre los tests e instala los binarios,
assets y configuración en las rutas estándar del usuario.

Desde la raíz del proyecto:

```bash
./install.sh
```

Esto deja todo listo en:

| Qué                        | Dónde                                                                |
| -------------------------- | -------------------------------------------------------------------- |
| Binarios                   | `~/.local/bin/argentum-server`, `argentum-client`, `argentum-editor` |
| Configuración              | `~/.config/argentum/`                                                |
| Assets (imágenes, sonidos) | `~/.local/share/argentum/resources/`                                 |
| Datos de jugadores         | `~/.local/share/argentum/jugadores.bin`                              |

Opciones útiles:

```bash
./install.sh --no-deps    # no instala paquetes apt (si ya los tenés)
./install.sh --no-tests   # no corre los tests
NAME=otro ./install.sh    # cambia el nombre de instalación
./install.sh --help
```

También está disponible como `make install` (y `make uninstall`).

> Asegurate de tener `~/.local/bin` en tu `PATH`. Si al escribir
> `argentum-server` dice "command not found", agregá esa carpeta al PATH
> (`export PATH="$HOME/.local/bin:$PATH"`).

### Desinstalar

```bash
./uninstall.sh           # borra binarios, assets y configuración
./uninstall.sh --purge   # además borra los datos de jugadores
```

---

## 3. Compilación manual (para desarrollo)

Si preferís compilar a mano sin instalar en el sistema:

**1. Herramientas base:**

```bash
sudo apt install build-essential cmake git pkg-config
```

**2. Qt (editor):**

```bash
sudo apt install qt6-base-dev qt6-declarative-dev
```

**3. Audio y fuentes:**

```bash
sudo apt install libopus-dev libopusfile-dev libxmp-dev \
    libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev wavpack \
    libfreetype-dev
```

**4. Compilar:**

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

Esto genera tres ejecutables dentro de `build/`:
`taller_server`, `taller_client`, `taller_editor` (y `taller_tests`).

**5. Correr los tests (opcional):**

```bash
./build/taller_tests
```

---

## 4. Configuración del software

Toda la configuración vive en **archivos TOML de texto** (no hay que recompilar
para cambiar valores). En modo desarrollo se leen de la carpeta `config/`; con
el instalador, de `~/.config/argentum/`.

| Archivo              | Qué configura                                                                                                                                                            |
| -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `game_config.toml`   | **Servidor:** puerto-tick, factores de vida/maná por clase y raza, fórmulas de experiencia/oro, reglas de combate y fair-play, rutas de persistencia, tiempo de resurrección. |
| `client_config.toml` | **Cliente:** resolución de ventana, VSync/FPS, fuente y aspecto del chat, panel lateral, intervalo de movimiento.                                                        |
| `mapa.toml`          | El mapa del mundo (zonas, paredes, ciudades, NPCs). Es **texto**, editable a mano o con el editor.                                                                       |
| `criaturas.toml`     | Catálogo de criaturas/enemigos (compartido por servidor y editor).                                                                                                       |
| `pisos.toml`         | Catálogo de pisos/terrenos para el editor.                                                                                                                               |

### Recursos (imágenes y sonidos)

Los assets del cliente están en `client/resources/` (modo desarrollo) o en
`~/.local/share/argentum/resources/` (instalado). Por ejemplo:

- `resources/imgs/...` — sprites de personajes, criaturas, NPCs, UI, auras.
- `resources/sound/...` — efectos y música.
- `resources/config/sprites.toml` y `sonidos.toml` — mapean cada id/clave a su
  archivo (editables sin recompilar).

> Si agregás un sprite o sonido nuevo, dejá el archivo en la carpeta
> correspondiente y registralo en `sprites.toml` / `sonidos.toml`.

### Persistencia de jugadores

El servidor guarda el progreso de cada personaje en **archivos binarios**
(`jugadores.bin` + un índice). Se carga al conectarse, se guarda al
desconectarse, periódicamente (cada `guardado_seg`) y al apagar el servidor.
No hay que tocarlos a mano.

---

## 5. Cómo levantar el servidor

El servidor recibe **un único argumento: el puerto** donde escucha.

Instalado:

```bash
argentum-server 7777
```

En desarrollo (desde la raíz del repo, para que encuentre `config/`):

```bash
./build/taller_server 7777
```

Para **apagarlo** de forma ordenada (guarda a los jugadores conectados),
escribí `q` y presioná **Enter** en la terminal del servidor.

> 📸 _Captura sugerida: terminal mostrando "Servidor escuchando…" → `img/servidor.png`._

---

## 6. Cómo lanzar el cliente

Instalado:

```bash
argentum-client
```

En desarrollo:

```bash
./build/taller_client
```

Al abrir, aparece un **menú gráfico (Qt)** para conectarse:

1. **Conexión:** ingresá el **host** (por ejemplo `localhost`) y el **puerto**
   (el mismo del servidor, p. ej. `7777`).
2. **Crear un personaje nuevo:** elegí un **nombre**, una **raza** (Humano, Elfo,
   Enano, Gnomo) y una **clase** (Guerrero, Paladín, Clérigo, Mago).
3. **Cargar un personaje existente:** ingresá el nombre con el que lo creaste;
   el servidor recupera su progreso guardado.

Tras confirmar, se abre la **ventana del juego**.

![login](../doc/imgs/login.png)
![sesion](../doc/imgs/ventanaSesion.png)
![crear personaje](../doc/imgs/creacionDePersonaje.png)

---

## 7. Cómo se juega

### 7.1 Pantalla del juego

- **Mundo** (centro/izquierda): tu personaje y el resto de entidades.
- **Chat** (arriba a la izquierda): mensajes del juego y entrada de comandos.
- **Panel** (derecha): nombre, raza/clase, nivel y oro, barras de **vida**,
  **maná** y **experiencia**, **equipo**, pestañas de **inventario** / **hechizos**
  y botones de acción (Vender, Equipar, Usar, Curar).

![juego](../doc/imgs/juego.png)

### 7.2 Controles (teclado y mouse)

| Acción                               | Control                                                                                             |
| ------------------------------------ | --------------------------------------------------------------------------------------------------- |
| Moverse                              | **W A S D** o **flechas** (mantené apretado; el personaje camina mientras la tecla esté presionada) |
| Atacar                               | **Click izquierdo** sobre una criatura u otro jugador                                               |
| Seleccionar / interactuar con un NPC | **Click izquierdo** sobre el NPC (abre su lista: comercio, hechizos, etc.)                          |
| Abrir el chat / escribir un comando  | **Click** en el área del chat, escribí y **Enter**                                                  |
| Cerrar el chat                       | **Esc**                                                                                             |
| Zoom de cámara                       | **Ctrl** + **`+`** (acercar) / **Ctrl** + **`-`** (alejar)                                          |
| Scroll de la lista de comercio       | **Rueda del mouse**                                                                                 |
| Meditar                              | tecla **M** (o `/meditar`)                                                                          |
| Resucitar                            | tecla **R** (o `/resucitar`)                                                                        |

### 7.3 Comandos de chat

Se escriben en el chat empezando con `/`. Los que actúan sobre un NPC/jugador
requieren **seleccionarlo antes con un click**.

| Comando                                          | Qué hace                                                                       |
| ------------------------------------------------ | ------------------------------------------------------------------------------ |
| `/meditar`                                       | Recupera maná (no disponible para Guerrero)                                    |
| `/resucitar`                                     | Revive tu personaje (cerca de un sacerdote; el tiempo depende de la distancia) |
| `/curar`                                         | Pide curación al **sacerdote** seleccionado                                    |
| `/listar`                                        | Lista lo que vende el **comerciante**/banquero seleccionado                    |
| `/comprar <objeto>`                              | Compra un objeto (o hechizo) al NPC seleccionado                               |
| `/vender <slot>`                                 | Vende el objeto del slot de inventario indicado                                |
| `/equipar <slot>`                                | Equipa el objeto del slot indicado                                             |
| `/tirar <slot>`                                  | Tira al suelo el objeto del slot indicado                                      |
| `/depositar oro <monto>` · `/depositar <slot>`   | Deposita oro/objeto en el **banco** (banquero seleccionado)                    |
| `/retirar oro <monto>` · `/retirar <objeto>`     | Retira oro/objeto del banco                                                    |
| `@nick mensaje`                                  | Chat **privado** a un jugador (se ve en otro color; el emisor también lo ve)   |
| _texto sin `/` ni `@`_                           | Chat **global** (lo ven todos los jugadores conectados)                        |

### 7.4 Mecánicas

- **Combate:** acercate y hacé click sobre el enemigo. No se puede atacar
  dentro de zona segura (ciudad) ni entre personajes principiantes.
- **Meditar:** quedate quieto y meditá para recuperar maná (aparece un aura).
- **Morir y resucitar:** al morir quedás como **fantasma**. Con `/resucitar`
  revivís: si estás en ciudad es inmediato; fuera de ciudad quedás **inmóvil**
  un tiempo proporcional a la distancia al sacerdote más cercano, con un **aura**
  envolvente y una **barra sobre la cabeza** que se consume mientras transcurre.
- **NPCs:** **comerciante** (comprar/vender), **banquero** (depositar/retirar
  oro e items), **sacerdote** (curar, comprar hechizos, resucitar).
- **Chat:** texto suelto = global; `@nick mensaje` = privado (otro color, el
  emisor también ve su mensaje). El juego además muestra avisos del sistema
  (muerte, compras, subir de nivel, entrada/salida de mazmorra, conexión y
  desconexión de jugadores).

### 7.5 Cheats de prueba (teclas de función)

Pensados para probar el juego rápido:

| Tecla  | Efecto            |
| ------ | ----------------- |
| **F1** | Vida infinita     |
| **F2** | Maná infinito     |
| **F3** | Morir al instante |
| **F4** | +1000 de oro      |

---

## 8. Cómo usar el editor de mapas

Instalado:

```bash
argentum-editor
```

En desarrollo:

```bash
./build/taller_editor
```

El editor (Qt) permite **crear y editar mapas** visualmente:

1. Elegí un elemento del panel (paredes, pisos, criaturas, NPCs: sacerdote,
   banquero, comerciante).
2. **Arrastrá / clickeá** sobre la grilla para colocarlo; volvé a clickear para
   borrar.
3. Definí **zonas seguras** (ciudades) como rectángulos.
4. Guardá el mapa a un archivo **`mapa.toml`**, que luego usa el servidor.

## ![editor mapa](../doc/imgs/editorMapa.png)

## 9. Problemas comunes

| Síntoma                                       | Solución                                                                                                                       |
| --------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| `command not found: argentum-...`             | Agregá `~/.local/bin` al `PATH`.                                                                                               |
| Falla el link de `libxmp` (ruta con espacios) | Reconfigurá con `-DSDL2MIXER_MOD=OFF`.                                                                                         |
| El cliente no conecta                         | Verificá que el servidor esté corriendo y que host/puerto coincidan.                                                           |
| No se ve el chat / no se cargan sprites       | Corré los binarios desde la raíz del repo (modo desarrollo) o usá el instalador, para que encuentren `config/` y `resources/`. |
| No hay sonido                                 | Verificá las dependencias de audio del paso 1/3. El juego funciona igual sin audio.                                            |

---

\_Para detalles de arquitectura y protocolo, ver [documentacion tecnica](../doc/Documentacion_Tecnica.md)
