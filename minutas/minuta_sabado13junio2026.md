# Minuta — Reunión TP Argentum Online

**Tipo de reunión:** seguimiento técnico y revisión de demo  
**Tema principal:** editor de mapas, demo del juego, rendimiento, protocolo, configuración, persistencia, concurrencia y alcance para la próxima demo.  
**Estado de lectura:** se revisó la transcripción completa. La transcripción no parece estar cortada, aunque contiene errores propios del reconocimiento automático de voz. Cuando una frase era ambigua, se reconstruyó el sentido por contexto.

---

## 1. Resumen ejecutivo

La reunión fue una revisión extensa del avance del TP. En términos generales, la devolución fue muy positiva: el editor, el cliente, el servidor, la configuración, la documentación, los tests y la demo del juego fueron considerados avanzados para el estado esperado del proyecto.

Los docentes destacaron que el proyecto está bien encaminado y que varias funcionalidades ya están implementadas o muy cerca de estarlo. También marcaron puntos importantes para corregir antes de la próxima demo y antes de la entrega final.

Los principales temas pendientes son:

- Mejorar el editor para soportar importación/exportación más amigable.
- Permitir mapas de dimensión configurable.
- Evaluar redimensionamiento de mapas sin perder contenido.
- Incorporar al menos una mazmorra para la entrega final.
- Revisar secciones críticas y concurrencia, especialmente en conexión de usuarios y persistencia.
- Verificar el movimiento remoto entre clientes, porque se observa a saltos.
- Revisar el constant rate loop para que sea genérico y reutilizable entre cliente y servidor.
- Mostrar mejor el chat y las interacciones de comandos.
- Preparar una demo más sólida para la semana siguiente.
- Preparar un tráiler del juego usando gameplay real.

---

## 2. Editor de mapas

### 2.1 Funcionalidades mostradas

El editor permite actualmente:

- Cargar el mapa existente.
- Editar el mapa.
- Guardar o sobrescribir el mapa editado.
- Exportar el resultado, aunque todavía de forma limitada.
- Hacer zoom in y zoom out.
- Moverse por el mapa usando una mecánica tipo “mano” con la rueda presionada.
- Pintar zonas o elementos.
- Borrar con click derecho.
- Colocar NPCs amigables, como vendedor y sacerdote.
- Definir zonas seguras.
- Diferenciar zonas como ciudad.
- Evitar, por lógica del editor o del mapa, colocar ciertos elementos en zonas no permitidas.

Se valoró positivamente que el editor ya tenga mecánicas funcionales y que se pueda usar para crear o modificar mapas visualmente.

---

### 2.2 Zoom y navegación

Se mencionó que el zoom es una funcionalidad importante y positiva. Aunque durante la reunión no se pudo corroborar visualmente del todo por problemas de control en la máquina compartida, el equipo explicó que ya está implementado.

Quedó pendiente mostrarlo claramente en la próxima demo.

**Pendiente:**

- Mostrar zoom in, zoom out y desplazamiento del mapa en la próxima reunión.

---

### 2.3 Tamaño fijo del mapa

Se detectó que el mapa actual parece tener un tamaño fijo y estar precargado con ciertos bloques, como pasto y zonas de ciudad.

Los docentes indicaron que, desde la perspectiva de un editor de mapas, sería deseable poder crear mapas con dimensiones configurables. La idea es que el usuario pueda elegir el tamaño inicial del mapa y, eventualmente, redimensionarlo si el tamaño quedó chico.

El punto importante no es solamente visual, sino arquitectónico: si el editor permite crear mapas de distintos tamaños, demuestra que el parser, el servidor y el cliente pueden interpretar mapas de manera flexible.

**Recomendaciones:**

- Permitir crear mapas de dimensión `N x M`.
- Evitar depender de un único mapa precargado.
- Evaluar redimensionar el mapa sin perder el contenido ya pintado.
- Considerar que las zonas fuera del mapa sean tratadas como no caminables o colisionables.

