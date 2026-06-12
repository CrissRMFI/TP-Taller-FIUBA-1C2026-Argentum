# Argentum Online — Gameloop: Arquitectura e Implementación

## Qué es el gameloop

El gameloop es el núcleo del servidor. Es el hilo que mantiene el estado del mundo, recibe los comandos de los clientes, los aplica al estado y envía las respuestas. **Todo el estado del juego vive en un único hilo**, sin concurrencia interna. Los hilos de red depositan comandos en una cola y leen mensajes de salida de otra; el gameloop consume y produce de esas estructuras thread-safe.

---

## Diagrama de arranque

```
main()
└─ LectorConfigToml::cargar(ruta) → ConfigCompleta { juego, items }
└─ toml++ parsea el archivo TOML una única vez
└─ Gameloop(monitor, ConfigCompleta)
└─ Juego(ConfigJuego, CatalogoItems)
└─ Gameloop::run() → hilo del servidor
```

El servidor arranca leyendo una sola vez el archivo `config/game_config.toml`. De ese único parse se construyen dos cosas: la configuración numérica del juego (`ConfigJuego`) y el catálogo de items (`CatalogoItems`). Ambas se pasan al `Gameloop`, que las transfiere a `Juego`.

---

## Carga de configuración

### ConfigJuego

`ConfigJuego` es un struct plano con los parámetros numéricos del juego: factores de vida y maná por clase y raza, fórmulas de experiencia y oro, umbrales de combate, parámetros de clan, cheats de testing y duración del tick del gameloop. Ver [`config/config_juego.h`](../server/game/config/config_juego.h).

No tiene lógica de carga propia. Quien la carga es `LectorConfigToml`.

### LectorConfigToml y toml++

La configuración del juego se carga desde archivos TOML mediante la librería externa `toml++`.

Esta decisión cumple con la restricción del enunciado, que exige utilizar archivos de configuración TOML y no implementar un parser propio. El objetivo es que los valores de balance del juego puedan modificarse sin recompilar, por ejemplo factores de vida, maná, recuperación, experiencia, oro, reglas de clan, combate y duración del tick del gameloop.

La clase `LectorConfigToml` es responsable de leer el archivo de configuración y construir una instancia de `ConfigCompleta`.

### CatalogoItems

`CatalogoItems` registra todos los tipos de items del juego, cargados desde la sección `[items.*]` del mismo TOML. Mapea `uint16_t id → Item*`. Los IDs son estables, definidos en el TOML con el campo `id`, lo que garantiza que el mismo número identifica siempre el mismo tipo de item entre reinicios del servidor.

Cada item tiene un `tipo` en el TOML (`"arma"`, `"baculo"`, `"armadura"`, `"casco"`, `"escudo"`, `"pocion_vida"`, `"pocion_mana"`) que determina qué subclase de `Item` se construye. Ver [`objeto/catalogo_items.h`](../server/game/objeto/catalogo_items.h).

### ConfigCompleta y parseo único

`LectorConfigToml::cargar` devuelve `ConfigCompleta { ConfigJuego juego; CatalogoItems items; }`.

El archivo se abre y parsea una sola vez mediante `toml++`; a partir de esa misma estructura TOML se construyen tanto la configuración del juego como el catálogo de items. Esto evita la doble lectura que habría si `CatalogoItems` tuviera su propio `cargar(ruta)`.

---

## El hilo Gameloop

[`server/gameloop/gameloop.h`](../server/gameloop/gameloop.h)

```
mientras (servidor activo):
1. Procesar eventos de sesión pendientes
2. Procesar todos los comandos pendientes en la cola
3. Llamar a Juego::actualizar(segundos) ← tick del mundo
4. Despachar mensajes de salida a los clientes correspondientes
5. Dormir tickMs milisegundos, leído desde la configuración TOML
```

### Cola de comandos (`Queue<ComandoCliente>`)

Los hilos de red reciben paquetes del cliente y los convierten en `ComandoCliente { idCliente, ComandoJugador }`. Los depositan en `colaComandos` (thread-safe). El gameloop los consume con `try_pop` sin bloquear al inicio de cada tick.

### Eventos de sesión

Además de comandos de juego, el gameloop procesa eventos de sesión, por ejemplo conexión y desconexión de jugadores. Esto permite que el alta y baja de personajes se haga dentro del mismo hilo que modifica el estado del mundo, evitando condiciones de carrera.

### Despacho de mensajes (`MonitorClientes`)

Cada método de `Juego` retorna `std::list<MensajeSalida>`. Un `MensajeSalida` tiene un destino: `TipoDestino::UNO` para un cliente específico o `TipoDestino::TODOS` para broadcast. `Gameloop::despachar` itera esa lista y llama a `MonitorClientes`, que administra las colas de salida de cada cliente conectado.

