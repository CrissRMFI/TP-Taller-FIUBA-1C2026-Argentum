# Minuta — Reunión final previa a entrega del TP Argentum

---

## 1. Estado general de la demo

La demo fue dada por cerrada y evaluada positivamente.

Se indicó que el funcionamiento general del proyecto está bien y que no se detectó una sección crítica peligrosa en el mecanismo de persistencia, porque el guardado lo realiza el `GameLoop` cada cierta cantidad de ticks, en lugar de delegarlo a handlers concurrentes.

### Conclusión general

El proyecto fue considerado en buen estado para la entrega. La recomendación principal fue no agregar funcionalidades grandes nuevas, sino enfocarse en:

- Corregir bugs.
- Mejorar la experiencia de usuario.
- Dejar documentación clara.
- Preparar correctamente el video y el README.
- Evitar romper funcionalidades que ya están funcionando.

---

## 2. Fecha y forma de entrega

La entrega se debe realizar el **martes**.

### Entregables requeridos

Para la entrega deben estar preparados:

| Entregable    | Estado esperado                               |
| ------------- | --------------------------------------------- |
| Proyecto      | Código final listo en el repositorio          |
| Documentación | README/manual de usuario completo             |
| Video         | Trailer o demostración en YouTube             |
| Instalador    | Debe estar disponible y funcionando           |
| Tests         | Ya existen, pero deben mantenerse funcionando |

Aunque se mencionaron varios ítems, el resumen operativo fue:

1. Proyecto.
2. Documentación.
3. Video.

El instalador y los tests también forman parte de lo esperado, pero ya estaban bastante encaminados según lo conversado.

---

## 3. Repositorio, tag y criterio de evaluación

La entrega se realiza mediante el repositorio.

### Indicaciones importantes

- Deben hacer un **tag**.
- Se evaluará el **último tag válido antes del horario de entrega**.
- Si hay cambios posteriores al horario límite, podrían no ser considerados.

---

## 4. README y documentación

El README debe funcionar como instructivo de usuario.

### Contenido mínimo recomendado para el README

| Sección                | Contenido esperado                                     |
| ---------------------- | ------------------------------------------------------ |
| Descripción del juego  | Qué es el proyecto y qué permite hacer                 |
| Instalación            | Cómo instalar dependencias y compilar                  |
| Ejecución              | Cómo levantar servidor, cliente y editor               |
| Controles              | Teclas, comandos y acciones disponibles                |
| Mecánicas              | Combate, inventario, NPCs, banco, mazmorra, chat, etc. |
| Editor                 | Cómo crear, editar, importar y exportar mapas          |
| Video                  | Link al trailer o demo en YouTube                      |
| Licencia               | Si corresponde                                         |
| Limitaciones conocidas | Bugs o restricciones que no lleguen a corregir         |

### Punto importante

Si algo no llega a corregirse, debe documentarse.

Esto no reemplaza corregir bugs, pero evita que el evaluador lo interprete como un comportamiento desconocido o accidental.

---

## 5. Video / trailer

El video debe subirse a YouTube y enlazarse desde el README.

### Criterio esperado

El video debe mostrar el juego real del grupo.

No debe usar gameplay de Argentum original ni de otros juegos. Se hizo énfasis en evitar trailers que prometan algo que el proyecto no tiene.

### Recomendaciones para el video

- Mostrar el juego real.
- Mostrar varias features en poco tiempo.
- Usar música de fondo sin copyright si quieren.
- Mostrar a varios jugadores conectados si es posible.
- Mostrar combate, NPCs, inventario, banco, chat, editor y mazmorra.
- No hacerlo excesivamente largo.
- Que parezca una presentación atractiva, no solo una grabación cruda de consola.

### Features que conviene mostrar

| Feature                     | Mostrar en video |
| --------------------------- | ---------------- |
| Login / conexión            | Sí               |
| Varios jugadores            | Sí, idealmente   |
| Movimiento                  | Sí               |
| Combate                     | Sí               |
| Muerte / fantasma / revivir | Sí               |
| NPC comerciante             | Sí               |
| Banco                       | Sí               |
| Inventario y equipamiento   | Sí               |
| Hechizos                    | Sí, si funcionan |
| Chat o comandos             | Sí               |
| Editor de mapas             | Sí               |
| Mazmorra                    | Sí               |
| Sonidos / música            | Sí, si suman     |

---

## 6. Recomendación central: no agregar features grandes

Se indicó explícitamente que, estando a pocos días de la entrega, no conviene agregar funcionalidades nuevas grandes.

### Prioridad correcta

La prioridad debe ser:

1. Fixear bugs.
2. Mejorar estabilidad.
3. Mejorar experiencia de usuario.
4. Documentar lo que no esté perfecto.
5. Preparar video y README.

### Features no recomendadas para agregar ahora

Se mencionó que no conviene intentar agregar en estos últimos días:

