# Protocolo Binario — Servidor → Cliente

## Formato general

Todos los mensajes comienzan con un opcode de 1 byte (`uint8`) que identifica
el tipo de mensaje.

Los opcodes del servidor comienzan en 26 para evitar colisiones con los opcodes
del cliente. Ambos conjuntos de opcodes viven en un único enum compartido en
[`common/protocolo/opcode.h`](../common/protocolo/opcode.h). Hay además opcodes
de cliente posteriores al 25 (`CHEAT`=45, `DETENER_MOVER`=46, `USAR`=47,
`COMPRAR_HECHIZO`=49, `LANZAR_HECHIZO`=51) intercalados con los del servidor.

Luego del opcode, cada mensaje contiene los campos definidos para ese tipo.
Los campos numéricos tienen tamaño fijo y se envían en orden de red
(network byte order):

| Tipo lógico | Tamaño  | Codificación      |
| ----------- | ------- | ----------------- |
| uint8       | 1 byte  | sin conversión    |
| uint16      | 2 bytes | `htons` / `ntohs` |
| uint32      | 4 bytes | `htonl` / `ntohl` |

Los campos de texto se envían siempre con longitud explícita:

| Campo  | Tipo         | Descripción                 |
| ------ | ------------ | --------------------------- |
| length | uint16       | cantidad de bytes del texto |
| texto  | char[length] | contenido del texto         |

El receptor lee primero el opcode. Según el opcode, sabe qué campos debe leer.
Cuando un campo es texto, lee primero su `length` y luego exactamente esa
cantidad de bytes.

## ESTADO_PERSONAJE (opcode 26)

Enviado periódicamente o ante cualquier cambio en el estado del jugador.

| Campo       | Tipo   | Descripción             |
| ----------- | ------ | ----------------------- |
| opcode      | uint8  | valor: 26               |
| vidaActual  | uint16 | puntos de vida actuales |
| vidaMax     | uint16 | puntos de vida máximos  |
| manaActual  | uint16 | puntos de maná actuales |
| manaMax     | uint16 | puntos de maná máximos  |
| oro         | uint32 | oro en mano             |
| nivel       | uint8  | nivel actual            |
| experiencia | uint32 | experiencia acumulada   |
| estado      | uint8  | 0=vivo 1=fantasma 2=meditando 3=resucitando |
| raza        | uint8  | raza del personaje      |
| clase       | uint8  | clase del personaje     |
| expSiguienteNivel | uint32 | experiencia para subir de nivel |
| tiempoResurreccionMs | uint16 | ms inmóvil restantes al resucitar (0 si no resucita) |

## POSICION_ENTIDAD (opcode 27)

Enviado cuando una entidad aparece o se mueve en el área visible del jugador.

| Campo  | Tipo   | Descripción                        |
| ------ | ------ | ---------------------------------- |
| opcode | uint8  | valor: 27                          |
| id     | uint16 | id de la entidad                   |
| x      | uint16 | posición x                         |
| y      | uint16 | posición y                         |
| tipo   | uint8  | 0=personaje 1=criatura 2=NPCCiudad |
| estado | uint8  | 0=vivo 1=fantasma 2=meditando 3=resucitando |
| cabeza | uint16 | sprite de cabeza                   |
| cuerpo | uint16 | sprite de cuerpo                   |
| arma   | uint16 | sprite del arma/báculo equipado (0 = ninguno) |
| escudo | uint16 | sprite del escudo equipado (0 = ninguno) |
| casco  | uint16 | sprite del casco equipado (0 = ninguno) |

## ENTIDAD_DESAPARECIO (opcode 28)

Enviado cuando una entidad sale del área visible o muere.

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 28        |
| id     | uint16 | id de la entidad |

## DAÑO_RECIBIDO (opcode 29)

| Campo      | Tipo   | Descripción            |
| ---------- | ------ | ---------------------- |
| opcode     | uint8  | valor: 29              |
| cantidad   | uint16 | puntos de daño         |
| idAtacante | uint16 | id del atacante        |
| esCritico  | uint8  | 1 = golpe crítico, 0 = normal |

