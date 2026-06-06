# Minuta de reunión — TP Argentum

**Fecha:** sábado 6 de junio  
**Tema principal:** revisión de avance del editor, demo del juego, rendimiento, protocolo, Valgrind y alcance para la próxima semana.

---

## 1. Resumen ejecutivo

Durante la reunión se revisó el estado actual del TP Argentum, principalmente el editor de mapas, la demo del juego, el rendimiento del cliente, el uso de Valgrind, el protocolo de comunicación y el alcance esperado para la próxima semana.

La evaluación general fue positiva. Se indicó que el equipo llegó con lo pedido para la semana e incluso avanzó un poco más de lo esperado.

Los principales puntos a corregir o reforzar son:

- Correr Valgrind en escenarios más reales.
- Sacar la interpolación de movimiento.
- Revisar el rendimiento del cliente y del movimiento.
- Completar la lógica de combate.
- Implementar compra y consulta de objetos.
- Mejorar la lógica de NPCs.
- Pulir la experiencia de usuario del editor.
- Hacer más claros los comandos o el chat.

---

## 2. Editor de mapas

### 2.1 Avances mostrados

El editor ya permite crear mapas visualmente y colocar distintos elementos sobre el mapa.

Funcionalidades mostradas:

- Creación visual de mapas.
- Selección de elementos desde la interfaz.
- Colocación de paredes.
- Definición de zonas seguras como rectángulos.
- Colocación de sacerdotes.
- Colocación de banqueros.
- Colocación de comerciantes.
- Borrado de elementos del mapa.
- Selección o cambio de skin desde Qt.

La mecánica de arrastre fue considerada correcta.

### 2.2 Observaciones sobre UX del editor

Se sugirieron mejoras de experiencia de usuario:

#### Agregar zoom in y zoom out

Actualmente las celdas o píxeles se ven muy pequeños. Esto dificulta colocar elementos con precisión.

Se sugirió implementar zoom mediante alguna de estas alternativas:

- `Ctrl + rueda del mouse`.

#### Reemplazar la barra superior por una barra lateral

Se recomendó mover las herramientas desde la barra superior hacia una barra lateral.

La barra lateral debería permitir:

- Ver los elementos disponibles.
- Previsualizar el asset antes de colocarlo.
- Elegir de forma más clara qué se va a pintar en el mapa.

#### Mejorar los assets visuales

Por ahora, la pared se representa como un cuadrado negro. Esto fue aceptado para la etapa actual, pero se sugirió que más adelante podría mejorarse visualmente.

### 2.3 Evaluación del editor

El editor fue considerado correcto para esta etapa. Los cambios solicitados son principalmente de UX y no de lógica fundamental.

---

## 3. Demo del juego

### 3.1 Estado mostrado

Durante la demo se mostró el cliente del juego funcionando.

Se observó que:

- Se pueden conectar personajes.
- Se puede ingresar con nombres de personaje.
- Se puede ver el movimiento de otros jugadores.

### 3.2 Observación principal: movimiento lento

Se detectó que el personaje se mueve de forma demasiado lenta.

El servidor mueve al personaje una celda por tick, donde una celda equivale aproximadamente a 32 píxeles. A partir de eso, se indicó que el movimiento no debería verse tan lento.

Posibles causas mencionadas:

- Renderizado completo del mapa en cada frame.
- Pérdida de frames.
- Saturación de red por envío excesivo de mensajes.
- Problemas en la interpolación del cliente.
- Problemas en el constant rate loop.
- Exceso de trabajo del lado del cliente.

También se observó que al hacer muchos clicks seguidos el personaje se teletransporta. Esto sugiere pérdida de frames o saturación de mensajes.

---

## 4. Interpolación de movimiento

### 4.1 Situación actual

Se implementó interpolación entre la posición actual y la nueva posición para suavizar el movimiento del personaje.

La motivación era lograr una animación más fluida y evitar que el movimiento se viera brusco.