---

### 2.4 Carrusel de assets

El editor usa una interfaz tipo carrusel para seleccionar assets. La devolución fue mixta pero mayormente positiva.

Por un lado, se reconoció que el carrusel es original, visualmente agradable y que incluye información útil sobre NPCs, enemigos o elementos. Eso fue valorado porque enriquece la interfaz.

Por otro lado, se comentó que para un editor de mapas quizá no es la interfaz más eficiente. La referencia mencionada fue RPG Maker o herramientas como Photoshop/Gimp, donde los assets suelen verse todos juntos o agrupados por categorías.

Los docentes aclararon explícitamente que no piden cambiar el carrusel y que no resta puntos. Funciona, está bien hecho y no conviene romper algo estable por una preferencia de interfaz.

**Decisión:**

- No es necesario cambiar el carrusel.
- Se puede dejar como está.
- Si se mejora, debe ser solo si no compromete funcionalidades ya estables.

---

### 2.5 Importación y exportación de mapas

Actualmente el editor modifica o sobrescribe el mapa usado por el servidor. Los docentes marcaron que esto funciona, pero que idealmente el editor debería comportarse como una aplicación independiente de edición.

La funcionalidad esperada sería similar a una aplicación de diseño:

- Crear un mapa nuevo.
- Importar un mapa desde una ruta elegida por el usuario.
- Exportar o guardar el mapa en una ruta elegida por el usuario.
- Guardar como, sin necesidad de sobrescribir siempre el mismo archivo.

Se aclaró que el servidor puede seguir usando un único mapa definido por configuración o por ruta fija. El editor, en cambio, debería poder crear y manipular múltiples archivos de mapa aunque después el servidor use solo uno.

**Pendiente obligatorio:**

- Implementar importación y exportación de mapas de forma más amigable para el usuario.

**Mejoras deseables:**

- Pantalla inicial para elegir entre crear mapa nuevo o importar mapa existente.
- File picker para seleccionar desde dónde cargar.
- File picker para seleccionar dónde guardar.
- Evitar sobrescribir automáticamente sin control del usuario.

---

## 3. Mazmorras

### 3.1 Aclaración de alcance

El equipo había interpretado que las mazmorras habían sido sacadas del alcance. Los docentes aclararon que no era así: clanes puede quedar postergado, pero una mazmorra debe existir para la entrega final.

Se aclaró que no necesariamente debe estar para la próxima semana, pero sí para la entrega final.

**Decisión:**

- Debe existir al menos una mazmorra en la entrega final.
- No hace falta que esté terminada para la próxima demo, pero conviene empezar a diseñarla.

---

### 3.2 Implementación mínima aceptable

Como implementación mínima, se sugirió que exista una zona fija del mapa que represente una mazmorra o caverna. Esa zona podría tener un aspecto propio y enemigos más fuertes.

Ejemplo mínimo:

- Una zona marcada como mazmorra.
- Tiles o assets visuales de caverna.
- Mobs con mayor daño o mayor vida.
- Reglas diferenciadas respecto de una ciudad o zona segura.

---

### 3.3 Implementación ideal sugerida

La implementación ideal sugerida fue pensar la mazmorra como una capa o submapa independiente dentro del archivo de mapa.

La analogía usada fue el sistema de capas de Photoshop/Gimp: el mapa principal sería una capa o viñeta, y la mazmorra podría ser otra capa asociada a una entrada en el mapa principal.

La idea sería:

- El mapa principal contiene una entrada a la mazmorra.
- Esa entrada está vinculada a otra matriz o capa.
- Al pisar o interactuar con la entrada, el jugador es transportado a la matriz de la mazmorra.
- La mazmorra se define dentro del mismo archivo de mapa o mediante una estructura vinculada.

**Punto técnico:**

El servidor probablemente debería manejar una matriz adicional por cada mazmorra. No sería simplemente una zona dentro de la misma matriz principal, sino un espacio independiente.

---

## 4. Demo del juego

