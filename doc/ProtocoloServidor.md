# Protocolo Binario — Servidor → Cliente

## Formato general

Todos los mensajes comienzan con un opcode de 1 byte (uint8). Los opcodes
del servidor arrancan en 50 para evitar colisiones con los opcodes del
cliente que van del 1 al 49. Ambos conjuntos de opcodes viven en un único
enum en common/protocol/Opcode.h compartido entre cliente y servidor.

Mensajes de tamaño variable: MENSAJE_CHAT y LISTA_ITEMS incluyen un campo
length o cantidad que indica cuántos bytes leer a continuación.

## ESTADO_PERSONAJE (opcode 50)

Enviado periódicamente o ante cualquier cambio en el estado del jugador.

| Campo       | Tipo   | Descripción             |
| ----------- | ------ | ----------------------- |
| opcode      | uint8  | valor: 50               |
| vidaActual  | uint16 | puntos de vida actuales |
| vidaMax     | uint16 | puntos de vida máximos  |
| manaActual  | uint16 | puntos de maná actuales |
| manaMax     | uint16 | puntos de maná máximos  |
| oro         | uint32 | oro en mano             |
| nivel       | uint8  | nivel actual            |
| experiencia | uint32 | experiencia acumulada   |

## POSICION_ENTIDAD (opcode 51)

Enviado cuando una entidad aparece o se mueve en el área visible del jugador.

| Campo  | Tipo   | Descripción                        |
| ------ | ------ | ---------------------------------- |
| opcode | uint8  | valor: 51                          |
| id     | uint16 | id de la entidad                   |
| x      | uint16 | posición x                         |
| y      | uint16 | posición y                         |
| tipo   | uint8  | 0=personaje 1=criatura 2=NPCCiudad |
| estado | uint8  | 0=vivo 1=fantasma 2=meditando      |

## ENTIDAD_DESAPARECIO (opcode 52)

Enviado cuando una entidad sale del área visible o muere.

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 52        |
| id     | uint16 | id de la entidad |

## DAÑO_RECIBIDO (opcode 53)

| Campo      | Tipo   | Descripción     |
| ---------- | ------ | --------------- |
| opcode     | uint8  | valor: 53       |
| cantidad   | uint16 | puntos de daño  |
| idAtacante | uint16 | id del atacante |

## DAÑO_PRODUCIDO (opcode 54)

| Campo      | Tipo   | Descripción     |
| ---------- | ------ | --------------- |
| opcode     | uint8  | valor: 54       |
| cantidad   | uint16 | puntos de daño  |
| idObjetivo | uint16 | id del objetivo |

## ESQUIVE (opcode 55)

| Campo      | Tipo   | Descripción                    |
| ---------- | ------ | ------------------------------ |
| opcode     | uint8  | valor: 55                      |
| idEntidad  | uint16 | id de quien esquivó            |
| esquivador | uint8  | 0=yo esquivé 1=el otro esquivó |

## MUERTE_ENTIDAD (opcode 56)

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 56        |
| id     | uint16 | id de la entidad |

## ITEM_EN_SUELO (opcode 57)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 57   |
| idItem | uint16 | id del item |
| x      | uint16 | posición x  |
| y      | uint16 | posición y  |

## ITEM_DESAPARECIO_SUELO (opcode 58)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 58   |
| x      | uint16 | posición x  |
| y      | uint16 | posición y  |

## ACTUALIZAR_INVENTARIO (opcode 59)

Enviado cuando el inventario cambia. Manda los slots completos.
N viene del TOML — es el mismo valor que el servidor usa para el inventario.

| Campo  | Tipo      | Descripción                     |
| ------ | --------- | ------------------------------- |
| opcode | uint8     | valor: 59                       |
| size   | uint8     | cantidad de items en inventario |
| slots  | uint16[N] | ids de items (0 = slot vacío)   |

## ACTUALIZAR_EQUIPAMIENTO (opcode 60)

| Campo   | Tipo   | Descripción                   |
| ------- | ------ | ----------------------------- |
| opcode  | uint8  | valor: 60                     |
| arma    | uint16 | id del arma (0 = vacío)       |
| baculo  | uint16 | id del báculo (0 = vacío)     |
| defensa | uint16 | id de la armadura (0 = vacío) |
| casco   | uint16 | id del casco (0 = vacío)      |
| escudo  | uint16 | id del escudo (0 = vacío)     |

## MENSAJE_CHAT (opcode 61)

| Campo      | Tipo         | Descripción                 |
| ---------- | ------------ | --------------------------- |
| opcode     | uint8        | valor: 61                   |
| nickOrigen | char[32]     | nick del emisor             |
| length     | uint16       | cantidad de bytes del texto |
| texto      | char[length] | contenido del mensaje       |

## MENSAJE_CLAN (opcode 62)

| Campo  | Tipo     | Descripción                                             |
| ------ | -------- | ------------------------------------------------------- |
| opcode | uint8    | valor: 62                                               |
| tipo   | uint8    | 0=entró 1=salió 2=siendo atacado 3=aceptado 4=rechazado |
| nick   | char[32] | nick del miembro involucrado                            |

## RESUCITADO (opcode 63)

| Campo  | Tipo   | Descripción      |
| ------ | ------ | ---------------- |
| opcode | uint8  | valor: 63        |
| x      | uint16 | nueva posición x |
| y      | uint16 | nueva posición y |

## LISTA_ITEMS (opcode 64)

Respuesta al comando LISTAR. Tamaño variable según cantidad de items.

| Campo    | Tipo             | Descripción                   |
| -------- | ---------------- | ----------------------------- |
| opcode   | uint8            | valor: 64                     |
| cantidad | uint8            | cantidad de items en la lista |
| ids      | uint16[cantidad] | ids de los items disponibles  |

## ERROR_ACCION (opcode 65)

| Campo       | Tipo  | Descripción           |
| ----------- | ----- | --------------------- |
| opcode      | uint8 | valor: 65             |
| codigoError | uint8 | 0=inventario lleno    |
|             |       | 1=oro insuficiente    |
|             |       | 2=nivel insuficiente  |
|             |       | 3=accion no permitida |
|             |       | 4=objetivo invalido   |
|             |       | 5=clan lleno          |