### 4.2 Observación recibida

Se aclaró que la interpolación es una técnica válida en juegos reales, pero que para este TP no debería ser necesaria.

Dado que el juego corre localmente, la interpolación puede estar agregando complejidad y generando efectos secundarios innecesarios.

### 4.3 Decisión recomendada

Se recomendó:

- Quitar la interpolación por ahora.
- Probar si el movimiento mejora al eliminarla.
- Priorizar una implementación simple, estable y predecible.
- Volver a considerar suavizado visual solo si lo básico ya funciona correctamente.

---

## 5. Constant Rate Loop y pérdida de frames

### 5.1 Estado actual

Se consultó si el constant rate loop estaba implementado y si contemplaba pérdida de frames.

Se pasa un contador de frames al `SpriteManager`, y que el sprite manager actualiza la animación según la cantidad de ticks transcurridos, no según el tiempo real.

Esto fue considerado correcto.

### 5.2 Recomendación técnica

Se recomendó que el constant rate loop sea genérico y no dependa directamente de SDL.

La idea sugerida fue que el loop reciba una función, y que esa función acepte como parámetro la cantidad de frames perdidos o transcurridos.

De esa forma, el mismo mecanismo podría reutilizarse tanto en el cliente como en el servidor.

---

## 6. Protocolo, hilos y snapshots

### 6.1 Organización de hilos

El equipo explicó que el cliente utiliza dos flujos principales:

- Uno para enviar comandos al servidor.
- Otro para recibir respuestas del servidor.

Del lado del servidor también hay lógica de sender y receiver por cliente.

### 6.2 Protocolo

Se indicó que el protocolo ya está definido.

La estructura mencionada fue:

- Envío de un byte para el opcode.
- Envío de bytes adicionales para el payload, según el tipo de mensaje o comando.

### 6.3 Punto pendiente de documentación

No quedó completamente claro si la serialización de eventos y mensajes se realiza de forma manual o mediante una abstracción común.

Conviene documentar:

- Qué componente arma cada paquete.
- Qué componente interpreta el opcode.
- Cómo se representa cada payload.
- Si cada evento se serializa manualmente.
- Si existe una interfaz común para serializar y deserializar mensajes.
- Cómo se manejan errores de protocolo.

---

## 7. Valgrind

### 7.1 Estado previo

Al inicio de la reunión se preguntó si el servidor ya había sido corrido con Valgrind.

La respuesta fue que todavía no.

Se indicó que esto era urgente.

### 7.2 Prueba realizada durante la reunión

Durante la demo se ejecutó el servidor con Valgrind y se cerró con `Q`.

En ese escenario, sin jugadores conectados, no se detectaron leaks.

### 7.3 Limitación de la prueba

Se aclaró que esa prueba no alcanza como validación completa.

Todavía deben probarse escenarios más representativos:

- Servidor con jugadores conectados.
- Conexión de clientes.
- Desconexión de clientes.
- Cierre del servidor con clientes conectados.
- Uso de comandos.
- Interacciones con NPCs.
- Compra y consulta de objetos.
- Escenarios de combate.
- Persistencia.

---

## 8. Persistencia

Se preguntó si el sistema tenía persistencia. La respuesta fue: si

No se profundizó demasiado en este punto, pero se entiende que el sistema puede recuperar datos o personajes previamente creados.

---

## 9. Alcance para la próxima semana

La próxima semana será la demo o preentrega, por lo que se definió un conjunto concreto de prioridades.

### 9.1 Prioridades principales

#### 1. Correr Valgrind en escenarios reales

No alcanza con iniciar y cerrar el servidor vacío.

Se deben probar escenarios con jugadores conectados y acciones reales dentro del juego.

#### 2. Sacar la interpolación

La interpolación no es prioritaria para esta entrega y puede estar afectando el comportamiento del movimiento.

#### 3. Revisar rendimiento

Se debe investigar por qué el movimiento se ve lento.

