# Protocolo Binario — Servidor → Cliente

## Formato general

Todos los mensajes comienzan con un opcode de 1 byte (uint8). Los opcodes
del servidor arrancan en 26 para evitar colisiones con los opcodes del
cliente que van del 1 al 25. Ambos conjuntos de opcodes viven en un único
enum en common/protocol/Opcode.h compartido entre cliente y servidor.

Mensajes de tamaño variable: MENSAJE_CHAT y LISTA_ITEMS incluyen un campo
length o cantidad que indica cuántos bytes leer a continuación.

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

## POSICION_ENTIDAD (opcode 27)

Enviado cuando una entidad aparece o se mueve en el área visible del jugador.

| Campo  | Tipo   | Descripción                        |
| ------ | ------ | ---------------------------------- |
| opcode | uint8  | valor: 27                          |
| id     | uint16 | id de la entidad                   |
| x      | uint16 | posición x                         |
| y      | uint16 | posición y                         |
| tipo   | uint8  | 0=personaje 1=criatura 2=NPCCiudad |
| estado | uint8  | 0=vivo 1=fantasma 2=meditando      |

## ENTIDAD_DESAPARECIO (opcode 28)

Enviado cuando una entidad sale del área visible o muere.

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 28        |
| id     | uint16 | id de la entidad |

## DAÑO_RECIBIDO (opcode 29)

| Campo      | Tipo   | Descripción     |
| ---------- | ------ | --------------- |
| opcode     | uint8  | valor: 29       |
| cantidad   | uint16 | puntos de daño  |
| idAtacante | uint16 | id del atacante |

## DAÑO_PRODUCIDO (opcode 30)

| Campo      | Tipo   | Descripción     |
| ---------- | ------ | --------------- |
| opcode     | uint8  | valor: 30       |
| cantidad   | uint16 | puntos de daño  |
| idObjetivo | uint16 | id del objetivo |

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

## ITEM_DESAPARECIO_SUELO (opcode 31)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 31   |
| x      | uint16 | posición x  |
| y      | uint16 | posición y  |

## ACTUALIZAR_INVENTARIO (opcode 32)

Enviado cuando el inventario cambia. Manda los slots completos.
N viene del TOML — es el mismo valor que el servidor usa para el inventario.

| Campo  | Tipo      | Descripción                     |
| ------ | --------- | ------------------------------- |
| opcode | uint8     | valor: 32                       |
| size   | uint8     | cantidad de items en inventario |
| slots  | uint16[N] | ids de items (0 = slot vacío)   |

## ACTUALIZAR_EQUIPAMIENTO (opcode 33)

| Campo   | Tipo   | Descripción                   |
| ------- | ------ | ----------------------------- |
| opcode  | uint8  | valor: 33                     |
| arma    | uint16 | id del arma (0 = vacío)       |
| baculo  | uint16 | id del báculo (0 = vacío)     |
| defensa | uint16 | id de la armadura (0 = vacío) |
| casco   | uint16 | id del casco (0 = vacío)      |
| escudo  | uint16 | id del escudo (0 = vacío)     |

## MENSAJE_CHAT (opcode 34)

| Campo      | Tipo         | Descripción                 |
| ---------- | ------------ | --------------------------- |
| opcode     | uint8        | valor: 34                   |
| nickOrigen | char[32]     | nick del emisor             |
| length     | uint16       | cantidad de bytes del texto |
| texto      | char[length] | contenido del mensaje       |

## MENSAJE_CLAN (opcode 35)

| Campo  | Tipo     | Descripción                                             |
| ------ | -------- | ------------------------------------------------------- |
| opcode | uint8    | valor: 35                                               |
| tipo   | uint8    | 0=entró 1=salió 2=siendo atacado 3=aceptado 4=rechazado |
| nick   | char[32] | nick del miembro involucrado                            |

## RESUCITADO (opcode 36)

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 36        |
| x      | uint16 | nueva posición x |
| y      | uint16 | nueva posición y |

## LISTA_ITEMS (opcode 37)

Respuesta al comando LISTAR. Tamaño variable según cantidad de items.

| Campo    | Tipo             | Descripción                   |
| -------- | ---------------- | ----------------------------- |
| opcode   | uint8            | valor: 37                     |
| cantidad | uint8            | cantidad de items en la lista |
| ids      | uint16[cantidad] | ids de los items disponibles  |

## ERROR_ACCION (opcode 38)

| Campo       | Tipo  | Descripción           |
| ----------- | ----- | --------------------- |
| opcode      | uint8 | valor: 38             |
| codigoError | uint8 | 0=inventario lleno    |
|             |       | 1=oro insuficiente    |
|             |       | 2=nivel insuficiente  |
|             |       | 3=accion no permitida |
|             |       | 4=objetivo invalido   |
|             |       | 5=clan lleno          |