### 4.1 Estado general de la demo

La demo del juego fue muy bien recibida. Se observaron múltiples funcionalidades ya implementadas:

- Selección de raza y clase.
- Skin base según raza y clase.
- Personalización básica del personaje.
- Movimiento libre.
- Renderizado correcto del personaje.
- NPCs amigables.
- NPCs hostiles.
- Inventario funcional.
- Compra de objetos.
- Equipamiento de objetos.
- Cambio visual al equipar.
- Combate contra criaturas.
- Sistema de muerte.
- Transformación en fantasma.
- Loot al morir.
- Hechizos.
- Consumo de maná.
- Ganancia de experiencia.
- Banco.
- Depósito de dinero.
- Música y sonidos.
- Chat o sistema de comandos.

La devolución general fue que el cliente está muy bien armado visualmente y que las pantallas se ven sólidas.

---

### 4.2 Rendimiento

Durante la demo se corrió el cliente y se observó consumo mediante `top`. El consumo reportado fue bajo, alrededor de 6% a 8% para el cliente.

La devolución fue muy positiva: el rendimiento del cliente fue considerado excelente.

**Conclusión:**

- El cliente no parece estar consumiendo recursos excesivos.
- La performance general fue considerada muy buena.

---

### 4.3 Valgrind

Se pidió correr el servidor con Valgrind. En la transcripción aparece reconocido como “Balgrind”, pero por contexto se refiere a Valgrind.

Se ejecutó el servidor con un comando equivalente a:

```bash
valgrind --leak-check=yes <binario_servidor> <parámetros>
```

Durante la prueba, se conectó al menos un cliente y luego se cerró el servidor con `Q`. El resultado observado fue correcto: no aparecieron leaks en esa prueba.

**Resultado:**

- Valgrind no reportó leaks en el escenario probado.
- El cierre del servidor con un cliente conectado pareció estar bien controlado.
- El cliente quedó colgado momentáneamente al cerrarse el servidor, pero luego pudo cerrarse correctamente.

**Recomendación:**

- Mostrar en consola o ventana SDL un mensaje claro cuando se pierde la conexión con el servidor.

Ejemplo:

```text
Conexión cerrada por el servidor.
```

---

## 5. Jugabilidad y features observadas

### 5.1 Inventario y objetos

Se mostró un inventario funcional ubicado a la derecha de la pantalla. También se mostró que el jugador puede cargar oro mediante un comando o tecla de debug, comprar objetos y equiparlos.

Al equipar una espada u otro elemento, se observó que cambia la apariencia del personaje.

La devolución fue muy positiva, especialmente porque el equipamiento se refleja visualmente.

---

### 5.2 Interacción con comerciantes

Se observó que al interactuar con un comerciante aparece información sobre lo que vende. También se mencionó que el NPC saluda o emite audio al interactuar.

Se recomendó que la interfaz del comerciante sea similar a la del banco, porque la ventana del banco fue considerada clara y bien lograda.

**Pendiente sugerido:**

- Unificar o mejorar la interfaz del comerciante para que se parezca a la del banco.
- Indicar visualmente que el NPC es clickeable al pasar el mouse por encima.
- Agregar un texto discreto como “comprar” o una ayuda contextual.

---

### 5.3 Banco

El banco fue mostrado y valorado positivamente. Se observó una interfaz clara para depositar.

Se mencionó la posibilidad de depositar dinero, por ejemplo con un comando como:

```text
deposit 5000
```

La funcionalidad fue considerada bien encaminada.

---

### 5.4 Combate, muerte y loot

Se mostró combate contra criaturas. El jugador puede atacar, recibir daño y morir. Al morir, se transforma en fantasma y deja objetos o loot.

También se vio que el sistema de muerte bloquea o controla la reaparición durante un tiempo.

Se aclaró que no es obligatorio que al morir se dropeen todos los ítems o que se implemente una lógica exacta de drop compleja, salvo que el equipo decida hacerlo.

**Estado:**