Aspectos a revisar:

- Renderizado completo del mapa.
- Pérdida de frames.
- Saturación de red.
- Cantidad de mensajes enviados.
- Trabajo realizado por el cliente.
- Correcta implementación del constant rate loop.

#### 4. Implementar y corregir peleas

Se pidió que la lógica de pelea quede bien controlada.

Reglas esperadas:

- El jugador puede pelear fuera de zona segura.
- El jugador no puede pelear dentro de zona segura.
- El sistema debe responder correctamente cuando la acción no está permitida.

#### 5. Implementar objetos

Se pidió avanzar con objetos.

Funcionalidades esperadas:

- Comprar objetos.
- Consultar objetos.
- Integrar objetos con NPCs, especialmente comerciantes.

#### 6. Arreglar lógica de NPCs

Se pidió mejorar la lógica de NPCs.

NPCs mencionados:

- Sacerdote.
- Comerciante.
- Banquero.

#### 7. Avanzar el editor

Se pidió mejorar el editor, especialmente en términos de UX.

Puntos esperados:

- Zoom.
- Barra lateral.
- Mejor visualización de herramientas o assets.
- Mayor comodidad para colocar elementos.

#### 8. Arreglar chat o comandos

Se indicó que el chat debe poder verse correctamente o, en su defecto, los comandos por teclado deben estar claros.

También se sugirió:

- Documentar los comandos.
- Agregar ayuda visual.
- Mostrar qué tecla ejecuta cada acción.
- Evitar que quien pruebe el juego tenga que adivinar los comandos.

---

## 10. Funcionalidades postergadas

Se decidió no priorizar todavía las siguientes funcionalidades:

- Mazmorra.
- Clanes.

La mazmorra se dejó para más adelante porque no suma tanto para esta primera entrega. El tema de clanes también se postergó.

---

## 11. Diferencia de niveles y principiantes

Se mencionó que, si el equipo quiere agregar una regla para impedir peleas por diferencia de nivel, está bien.

Comentamos que eso ya existe, porque cuando se crean dos personajes principiantes no se pueden atacar.

### 11.1 Punto a verificar

Conviene revisar si esta lógica está:

- Implementada de forma explícita.
- Correctamente integrada con la lógica de combate.
- Cubierta por pruebas.
- Bien comunicada al usuario cuando intenta atacar.
- Compatible con la regla de zonas seguras.

---

## 12. UI de comandos e interacción con NPCs

### 12.1 Consulta realizada

Se preguntó si acciones como pedir curación a un sacerdote deberían ejecutarse por chat o mediante un botón.

### 12.2 Respuesta recibida

Funcionalmente, ambas opciones son válidas.

Puede resolverse mediante:

- Comando por texto.
- Tecla o keybinding.
- Botón discreto en la interfaz.
- Botón asociado al inventario o a una barra inferior.

### 12.3 Recomendación de UI

Si no se tiene una barra lateral tipo Argentum, se sugirió pensar en una barra inferior con inventario.

En esa barra podrían ubicarse botones discretos para acciones como:

- Curarse.
- Comprar.
- Consultar.
- Interactuar con NPCs.

Se recomendó evitar botones grandes que ensucien la pantalla o rompan el tono visual del juego.

---

## 13. Evaluación general recibida

La evaluación general fue positiva.

Se indicó que:

- La demo está bastante completa.
- El equipo llegó con lo pedido.
- El proyecto está en línea.
- El avance es bueno para la etapa actual.
- La lógica de negocio se ve bastante bien.
- Lo más importante a corregir ahora está en rendimiento, combate, NPCs y detalles visuales.

---

## 14. Pendientes consolidados