## DAÑO_PRODUCIDO (opcode 30)

| Campo      | Tipo   | Descripción                                    |
| ---------- | ------ | ---------------------------------------------- |
| opcode     | uint8  | valor: 30                                      |
| cantidad   | uint16 | puntos de daño                                 |
| idObjetivo | uint16 | id del objetivo                                |
| tipoGolpe  | uint8  | tipo de golpe/arma (espada, hacha, martillo, disparo, hechizo, explosión) — define el sonido |
| esCritico  | uint8  | 1 = golpe crítico, 0 = normal                  |

## ESQUIVE (opcode 31)

| Campo      | Tipo   | Descripción                    |
| ---------- | ------ | ------------------------------ |
| opcode     | uint8  | valor: 31                      |
| idEntidad  | uint16 | id de quien esquivó            |
| esquivador | uint8  | 0=yo esquivé 1=el otro esquivó |

## MUERTE_ENTIDAD (opcode 32)

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 32        |
| id     | uint16 | id de la entidad |

## ITEM_EN_SUELO (opcode 33)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 33   |
| idItem | uint16 | id del item |
| x      | uint16 | posición x  |
| y      | uint16 | posición y  |

## ITEM_DESAPARECIO_SUELO (opcode 34)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 34   |
| x      | uint16 | posición x  |
| y      | uint16 | posición y  |

## ACTUALIZAR_INVENTARIO (opcode 35)

Enviado cuando el inventario cambia. Manda los slots completos.
N viene del TOML — es el mismo valor que el servidor usa para el inventario.

| Campo  | Tipo      | Descripción                     |
| ------ | --------- | ------------------------------- |
| opcode | uint8     | valor: 35                       |
| size   | uint8     | cantidad de items en inventario |
| slots  | uint16[N] | ids de items (0 = slot vacío)   |

## ACTUALIZAR_EQUIPAMIENTO (opcode 36)

| Campo   | Tipo   | Descripción                   |
| ------- | ------ | ----------------------------- |
| opcode  | uint8  | valor: 36                     |
| arma    | uint16 | id del arma (0 = vacío)       |
| baculo  | uint16 | id del báculo (0 = vacío)     |
| defensa | uint16 | id de la armadura (0 = vacío) |
| casco   | uint16 | id del casco (0 = vacío)      |
| escudo  | uint16 | id del escudo (0 = vacío)     |

### MENSAJE_CHAT (opcode 37)

Mensajes de chat y avisos del juego. El campo `tipo` (categoría) indica al cliente
con qué color/formato mostrar la línea; cuando no hay emisor (avisos del sistema),
`nickOrigen` viaja vacío y la línea se muestra sin el prefijo `nick:`.

| Campo         | Tipo                | Descripción                              |
| ------------- | ------------------- | ---------------------------------------- |
| opcode        | uint8               | valor: 37                                |
| nickLength    | uint16              | cantidad de bytes del nick origen        |
| nickOrigen    | char[nickLength]    | nick del emisor (vacío si es del sistema)|
| mensajeLength | uint16              | cantidad de bytes del mensaje            |
| mensaje       | char[mensajeLength] | contenido del mensaje                    |
| tipo          | uint8               | categoría: 0=normal/global 1=privado 2=sistema |

Se usa para: chat global, chat privado (eco al emisor incluido), y avisos del
sistema (muerte, compra, subir de nivel, entrada/salida de mazmorra,
conexión/desconexión de jugadores).

## RESUCITADO (opcode 39)

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 39        |
| x      | uint16 | nueva posición x |
| y      | uint16 | nueva posición y |

## LISTA_ITEMS (opcode 40)

Respuesta al comando LISTAR. Tamaño variable según cantidad de items.

| Campo    | Tipo             | Descripción                   |
| -------- | ---------------- | ----------------------------- |
| opcode   | uint8            | valor: 40                     |
| cantidad | uint8            | cantidad de items en la lista |
| ids      | uint16[cantidad] | ids de los items disponibles  |