- Combate funcional.
- Muerte funcional.
- Fantasma funcional.
- Loot visible.
- Experiencia y maná integrados con acciones.

---

### 5.5 Hechizos y meditación

Se mostró que el jugador puede usar hechizos, consumir maná y ganar experiencia. También se preguntó por la meditación con la tecla `M`.

---

### 5.6 Chat y comandos

Se mencionó que existe chat y que permite escribir comandos. Por ejemplo, acercarse a un comerciante y escribir `listar` produce un resultado similar a hacer click sobre él.

También se mencionó que hay comando o interacción para robar a otro jugador.

Sin embargo, el chat no fue revisado en profundidad.

**Pendiente para próxima demo:**

- Mostrar explícitamente el chat.
- Verificar que el mensaje se vea correctamente.
- Verificar si el otro jugador recibe notificación.
- Documentar comandos disponibles.

---

## 6. Multijugador

### 6.1 Dos clientes conectados

Se conectaron dos clientes y ambos pudieron verse dentro del juego. Se probó movimiento, interacción y visualización de acciones desde un cliente hacia el otro.

También se probó que el combate y muerte de un jugador o criatura pueda verse desde otro cliente.

**Resultado:**

- El multijugador está funcionando.
- Los clientes pueden verse entre sí.
- Se puede observar loot y muerte desde otro cliente.

---

### 6.2 Problema de movimiento remoto

Se observó que el movimiento del otro jugador se ve “a saltos” o trabado, mientras que el movimiento local se ve fluido.

La explicación técnica sugerida fue que el servidor estaría enviando coordenadas lógicas del servidor, por ejemplo coordenadas de celda, en lugar de coordenadas de pantalla o coordenadas SDL. Eso hace que el cliente remoto renderice al otro jugador por casillas y no con movimiento suave.

**Diagnóstico probable:**

- El jugador local se mueve fluidamente porque el cliente interpola o actualiza localmente.
- El jugador remoto se ve a saltos porque recibe posiciones discretas/truncadas desde el servidor.

**Recomendación:**

- Revisar qué coordenadas envía el servidor.
- Evaluar enviar coordenadas compatibles con el sistema de renderizado del cliente.
- Mantener separación entre lógica del servidor y visualización del cliente, pero evitar que el cliente remoto reciba información demasiado truncada para animar.

---

## 7. Cámara y bordes del mapa

Se discutió el comportamiento de la cámara cerca de los bordes del mapa. Al principio pareció un bug, pero luego se entendió que la cámara está evitando mostrar zonas fuera del mapa.

Se aclaró que también sería aceptable que la cámara siga al jugador y que fuera del mapa se vea fondo negro.

**Recomendación:**

- Definir una política clara para zonas fuera del mapa:
  - Se pueden ver como fondo negro.
  - Deben ser no caminables.
  - Deben considerarse colisionables o fuera de bounds.

Este punto será especialmente importante si el editor permite mapas no necesariamente “llenos” o con zonas vacías.

---

## 8. Arquitectura de hilos

### 8.1 Servidor

Se repasó la arquitectura de hilos del servidor.

El esquema explicado fue:

- Hilo principal (`main`), que inicializa y espera la tecla `Q`.
- Hilo `Acceptor`, que escucha conexiones.
- Hilo del `GameLoop`.
- Por cada cliente conectado:
  - Un hilo receptor.
  - Un hilo enviador.

Resumen:

```text
Servidor = main + acceptor + gameloop + 2 hilos por cliente
```

La respuesta fue considerada correcta.

---

### 8.2 Cliente

En el cliente se aclaró que SDL corre en el hilo principal. Además, por cada usuario o conexión existen hilos separados para enviar y recibir.

El renderizado, la carga de texturas y el manejo de eventos SDL se hacen desde el hilo principal, no desde los hilos sender/receiver.

Esto fue considerado correcto.

---

## 9. SDL, renderizado y recursos

### 9.1 Carga de recursos