- Clanes.
- Grandes mecánicas nuevas.
- Cambios profundos de lógica.
- Refactors riesgosos.
- Features que puedan dejar el proyecto “pegado con cinta”.

La idea es no sacrificar estabilidad por sumar funcionalidades de bajo impacto.

---

## 7. Ventana SDL y tamaño de visualización

Uno de los pedidos principales fue mejorar el tamaño de la ventana del cliente SDL.

Durante la demo se veía chico y costaba observar bien el juego.

### Pedido concreto

Permitir que la ventana se pueda:

- Agrandar.
- Maximizar.
- Visualizar mejor durante la demo.

No se pidió soportar cualquier resolución ni rediseñar toda la UI.

### Solución sugerida

Usar una resolución lógica fija y permitir escalar la ventana física.

Se mencionó la idea de usar funciones de SDL relacionadas con tamaño lógico, por ejemplo `SDL_RenderSetLogicalSize` o equivalente según el wrapper usado.

La idea es:

- Mantener la resolución lógica del juego.
- Permitir que la ventana física se agrande.
- Escalar visualmente el contenido.
- No deformar la lógica del juego.

### Criterio esperado

No hace falta implementar fullscreen real. Alcanza con que se pueda agrandar la ventana para que la demo sea más legible.

---

## 8. Editor de mapas

El editor ya venía bien encaminado, pero se mencionaron bugs y restricciones importantes.

### Bug o riesgo detectado

Al redimensionar el mapa, puede ocurrir que se pierda o se borre la mazmorra.

Esto se marcó como algo a revisar.

### Pedido sobre mazmorra

Debe existir una mazmorra. No necesariamente tiene que ser una implementación compleja, pero el proyecto debe demostrar que el editor y el juego soportan una zona de mazmorra.

### Restricciones sugeridas

Se sugirió agregar reglas para evitar inconsistencias:

| Regla                                              | Objetivo                          |
| -------------------------------------------------- | --------------------------------- |
| No permitir guardar/exportar si no hay mazmorra    | Evitar mapas inválidos            |
| No permitir colocar más de una entrada de mazmorra | Evitar ambigüedad                 |
| Permitir volver a colocar la mazmorra si se perdió | Recuperar el mapa sin romperlo    |
| Documentar el comportamiento si no se corrige      | Evitar sorpresas en la evaluación |

### Alternativa aceptable

Si no llegan a corregir completamente el bug, deben documentarlo en el manual de usuario.

---

## 9. Manejo de archivos de mapa y mazmorra

Se explicó que el editor trabaja con un mapa exterior y una mazmorra asociada.

La mazmorra puede estar en un archivo separado, generado a partir del nombre del mapa exterior.

### Aclaración de los docentes

Tener archivos separados para exterior y mazmorra no está mal.

La preocupación es que agrega complejidad y más margen de error:

- Se puede romper si cambia el nombre del archivo.
- Puede ser más difícil limpiar o mantener archivos relacionados.
- Puede perderse la relación entre mapa exterior y mazmorra.

### Conclusión

La solución actual es aceptable si funciona, pero debe estar bien controlada y documentada.

---

## 10. Interpolación de frames / animaciones

Se recordó que el grupo había intentado implementar interpolación de frames o suavizado de animaciones.

Esto fue considerado algo positivo y distintivo del grupo.

### Criterio

Aunque no sea una feature obligatoria, suma como intento técnico y como mejora visual.

Conviene mencionarlo en la documentación o en la lista de features si está funcionando o si quedó como mejora parcial.

---

### Qué debe hacer el grupo

Si el proyecto tiene features adicionales, conviene comunicarlas explícitamente.

Ejemplo:

```md
## Features adicionales implementadas

- Interpolación o suavizado de animaciones.
- Sistema de sonidos para NPCs y acciones.
- Configuración avanzada por TOML.
- Persistencia de jugadores.
- Tests de protocolo con round-trip.
```

La lógica es simple: si algo suma y no se muestra, puede pasar desapercibido.

---

## 12. Bugs y pendientes concretos

| Prioridad | Pendiente                 | Detalle                                                          |
| --------- | ------------------------- | ---------------------------------------------------------------- |
| Alta      | Preparar entrega          | Proyecto, documentación, video, instalador y tests               |
| Alta      | Crear tag final           | El último tag antes del horario límite será evaluado             |
| Alta      | README completo           | Debe explicar instalación, ejecución, controles y features       |
| Alta      | Video en YouTube          | Debe mostrar el juego real                                       |
| Alta      | Agrandar ventana SDL      | Permitir maximizar o escalar la ventana para mejor visualización |
| Alta      | Revisar editor y mazmorra | Evitar perder la mazmorra al redimensionar                       |
| Media     | Documentar bugs conocidos | Especialmente si no llegan a fixearlos                           |
| Media     | Mostrar features extra    | Interpolación, sonidos, configuración, persistencia, tests       |
| Baja      | Agregar features nuevas   | No recomendado salvo que estén ya casi listas                    |

---