## ERROR_ACCION (opcode 41)

| Campo       | Tipo  | Descripción |
| ----------- | ----- | ----------- |
| opcode      | uint8 | valor: 41   |
| codigoError | uint8 | ver tabla   |

Códigos (`CodigoErrorAccion`):

> Los valores son explícitos en `CodigoErrorAccion`; algunos números quedan sin
> usar (huecos) por features descartadas. Abajo se listan solo los vigentes.

| Código | Significado | Código | Significado |
| ------ | ----------- | ------ | ----------- |
| 0 | inventario lleno | 13 | usuario ya conectado |
| 1 | oro insuficiente | 14 | sin posición libre |
| 2 | nivel insuficiente | 15 | id de personaje en uso |
| 3 | acción no permitida | 16 | no se pudo cargar el personaje |
| 4 | objetivo inválido | 17 | estás muerto |
| 6 | maná insuficiente | 23 | no podés resucitar |
| 7 | fuera de rango | 24 | estás inmovilizado |
| 8 | zona segura | 25 | ataque a newbie |
| 9 | clase sin magia (guerrero) | 26 | diferencia de nivel excesiva |
| 10 | hechizo ya conocido | 28 | hechizo no ofensivo |
| 11 | cooldown de ataque | 29 | hechizo no conocido |
| 12 | objetivo muerto | | |

## ORO_EN_SUELO (opcode 42)

| Campo    | Tipo   | Descripción      |
| -------- | ------ | ---------------- |
| opcode   | uint8  | valor: 42        |
| cantidad | uint32 | cantidad de oro  |
| x        | uint16 | posición x       |
| y        | uint16 | posición y       |

## ORO_DESAPARECIO_SUELO (opcode 43)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 43   |
| x      | uint16 | posición x  |
| y      | uint16 | posición y  |

## ESTADO_USUARIO (opcode 44)

Resultado del handshake de login (alta/reconexión del personaje).

| Campo  | Tipo         | Descripción                                                     |
| ------ | ------------ | -------------------------------------------------------------- |
| opcode | uint8        | valor: 44                                                      |
| id     | uint16       | id de cliente asignado                                         |
| nick   | length+texto | nick del personaje                                            |
| error  | uint8        | 0=nick no encontrado 1=nick ya existe 2=usuario ya conectado 3=ninguno (ok) |

## CONTENIDO_BANCO (opcode 48)

Contenido del banco del jugador (items guardados + oro).

| Campo    | Tipo             | Descripción                |
| -------- | ---------------- | -------------------------- |
| opcode   | uint8            | valor: 48                  |
| cantidad | uint8            | cantidad de items          |
| ids      | uint16[cantidad] | ids de los items en el banco |
| oroBanco | uint32           | oro guardado en el banco   |

## LISTA_HECHIZOS (opcode 50)

Hechizos que el jugador conoce (para la pestaña de hechizos / lanzar).

| Campo    | Tipo             | Descripción                  |
| -------- | ---------------- | ---------------------------- |
| opcode   | uint8            | valor: 50                    |
| cantidad | uint8            | cantidad de hechizos         |
| ids      | uint16[cantidad] | ids de hechizos conocidos    |

## FX_HECHIZO (opcode 52)

FX visual de un hechizo lanzado, para todos los que lo ven.

| Campo      | Tipo   | Descripción         |
| ---------- | ------ | ------------------- |
| opcode     | uint8  | valor: 52           |
| idHechizo  | uint16 | id del hechizo      |
| idObjetivo | uint16 | id del objetivo     |

## PROYECTIL (opcode 53)

Proyectil que viaja del atacante al objetivo (ataque a distancia).

| Campo     | Tipo   | Descripción          |
| --------- | ------ | -------------------- |
| opcode    | uint8  | valor: 53            |
| idOrigen  | uint16 | id de quien dispara  |
| idDestino | uint16 | id del objetivo      |