Se revisó cómo se cargan recursos gráficos y sonoros. El equipo mostró una estructura con archivos de configuración para recursos, imágenes, sonidos y sprites.

Se observó que:

- El fondo base se carga como background.
- Hay configuración de recursos con paths a imágenes y sonidos.
- Hay un cargador de texturas.
- Se usa una especie de manager o cache para evitar cargar repetidamente recursos ya cargados.

La devolución fue positiva.

---

### 9.2 Renderizado

Se preguntó si se renderiza todo el mapa o solo lo visible. El equipo indicó que solo se hace `render copy` de lo que corresponde a la vista.

Esto fue considerado correcto.

**Punto positivo:**

- No se renderizan mobs o elementos fuera de la pantalla.
- El cliente parece manejar bien la vista y el consumo.

---

### 9.3 Manejo de eventos SDL

El equipo explicó que usa un `InputHandler` que traduce eventos SDL a comandos del juego. Luego esos comandos pasan por una capa de negocio y se transforman en comandos enviados a la cola del enviador del cliente.

Se observó el uso de `pollEvent` y manejo de eventos como mouse wheel, key down y otros.

**Estado:**

- La estructura fue considerada razonable.
- Se mantiene separación entre eventos SDL, lógica de input y comandos de protocolo.

---

## 10. Constant Rate Loop

Se observó que el constant rate loop está implementado en el cliente y depende de SDL.

Los docentes recomendaron que el constant rate loop sea genérico y no dependa de SDL, para poder reutilizarlo también en el servidor.

**Pendiente técnico:**

- Extraer el constant rate loop a una clase o utilidad independiente.
- Evitar dependencia directa de SDL en esa abstracción.
- Reutilizarlo tanto en cliente como en servidor si corresponde.

---

## 11. Protocolo

### 11.1 Estructura del protocolo

Se repasó el protocolo de comunicación. El equipo explicó que el cliente arma comandos con:

- Opcode.
- Payload.

El opcode se envía como un byte y luego se envía el payload correspondiente.

Del lado del servidor se lee el opcode, se interpreta el comando y se deserializa el payload.

---

### 11.2 Flujo de recepción en servidor

El flujo explicado fue:

1. El receptor del cliente llama al protocolo para recibir un comando.
2. El protocolo devuelve un `ComandoJugador` deserializado.
3. El receptor no procesa la lógica del comando.
4. El receptor encola el comando en la cola del `GameLoop`.
5. El `GameLoop` procesa el comando y llama a la lógica del juego.

Este diseño fue considerado correcto porque separa recepción, protocolo y lógica de juego.

---

### 11.3 Tests de protocolo

El equipo indicó que tiene 88 tests, incluyendo round trips para los opcodes y casos borde.

Se mencionó que hay 53 opcodes testeados y que además existen pruebas de errores.

La devolución fue muy positiva. Los docentes esperaban menos tests y valoraron que el protocolo esté cubierto.

**Punto positivo:**

- Buena cobertura de protocolo.
- Tests con sockets reales en los casos donde puede haber bugs relevantes.
- Round trips para serialización/deserialización.

---

## 12. Configuración

### 12.1 Configuración del servidor

Se mostró un archivo de configuración avanzado para el servidor. Incluye, entre otras cosas:

- Puerto.
- Ticks.
- Rutas de persistencia.
- Vida.
- Maná.
- Razas.
- Clases.
- Experiencia.
- Oro.
- Combate.
- Inventario.
- Armas.
- Armaduras.
- Criaturas.
- Movimiento de criaturas cada cierta cantidad de ticks.

La devolución fue muy positiva. Se destacó que evita hardcodear valores y permite modificar reglas del juego desde configuración.

---

### 12.2 Configuración de recursos

También se mostró configuración de recursos del cliente, como:

- Imágenes.
- Sonidos.
- Sprites.
- Paths a assets.
- Volumen.

Esto también fue considerado correcto.

---

### 12.3 Evaluación general de configuración