| Prioridad | Pendiente           | Detalle                                                                      |
| --------- | ------------------- | ---------------------------------------------------------------------------- |
| Alta      | Correr Valgrind     | Probar con jugadores conectados y escenarios reales, no solo servidor vacío. |
| Alta      | Sacar interpolación | Validar si mejora el movimiento y evita comportamiento raro.                 |
| Alta      | Revisar rendimiento | Investigar lentitud, pérdida de frames, renderizado y saturación de red.     |
| Alta      | Implementar peleas  | Permitir combate fuera de zona segura y bloquearlo dentro de zona segura.    |
| Alta      | Implementar objetos | Permitir comprar y consultar objetos.                                        |
| Alta      | Corregir NPCs       | Revisar sacerdote, comerciante y banquero.                                   |
| Media     | Mejorar editor      | Agregar zoom, barra lateral y mejor visualización de assets.                 |
| Media     | Chat o comandos     | Hacer visible el chat o documentar claramente los comandos por teclado.      |
| Media     | Ayuda visual        | Mostrar comandos disponibles o feedback de acciones.                         |
| Baja      | Mazmorra            | Postergada para más adelante.                                                |
| Baja      | Clanes              | Postergado para más adelante.                                                |

---

## 15. Plan de acción recomendado

### Paso 1: estabilizar movimiento y rendimiento

Acciones recomendadas:

- Quitar interpolación.
- Medir si el movimiento mejora.
- Revisar cantidad de mensajes enviados por segundo.
- Revisar si el cliente renderiza el mapa completo en cada frame.
- Revisar pérdida de frames.
- Confirmar que el constant rate loop esté funcionando correctamente.

### Paso 2: validar memoria con Valgrind

Acciones recomendadas:

- Ejecutar servidor con Valgrind.
- Conectar uno o más clientes.
- Realizar acciones reales.
- Desconectar clientes.
- Cerrar el servidor.
- Verificar leaks y errores de memoria.

### Paso 3: cerrar combate

Acciones recomendadas:

- Implementar pelea fuera de zona segura.
- Bloquear pelea dentro de zona segura.
- Validar mensajes de error.
- Probar casos con principiantes o diferencia de nivel.
- Agregar pruebas si el tiempo lo permite.

### Paso 4: completar NPCs y objetos

Acciones recomendadas:

- Implementar compra de objetos.
- Implementar consulta de objetos.
- Integrar comerciante.
- Revisar sacerdote.
- Revisar banquero.
- Confirmar que las acciones estén disponibles desde comandos, teclas o UI.

### Paso 5: pulir UX para la demo

Acciones recomendadas:

- Agregar zoom al editor.
- Mejorar barra de herramientas.
- Documentar comandos.
- Mostrar ayuda visual mínima.
- Asegurar que quien evalúe pueda probar el juego sin adivinar controles.

---

## 16. Riesgos identificados

| Riesgo                         | Impacto | Mitigación                                                   |
| ------------------------------ | ------- | ------------------------------------------------------------ |
| Movimiento lento o trabado     | Alto    | Quitar interpolación, revisar renderizado y mensajes de red. |
| Leaks con clientes conectados  | Alto    | Correr Valgrind en escenarios reales.                        |
| Combate incompleto             | Alto    | Priorizar reglas de zona segura y ataques permitidos.        |
| Comandos poco claros           | Medio   | Agregar documentación o ayuda visual.                        |
| Editor incómodo de usar        | Medio   | Agregar zoom y barra lateral.                                |
| Exceso de features secundarias | Medio   | Postergar mazmorra y clanes.                                 |

---

## 17. Conclusión

El proyecto fue evaluado positivamente y se encuentra bien encaminado para la próxima preentrega. El foco no debería estar en agregar funcionalidades accesorias, sino en estabilizar lo que ya existe y completar las reglas centrales del juego.

La prioridad para la próxima semana debe ser:

1. Rendimiento y movimiento.
2. Valgrind en escenarios reales.
3. Combate.
4. NPCs y objetos.
5. Editor y comandos visibles.

La interpolación de movimiento, aunque técnicamente interesante, debe retirarse por ahora para reducir complejidad y evitar problemas difíciles de depurar antes de la demo.