`MonitorClientes` pertenece a la integración con el servidor/red. El gameloop solo depende de su interfaz para despachar mensajes.

---

## Estado del mundo: Juego

[`server/game/juego.h`](../server/game/juego.h)

`Juego` es la clase que centraliza el estado del servidor. Contiene:

| Miembro                  | Qué es                                                                                                   |
| ------------------------ | -------------------------------------------------------------------------------------------------------- |
| `jugadoresConectados`    | `map<id, Jugador>` — jugadores con sesión activa                                                         |
| `jugadoresDesconectados` | `map<id, Jugador>` — jugadores que se desconectaron pero cuyo personaje queda disponible para reconexión |
| `criaturasEnMapa`        | `map<id, Criatura>` — NPCs enemigos vivos                                                                |
| `clanes`                 | `map<id, Clan>` — organizaciones de jugadores                                                            |
| `cfg`                    | `ConfigJuego` — parámetros del juego                                                                     |
| `catalogo`               | `CatalogoItems` — tipos de items disponibles                                                             |

### Flujo de un comando

```
Gameloop::procesarComando(ComandoCliente)
└─ Juego::ejecutarComando(idCliente, ComandoJugador)
└─ switch(opcode) → ejecutarMeditar / ejecutarAtacar / ejecutarFundarClan / ...
└─ retorna list<MensajeSalida>
└─ Gameloop::despachar(mensajes)
```

El `switch` sobre `Opcode` en `ejecutarComando` es el punto de entrada de la lógica de juego. Cada comando delega a un método privado de `Juego`.

### Cancelación de meditación

Si un jugador está meditando y ejecuta una acción distinta de `/meditar`, el juego cancela la meditación antes de procesar el comando. Esto respeta la regla del enunciado: un jugador meditando no puede realizar otra acción, y cualquier acción lo saca de ese estado.

### Tick del mundo (`actualizar`)

`Juego::actualizar(segundos)` se llama en cada tick del gameloop y resuelve toda
la evolución del mundo por tick:

- Regenera vida y maná (`jugador.recuperar`), aplica la meditación y emite
  `ESTADO_PERSONAJE` ante cualquier cambio.
- **Movimiento server-driven** de los jugadores: avanza una celda por tick a
  quienes estén moviéndose (entre `EMPEZAR_MOVER` y `DETENER_MOVER`) y hace el
  auto-pickup de oro/items al pisar la celda.
- Resuelve las **transiciones de estado**: muerte, fin de inmovilización tras
  morir (resurrección con tiempo proporcional a la distancia al sacerdote) y fin
  de meditación.
- Mueve y actualiza a las **criaturas** según su cadencia configurada.

---

## Jugador

[`server/game/jugador.h`](../server/game/jugador.h)

Representa el personaje de un cliente conectado. Los stats base (fuerza, agilidad, inteligencia, constitución) se calculan a partir de la raza usando `ConfigJuego::statsRaza`. `vidaMax` y `manaMax` se derivan de esos stats combinados con los factores de clase.

### Estado del jugador

El campo `estado` es un `enum class Estado { Vivo, Fantasma, Meditando, Resucitando }`. Centraliza toda la lógica de estado sin flags booleanos independientes que puedan desincronizarse.

- `estaVivo()` → `Vivo || Meditando`
- `esFantasma()` → `Fantasma || Resucitando`

### Progresión

`ganar_experiencia(cantidad)` acumula experiencia y llama a `subirNivel()` cuando se supera el límite definido por la fórmula de experiencia. Al subir de nivel se recalculan `vidaMax` y `manaMax`, y el personaje se cura completamente.

### Oro

El oro se divide en dos campos:

- `oroMano`: oro seguro en mano.
- `oroExceso`: oro por encima del límite seguro, que puede caer al suelo al morir.

`getOro()` devuelve la suma de ambos. `sumar_oro` llena primero `oroMano`; si se supera el tope seguro, el resto va a `oroExceso`, respetando el máximo adicional permitido por configuración.

### Banco interno

No existe una clase `Banco` separada. El banco es global por definición del juego, ya que se accede desde cualquier sucursal. Por eso sus datos viven directamente en `Jugador`: `oroBanco` e `idItemsBanco`.

Las operaciones `agregar_oro_banco` y `sacar_oro_banco` mueven oro entre la mano del jugador y el banco.

### Meditación y recuperación

`meditar()` cambia el estado a `Meditando`.

En cada tick, `recuperar(segundos)` aplica:

- recuperación natural de vida;
- recuperación natural de maná;
- recuperación adicional de maná si el personaje está meditando.

La meditación utiliza el factor de meditación de la clase y la inteligencia del personaje. El guerrero no puede meditar porque no utiliza magia y su maná máximo es cero.