Los docentes destacaron que la configuración es más avanzada de lo mínimo pedido.

**Conclusión:**

- Configuración validada.
- Buen diseño para evitar valores hardcodeados.
- Suma positivamente al proyecto.

---

## 13. Persistencia

### 13.1 Estado actual

Se mencionó que hay persistencia funcionando, aunque también se aclaró que hubo problemas recientes por una corrección subida el mismo día.

El equipo explicó que si un usuario se registra con un nombre, queda persistido. Si se cierra y vuelve a abrir el servidor, el usuario sigue existiendo.

También se explicó que si un cliente intenta entrar con un nombre ya conectado, el servidor debería rechazarlo y mostrar un mensaje de error.

---

### 13.2 Problema reciente

Se mencionó que hasta el día anterior funcionaba, pero al corregir un caso de datos mal formados se generó un problema de compilación o integración en algunas máquinas.

También se dijo que, ante datos mal formados, el cliente renderizaba un personaje invisible para evitar crashear.

**Pendiente:**

- Revisar persistencia luego de los últimos cambios.
- Evitar que datos mal formados deriven en personajes invisibles sin diagnóstico claro.
- Asegurar que todos puedan compilar después de los cambios.

---

## 14. Concurrencia y secciones críticas

### 14.1 Pregunta técnica

Los docentes preguntaron por las secciones críticas del servidor, especialmente dónde hay recursos compartidos y dónde se usan mutex.

Se identificó un `MonitorClientes` con un mutex que protege operaciones sobre clientes conectados.

---

### 14.2 Punto delicado detectado

Se señaló una posible race condition en el flujo de conexión y validación de nombres.

El problema conceptual es el siguiente:

1. Dos clientes intentan conectarse al mismo tiempo con el mismo nombre.
2. Ambos consultan si el nombre está registrado o conectado.
3. Si cada llamada toma el mutex por separado, puede ocurrir que ambas validaciones pasen antes de que se actualice el estado global.
4. Eso podría permitir una inconsistencia.

El docente aclaró que no necesariamente están mal, pero que el equipo debe poder explicar si esa zona está protegida correctamente.

---

### 14.3 Recomendación técnica

Se sugirió revisar con más cuidado:

- Monitor de clientes.
- Aceptor.
- Validación de conexión.
- Persistencia de usuarios.
- Registro de nombres conectados.
- Uso de mutex por llamada versus mutex para toda la operación crítica.

También se sugirió que quizá sería conveniente un monitor específico para persistencia.

**Pendiente importante para próxima reunión:**

- Repasar secciones críticas.
- Poder explicar dónde hay mutex.
- Poder justificar qué recurso compartido protege cada mutex.
- Poder explicar si la persistencia es sección crítica o no.
- Poder responder qué pasa con dos clientes conectándose simultáneamente con el mismo nombre.

---

## 15. Instalador y desinstalador

Se mostró un instalador que documenta lo que instala. También existe un desinstalador.

Se preguntó si el desinstalador borra dependencias que el usuario ya tenía instaladas, como Qt6. El equipo explicó que borra lo que el instalador instala.

La devolución fue positiva, aunque se observó que hay que tener cuidado con dependencias compartidas del sistema.

**Punto positivo:**

- Existe instalador.
- Existe desinstalador.
- Está documentado.

**Riesgo a revisar:**

- Evitar borrar dependencias que el usuario tenía previamente y que no pertenecen exclusivamente al TP.

---

## 16. Documentación

Se mostró documentación general del proyecto, incluyendo:

- Modelo de dominio.
- Arquitectura del cliente.
- Protocolo cliente-servidor.
- Estructura de hilos.
- Decisiones de diseño.

La devolución fue positiva. Se consideró que la documentación está bien encaminada.

**Pendiente sugerido:**

- Documentar mejor los archivos de configuración y sus campos.
- Documentar comandos del chat o keybindings.
- Documentar flujo de importación/exportación de mapas cuando esté terminado.

---

## 17. Tráiler del juego

