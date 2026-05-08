# Protocolo Binario — Cliente → Servidor

## Formato general

Todos los mensajes comienzan con un opcode de 1 byte (uint8) que identifica
el tipo de mensaje. El receptor lee el opcode primero y según su valor sabe
exactamente cuántos bytes leer a continuación.

Excepción: los mensajes de chat tienen tamaño variable. Luego del opcode
incluyen un campo length (uint16) que indica cuántos bytes de texto leer.

Los opcodes viven en un enum en common/ compartido entre cliente y servidor
para garantizar consistencia.

## Decisión de diseño

Se eligió tamaño fijo por mensaje en todos los mensajes porque la mayoría de los mensajes contienen solo datos
numéricos de tamaño conocido. Agregar length a todos por el caso del chat
sería overhead innecesario. El chat es el único caso con texto de longitud
variable y se maneja de forma explícita.

Los campos de texto fijo como nicknames y nombres de clan usan char[32].
Los mensajes de chat usan length variable porque limitar el texto de chat
a un tamaño fijo arbitrario degradaría la experiencia.

## Mensajes

### MOVER (opcode 1)

| Campo     | Tipo  | Descripción                            |
| --------- | ----- | -------------------------------------- |
| opcode    | uint8 | valor: 1                               |
| direccion | uint8 | 0=arriba 1=abajo 2=izquierda 3=derecha |

### ATACAR (opcode 2)

| Campo      | Tipo   | Descripción          |
| ---------- | ------ | -------------------- |
| opcode     | uint8  | valor: 2             |
| idObjetivo | uint16 | id del NPC o jugador |

### MEDITAR (opcode 3)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 3    |

### RESUCITAR (opcode 4)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 4    |

### CURAR (opcode 5)

| Campo       | Tipo   | Descripción          |
| ----------- | ------ | -------------------- |
| opcode      | uint8  | valor: 5             |
| idSacerdote | uint16 | id del sacerdote NPC |

### TOMAR (opcode 6)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 6    |

### TIRAR (opcode 7)

| Campo      | Tipo  | Descripción                   |
| ---------- | ----- | ----------------------------- |
| opcode     | uint8 | valor: 7                      |
| indiceItem | uint8 | índice del item en inventario |

### EQUIPAR (opcode 8)

| Campo      | Tipo  | Descripción                   |
| ---------- | ----- | ----------------------------- |
| opcode     | uint8 | valor: 8                      |
| indiceItem | uint8 | índice del item en inventario |

### COMPRAR (opcode 9)

| Campo  | Tipo   | Descripción                    |
| ------ | ------ | ------------------------------ |
| opcode | uint8  | valor: 9                       |
| idItem | uint16 | id del item a comprar          |
| idNPC  | uint16 | id del comerciante o sacerdote |

### VENDER (opcode 10)

| Campo      | Tipo   | Descripción                   |
| ---------- | ------ | ----------------------------- |
| opcode     | uint8  | valor: 10                     |
| indiceItem | uint8  | índice del item en inventario |
| idNPC      | uint16 | id del comerciante            |

### DEPOSITAR_ITEM (opcode 11)

| Campo      | Tipo   | Descripción                   |
| ---------- | ------ | ----------------------------- |
| opcode     | uint8  | valor: 11                     |
| indiceItem | uint8  | índice del item en inventario |
| idBanquero | uint16 | id del banquero               |

### DEPOSITAR_ORO (opcode 12)

| Campo      | Tipo   | Descripción     |
| ---------- | ------ | --------------- |
| opcode     | uint8  | valor: 12       |
| monto      | uint32 | cantidad de oro |
| idBanquero | uint16 | id del banquero |

### RETIRAR_ITEM (opcode 13)

| Campo      | Tipo   | Descripción           |
| ---------- | ------ | --------------------- |
| opcode     | uint8  | valor: 13             |
| idItem     | uint16 | id del item a retirar |
| idBanquero | uint16 | id del banquero       |

### RETIRAR_ORO (opcode 14)

| Campo      | Tipo   | Descripción     |
| ---------- | ------ | --------------- |
| opcode     | uint8  | valor: 14       |
| monto      | uint32 | cantidad de oro |
| idBanquero | uint16 | id del banquero |

### LISTAR (opcode 15)

| Campo  | Tipo   | Descripción |
| ------ | ------ | ----------- |
| opcode | uint8  | valor: 15   |
| idNPC  | uint16 | id del NPC  |

### CHAT_GLOBAL (opcode 16)

| Campo  | Tipo         | Descripción                 |
| ------ | ------------ | --------------------------- |
| opcode | uint8        | valor: 16                   |
| length | uint16       | cantidad de bytes del texto |
| texto  | char[length] | contenido del mensaje       |

### CHAT_PRIVADO (opcode 17)

| Campo       | Tipo         | Descripción                 |
| ----------- | ------------ | --------------------------- |
| opcode      | uint8        | valor: 17                   |
| nickDestino | char[32]     | nick del destinatario       |
| length      | uint16       | cantidad de bytes del texto |
| texto       | char[length] | contenido del mensaje       |

### FUNDAR_CLAN (opcode 18)

| Campo      | Tipo     | Descripción     |
| ---------- | -------- | --------------- |
| opcode     | uint8    | valor: 18       |
| nombreClan | char[32] | nombre del clan |

### UNIRSE_CLAN (opcode 19)

| Campo      | Tipo     | Descripción     |
| ---------- | -------- | --------------- |
| opcode     | uint8    | valor: 19       |
| nombreClan | char[32] | nombre del clan |

### REVISAR_CLAN (opcode 20)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 20   |

### CLAN_ACEPTAR (opcode 21)

| Campo  | Tipo     | Descripción      |
| ------ | -------- | ---------------- |
| opcode | uint8    | valor: 21        |
| nick   | char[32] | nick del jugador |

### CLAN_RECHAZAR (opcode 22)

| Campo  | Tipo     | Descripción      |
| ------ | -------- | ---------------- |
| opcode | uint8    | valor: 22        |
| nick   | char[32] | nick del jugador |

### CLAN_BAN (opcode 23)

| Campo  | Tipo     | Descripción      |
| ------ | -------- | ---------------- |
| opcode | uint8    | valor: 23        |
| nick   | char[32] | nick del jugador |

### CLAN_KICK (opcode 24)

| Campo  | Tipo     | Descripción      |
| ------ | -------- | ---------------- |
| opcode | uint8    | valor: 24        |
| nick   | char[32] | nick del jugador |

### DEJAR_CLAN (opcode 25)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 25   |
