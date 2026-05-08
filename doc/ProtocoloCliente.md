# Protocolo Binario — Cliente → Servidor

## Formato general

Todos los mensajes comienzan con un opcode de 1 byte (uint8) que identifica
el tipo de mensaje.

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

## Decisión de diseño

Se eligió usar tamaño fijo para los campos numéricos y longitud explícita para
todos los campos de texto.
Con el formato `length + texto`, todas las cadenas se serializan de la misma
manera. Esto simplifica la implementación del protocolo y permite trabajar con
`std::string` en las capas superiores.

Aunque el formato sea variable, el servidor debe validar límites máximos según
el tipo de dato:

| Campo           | Máximo recomendado |
| --------------- | ------------------ |
| nickname        | 32 bytes           |
| nombre de clan  | 32 bytes           |
| mensaje de chat | 256 bytes          |

Estos límites son nuestras reglas del protocolo y deben validarse al recibir datos.

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

| Campo         | Tipo                | Descripción                   |
| ------------- | ------------------- | ----------------------------- |
| opcode        | uint8               | valor: 16                     |
| mensajeLength | uint16              | cantidad de bytes del mensaje |
| mensaje       | char[mensajeLength] | contenido del mensaje         |

### CHAT_PRIVADO (opcode 17)

| Campo         | Tipo                | Descripción                        |
| ------------- | ------------------- | ---------------------------------- |
| opcode        | uint8               | valor: 17                          |
| nickLength    | uint16              | cantidad de bytes del nick destino |
| nickDestino   | char[nickLength]    | nick del destinatario              |
| mensajeLength | uint16              | cantidad de bytes del mensaje      |
| mensaje       | char[mensajeLength] | contenido del mensaje              |

### FUNDAR_CLAN (opcode 18)

| Campo      | Tipo                   | Descripción                           |
| ---------- | ---------------------- | ------------------------------------- |
| opcode     | uint8                  | valor: 18                             |
| nombreClan | uint16                 | cantidad de bytes del nombre del clan |
| nombreClan | char[nombreClanLength] | nombre del clan                       |

### UNIRSE_CLAN (opcode 19)

| Campo            | Tipo                   | Descripción                           |
| ---------------- | ---------------------- | ------------------------------------- |
| opcode           | uint8                  | valor: 19                             |
| nombreClanLength | uint16                 | cantidad de bytes del nombre del clan |
| nombreClan       | char[nombreClanLength] | nombre del clan                       |

### REVISAR_CLAN (opcode 20)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 20   |

### CLAN_ACEPTAR (opcode 21)

| Campo      | Tipo             | Descripción                |
| ---------- | ---------------- | -------------------------- |
| opcode     | uint8            | valor: 21                  |
| nickLength | uint16           | cantidad de bytes del nick |
| nick       | char[nickLength] | nick del jugador           |

### CLAN_RECHAZAR (opcode 22)

| Campo      | Tipo             | Descripción                |
| ---------- | ---------------- | -------------------------- |
| opcode     | uint8            | valor: 22                  |
| nickLength | uint16           | cantidad de bytes del nick |
| nick       | char[nickLength] | nick del jugador           |

### CLAN_BAN (opcode 23)

| Campo      | Tipo             | Descripción                |
| ---------- | ---------------- | -------------------------- |
| opcode     | uint8            | valor: 23                  |
| nickLength | uint16           | cantidad de bytes del nick |
| nick       | char[nickLength] | nick del jugador           |

### CLAN_KICK (opcode 24)

| Campo      | Tipo             | Descripción                |
| ---------- | ---------------- | -------------------------- |
| opcode     | uint8            | valor: 24                  |
| nickLength | uint16           | cantidad de bytes del nick |
| nick       | char[nickLength] | nick del jugador           |

### DEJAR_CLAN (opcode 25)

| Campo  | Tipo  | Descripción |
| ------ | ----- | ----------- |
| opcode | uint8 | valor: 25   |