Los docentes mencionaron que para la entrega habrá que preparar un tráiler o video del juego.

Se recomendó que el tráiler muestre gameplay real del proyecto y no clips externos ni material que no represente al juego.

También se sugirió que, si es posible, jueguen varios integrantes al mismo tiempo usando alguna herramienta que permita conectarse como si estuvieran en la misma red.

**Recomendaciones para el tráiler:**

- Mostrar gameplay real.
- Mostrar varios jugadores conectados.
- Mostrar diferentes clases, razas o personajes.
- Mostrar combate.
- Mostrar NPCs.
- Mostrar inventario.
- Mostrar sonidos o música si se puede capturar.
- Evitar usar clips de otros juegos.

---

## 18. Alcance para la próxima semana

### 18.1 Pendientes prioritarios

| Prioridad | Tema                    | Acción esperada                                                        |
| --------- | ----------------------- | ---------------------------------------------------------------------- |
| Alta      | Secciones críticas      | Repasar y poder explicar mutex, monitores y recursos compartidos       |
| Alta      | Movimiento remoto       | Revisar por qué otros jugadores se ven a saltos                        |
| Alta      | Editor                  | Mostrar zoom, scroll y navegación correctamente                        |
| Alta      | Importación/exportación | Avanzar hacia file picker o flujo más claro                            |
| Alta      | Mapas `N x M`           | Evaluar creación de mapas de dimensión configurable                    |
| Media     | Mazmorra                | Diseñar solución; no necesariamente implementarla para la próxima demo |
| Media     | Chat                    | Mostrar funcionamiento real y notificaciones                           |
| Media     | Constant rate loop      | Evaluar extracción a componente genérico                               |
| Media     | Persistencia            | Revisar problemas recientes y consistencia de nombres                  |
| Baja      | Carrusel                | No cambiar salvo que sobre tiempo                                      |

---

### 18.2 Pendientes para entrega final

| Tema          | Requisito                                               |
| ------------- | ------------------------------------------------------- |
| Mazmorra      | Debe existir al menos una                               |
| Editor        | Debe poder importar/exportar mapas                      |
| Mapas         | Idealmente deben poder tener dimensiones configurables  |
| Cliente       | Debe mostrar correctamente jugadores remotos            |
| Servidor      | Debe manejar correctamente concurrencia y desconexiones |
| Configuración | Debe mantenerse documentada y funcional                 |
| Tráiler       | Debe mostrar gameplay real                              |

---

## 19. Riesgos identificados

### 19.1 Riesgo de concurrencia

El mayor riesgo técnico detectado está en las secciones críticas. No alcanza con “tener un mutex”; hay que asegurarse de que la operación completa que debe ser atómica esté protegida.

Ejemplo delicado:

```text
validar nombre disponible -> registrar nombre -> agregar cliente
```

Si esos pasos se protegen por separado, puede haber una race condition.

---

### 19.2 Riesgo de alcance en mazmorras

El equipo interpretó que las mazmorras estaban fuera del alcance. Ahora quedó claro que al menos una debe existir. Esto puede impactar en planificación.

**Recomendación:**

Diseñar una versión mínima cuanto antes para evitar llegar a la entrega final con una funcionalidad grande sin integrar.

---

### 19.3 Riesgo de editor demasiado acoplado al mapa actual

Si el editor solo sobrescribe un mapa fijo, puede quedar corto respecto de la expectativa de un editor real.

**Recomendación:**

Separar mentalmente:

- El editor puede crear muchos mapas.
- El servidor usa uno solo.
- El parser debe poder leer cualquier mapa válido exportado por el editor.

---

### 19.4 Riesgo de movimiento remoto poco fluido

Aunque no parece grave para la lógica del juego, visualmente puede afectar la demo. Si un jugador remoto se ve saltando por casillas, puede dar impresión de lag aunque la red funcione bien.

**Recomendación:**

Revisar coordenadas enviadas y renderizado remoto.

---

## 20. Decisiones y aclaraciones importantes

