# Argentum Online — Cámara y movimiento del jugador

## Qué es la cámara

La cámara es **cenital** (vista desde arriba) y sigue al jugador local: lo mantiene centrado
en el área de juego y hace *scroll* del mundo a medida que se mueve. Soporta **zoom**
(cuántos tiles entran en pantalla) con `Ctrl + rueda`. Vive en
[`client/camara/player_camera.h`](../client/camara/player_camera.h) /
[`.cpp`](../client/camara/player_camera.cpp) y la usa el renderer
[`ObjectRenderer`](../client/interface/renderer/client_renderer.cpp).

La lógica de juego es **independiente del render**: el servidor manda posiciones en **tiles
enteros** y mueve al jugador un tile por tick (`tick_ms = 130` en
[`config/game_config.toml`](../config/game_config.toml)). La cámara y el suavizado son puramente
del cliente: no tocan red, protocolo ni gameplay.

---

## Posición visual continua del jugador

El cliente mantiene una **posición visual continua del jugador local** (`vis_player_x/y`, en
`double`) que avanza hacia su tile destino a **velocidad constante igual a la del servidor**. La
cámara se centra en esa posición continua y el jugador se dibuja en ella, de modo que jugador,
cámara y terreno se mueven juntos y alineados. Las **demás entidades** (otros jugadores,
criaturas) se dibujan en su tile y acompañan el scroll del mundo.

### Función clave

[`ObjectRenderer::actualizar_pos_visual(tile_x, tile_y, now_tick)`](../client/interface/renderer/client_renderer.cpp)
actualiza la posición visual cada frame:

```
velocidad = 1000 / walk_tile_ms     (tiles por segundo; walk_tile_ms = intervalo de movimiento)
paso      = velocidad * dt           (dt acotado a 0.1 s)

restante  = |tile_x - vis_x| + |tile_y - vis_y|
  restante < EPS    -> quieto, no se mueve
  restante > 2.5    -> teleport (muerte/resurrección/login): snap directo
  restante > 1.5    -> paso *= 2  (catch-up cuando quedó atrás)

avanzar UN solo eje por vez (camino en "L"):
  primero se termina el eje que ya venía en curso (vis con parte fraccionaria),
  recién después se mueve el otro.
```

El avance **por un solo eje** traza el recorrido real del jugador (p. ej. derecha y *después*
abajo) en los cambios de dirección.

### Integración en el render

En [`ObjectRenderer::render(...)`](../client/interface/renderer/client_renderer.cpp):

```
camera.configure(area_juego, mapa)                 // viewport + tamaño de mapa
actualizar_pos_visual(player_x, player_y, tick)    // posición visual continua
camera.center_on_point(vis_player_x, vis_player_y) // cámara sobre la posición continua
...
// al dibujar entidades, el jugador local usa su posición visual:
entity_x = (id == client_id) ? scrX(vis_player_x) : scrX(entity.x)
```

`center_on_point` (fraccionaria) es la que usa el render. En los bordes del mapa la cámara se
clampa y el jugador se desplaza hacia el borde de la pantalla.

---

## Zoom

`visible_tiles_x/y` es la cantidad de tiles que entran en pantalla (más tiles = más alejado).
`recalculate_scale()` deriva el tamaño de celda en píxeles. `zoom_in()` / `zoom_out()` varían
esa cantidad entre `min_visible_tiles_*` y `max_visible_tiles_*`, de a `zoom_step_tiles`.

---

## Configuración (sin hardcodear)

Todo lo configurable de la cámara vive en [`config/client_config.toml`](../config/client_config.toml):

```toml
[movimiento]
# cadencia del paso del jugador; debe coincidir con tick_ms del server (game_config.toml).
intervalo_ms = 130

[camara]
tiles_visibles_x = 35   # zoom inicial (tiles que entran en pantalla)
tiles_visibles_y = 30
zoom_min_tiles_x = 12   # límites del zoom
zoom_min_tiles_y = 10
zoom_max_tiles_x = 60
zoom_max_tiles_y = 50
zoom_paso_tiles  = 2    # cuántos tiles cambia por paso de zoom
```

### Flujo de la configuración

```
config/client_config.toml  [camara] / [movimiento]
  └─ LectorConfigCliente::cargar()      (client/config/lector_config_cliente.cpp)
       └─ ConfigCliente { ConfigCamara camara; uint32_t intervaloMovimientoMs; ... }
            └─ ClientGameLoop  ->  ObjectRenderer::init(..., config.camara, intervaloMovimientoMs)
                 ├─ PlayerCamera::aplicar_config(camara)   // zoom y límites
                 └─ walk_tile_ms = intervaloMovimientoMs   // cadencia del suavizado
```

- Los **valores por defecto** (fallback si falta el TOML o una clave) viven **una sola vez** en el
  struct [`ConfigCamara`](../client/camara/player_camera.h). El loader usa `value_or(...)`, así el
  TOML manda cuando está presente.
- Los miembros de `PlayerCamera` son placeholders neutros: los valores reales se cargan en
  `aplicar_config()` al iniciar.

Quedan como `constexpr` internos en `actualizar_pos_visual` solo los *safeguards* numéricos del
algoritmo (clamp de `dt` 0.1 s, umbral de teleport 2.5, catch-up 1.5 / ×2).

---

## Cómo ajustar

| Quiero… | Toco… |
|---|---|
| Más/menos zoom inicial | `[camara] tiles_visibles_x/y` |
| Cambiar el rango de zoom | `[camara] zoom_min/max_tiles_*`, `zoom_paso_tiles` |
| Que el suavizado calce con el server | `[movimiento] intervalo_ms` = `tick_ms` del server |

`intervalo_ms` debe coincidir con el `tick_ms` del servidor; mantenerlos iguales.

---

## Decisiones de diseño

- **El suavizado es 100% del cliente**: no cambia el protocolo ni la lógica del servidor, que
  sigue moviendo en tiles enteros.
- **Solo se suaviza el avatar local**; las entidades remotas se dibujan en su tile (no se usa
  interpolación de red).
- Los parámetros de cámara y movimiento van por TOML, con los defaults en una única fuente
  (`ConfigCamara`).