Al llegar a `manaMax`, el estado vuelve a `Vivo` automáticamente.

---

## Inventario y Equipamiento

[`server/game/objeto/inventario.h`](../server/game/objeto/inventario.h)

`Inventario` gestiona los slots de ítem y delega el equipamiento a `Equipamiento`.

La separación existe porque tienen responsabilidades distintas:

- **`Inventario`**: almacenamiento, agregar, sacar y consultar ítems.
- **`Equipamiento`**: qué ítem está activo en cada slot de combate (arma, báculo, armadura, casco, escudo) e impone la invariante de que arma y báculo son mutuamente excluyentes.

### Equipar un ítem

El comando `EQUIPAR` envía un **índice de slot** (posición en el inventario visible para el cliente), no un ID.

`Jugador::equipar_item(indice, catalogo)` resuelve el ID con `inventario.getIdEnSlot(indice)`, consulta el catálogo para saber el tipo y aplica una de estas reglas:

- armas y báculos se equipan en `Equipamiento`;
- armaduras, cascos y escudos se equipan en su slot defensivo correspondiente;
- pociones se consumen inmediatamente y aplican su efecto sobre vida o maná.

Esta lógica respeta la regla del enunciado: las pociones no quedan equipadas como un objeto persistente, sino que se usan y desaparecen del inventario.

---

## Clan

[`server/game/clan.h`](../server/game/clan.h)

Organización de jugadores con tres estados por miembro: `Pendiente`, `Aceptado`, `Baneado`.

Se usa un único `vector<MiembroClan>` con el estado como campo en lugar de tres listas separadas. Así, la transición entre estados es un cambio de valor en lugar de una remoción/inserción coordinada entre colecciones.

Comandos implementados:

- fundar clan;
- pedir unirse;
- aceptar miembro;
- rechazar solicitud;
- banear miembro o solicitud pendiente;
- expulsar miembro;
- dejar clan;
- revisar miembros y solicitudes pendientes.

Solo el fundador puede gestionar miembros. Si el fundador se desconecta, las solicitudes quedan en espera hasta que vuelva.

Reglas relevantes:

- el fundador no puede dejar el clan;
- el fundador no puede ser expulsado ni baneado;
- el ban impide futuros pedidos de ingreso;
- el kick expulsa pero no banea;
- el límite de miembros incluye al fundador.

Pendiente para integración posterior:

- mensajes de entrada y salida de miembros conectados;
- notificaciones pendientes para jugadores desconectados;
- bonus de clan por cercanía;
- bloqueo de ataque entre miembros del mismo clan.

---

## Protocolo

El protocolo define qué mensajes se intercambian entre cliente y servidor.

- **`ComandoJugador`** (`common/protocolo/comando_jugador.h`): lo que envía el cliente. Tiene un `Opcode` y un `std::variant` con el payload específico del comando. Los comandos sin payload (`MEDITAR`, `RESUCITAR`, `TOMAR`, `REVISAR_CLAN`, `DEJAR_CLAN`) usan structs vacíos en el variant.

- **`MensajeServidor`** (`common/protocolo/mensaje_servidor.h`): lo que envía el servidor. Sigue el mismo patrón: `Opcode` + `std::variant` con el payload.

- **`MensajeSalida`** (`server/gameloop/mensaje_salida.h`): wrapper interno que agrega el destino (`TipoDestino::UNO` con `idCliente` o `TipoDestino::TODOS`) antes de pasar por `MonitorClientes`.

---

## Estado de implementación

| Componente                                                 | Estado              |
| ---------------------------------------------------------- | ------------------- |
| Config TOML con `toml++` / `ConfigJuego` / `CatalogoItems` | ✅ Implementado     |
| Gameloop (hilo, cola, tick, despacho)                      | ✅ Implementado     |
| Persistencia binaria de jugadores                          | ✅ Implementado     |
| Mapa (zonas, ciudades, paredes, NPCs)                      | ✅ Implementado     |
| Movimiento server-driven                                   | ✅ Implementado     |
| Combate cuerpo a cuerpo y a distancia                      | ✅ Implementado     |
| Hechizos (comprar / lanzar / FX)                           | ✅ Implementado     |
| Meditar · morir / resucitar (aura+barra+sonido)            | ✅ Implementado     |
| Inventario / Equipamiento / Usar (pociones)                | ✅ Implementado     |
| NPCs: comerciante, banquero, sacerdote                     | ✅ Implementado     |
| Banco (depositar / retirar oro e items)                    | ✅ Implementado     |
| Chat global / privado                                      | ✅ Implementado     |
| Criaturas (movimiento por tick)                            | ✅ Implementado     |
| Mazmorra                                                   | ❌ Fuera de alcance |