- El carrusel de assets no hace falta cambiarlo.
- El editor está bien encaminado.
- La importación/exportación de mapas sí debe estar.
- El servidor juega con un mapa definido, pero el editor debe poder crear o editar más de uno.
- Debe haber al menos una mazmorra para la entrega final.
- Clanes pueden quedar para más adelante si no llegan.
- El cliente fue evaluado muy positivamente.
- Valgrind no mostró leaks en la prueba realizada.
- La configuración fue considerada muy buena.
- Los tests de protocolo fueron valorados positivamente.
- La documentación está bien encaminada.
- Para la próxima reunión van a preguntar de nuevo sobre secciones críticas.

---

## 21. Plan de acción recomendado

### Paso 1 — Cerrar riesgos técnicos

1. Revisar `MonitorClientes`.
2. Revisar flujo del `Acceptor`.
3. Revisar validación de nombres conectados.
4. Revisar persistencia bajo concurrencia.
5. Identificar todas las secciones críticas y documentarlas.

---

### Paso 2 — Mejorar demo multijugador

1. Revisar movimiento remoto.
2. Determinar si el servidor envía coordenadas truncadas.
3. Ajustar renderizado de jugadores remotos.
4. Probar dos clientes sin Valgrind.
5. Verificar combate y muerte visibles desde ambos clientes.

---

### Paso 3 — Completar editor mínimo esperado

1. Mostrar zoom y scroll.
2. Agregar flujo claro de importar/exportar.
3. Evaluar creación de mapa nuevo.
4. Evaluar dimensión configurable.
5. Evitar sobrescribir siempre el mismo mapa.

---

### Paso 4 — Diseñar mazmorra

1. Decidir si será zona dentro del mapa o capa/submapa.
2. Definir entrada a mazmorra.
3. Definir matriz o capa de mazmorra.
4. Definir mobs específicos.
5. Integrarla con parser, servidor y cliente.

---

### Paso 5 — Preparar entrega y tráiler

1. Documentar comandos y controles.
2. Grabar gameplay real.
3. Mostrar varios jugadores conectados.
4. Mostrar combate, NPCs, inventario y sonidos.
5. Evitar material externo que no represente al TP.

---

## 22. Lectura crítica

El proyecto está fuerte, pero el peligro ahora es confiarse. La demo mostró muchas funcionalidades y eso juega a favor, pero también aumenta la superficie de bugs.

Los puntos que más pueden doler en una corrección no son los visuales, sino los de arquitectura:

- Concurrencia mal protegida.
- Persistencia inconsistente.
- Parser acoplado a un único mapa.
- Editor que parece editor pero no puede crear/importar/exportar de verdad.
- Movimiento multijugador visualmente roto.

Mi recomendación es no perseguir “chiches” nuevos hasta cerrar esos riesgos. El carrusel está lindo, el inventario está fachero, los sonidos suman; pero si dos clientes entran con el mismo nombre por una race condition, el castillo medieval se convierte en castillo de naipes.

---

## 23. Checklist corto para llevar a la próxima reunión

- [ ] Mostrar zoom in y zoom out del editor.
- [ ] Mostrar scroll o desplazamiento dentro del editor.
- [ ] Explicar cómo se importan mapas.
- [ ] Explicar cómo se exportan mapas.
- [ ] Definir si habrá mapas `N x M`.
- [ ] Definir estrategia de mazmorra.
- [ ] Probar dos clientes sin Valgrind.
- [ ] Corregir o explicar movimiento remoto a saltos.
- [ ] Mostrar chat funcionando.
- [ ] Mostrar desconexión del servidor con mensaje claro.
- [ ] Revisar Valgrind con clientes conectados.
- [ ] Repasar secciones críticas.
- [ ] Explicar qué protege cada mutex.
- [ ] Revisar persistencia con conexiones simultáneas.
- [ ] Documentar comandos y keybindings.
- [ ] Preparar idea para tráiler con gameplay real.
