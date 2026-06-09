# Argentum Online — Decisiones de Diseño del Modelo de Dominio

## Posicion como Value Object

Se modeló `Posicion` como clase separada en lugar de atributos sueltos
`x`, `y`, `mapaId` en cada entidad.

**Por qué:** es reutilizada por `Personaje`, `Criatura`, `NPCCiudad` y
`ObjetoEnSuelo`. Sin value object, un cambio en su representación requeriría
modificar todas las entidades que la usan.

**Alternativa descartada:** atributos sueltos en cada clase. Más simple
inicialmente, pero genera duplicación y acopla todas las entidades a la
representación interna de la posición.

---

## Posicion como Value Object con métodos de distancia

`Posicion` encapsula `x`, `y` y `mapaId`, y expone métodos como
`distanciaEuclidea`, `distanciaManhattan` y `esAdyacente`.

**Por qué:** la distancia euclídea aparece en reglas como el tiempo de
resurrección y el bonus de clan por proximidad, mientras que la adyacencia
aparece en el combate cuerpo a cuerpo. Centralizar estos cálculos evita
duplicación y errores de conversión de tipos al restar coordenadas.

**Alternativa descartada:** funciones libres o métodos en cada entidad.
Duplica la lógica de conversión y dispersa los invariantes de distancia a lo
largo del código.

---

## Raza y ClasePersonaje como enum class

Las cuatro razas y las cuatro clases se modelan como `enum class` en lugar de
constantes enteras o strings.

**Por qué:** el compilador verifica exhaustividad en los `switch` de
`ConfigJuego`, por ejemplo en factores de vida, maná y meditación. Si se agrega
una nueva raza o clase sin actualizar los helpers, el compilador puede advertirlo.

**Alternativa descartada:** constantes enteras o strings. Más fáciles de
serializar, pero sin verificación estática y con mayor riesgo de errores en
runtime.

---

## Herencia para Items

Se eligió herencia (`Arma`, `Baculo`, `Defensa`, `Pocion` heredan de `Item`)
sobre un único struct con todos los campos posibles.

**Por qué:** cada subtipo tiene atributos genuinamente distintos sin
superposición. `Arma` tiene daño y si es de distancia; `Baculo` tiene hechizo,
daño y costo de maná; `Pocion` tiene tipo y cantidad; `Defensa` tiene defensa
mínima, defensa máxima y slot defensivo.

Un struct único tendría campos irrelevantes según el tipo, lo cual sería
confuso y más propenso a errores.

**Alternativa descartada:** un único struct `Item` con todos los campos y un
enum `TipoItem` para discriminar. Más simple para serialización, pero genera
estructuras con campos que no aplican según el tipo.

---

## Separacion de Inventario y Equipamiento

Se separaron en dos clases en lugar de tener todo en `Inventario`.

**Por qué:** tienen responsabilidades distintas. `Inventario` gestiona el
almacenamiento. `Equipamiento` gestiona qué está en uso activo e impone la
invariante `NOT (arma AND baculo equipados simultaneamente)`.

Mezclar ambas responsabilidades dificulta hacer cumplir la invariante y
complica el testing.

**Alternativa descartada:** una sola clase `Inventario` con slots especiales
para equipados. Más simple inicialmente, pero la invariante queda dispersa y
es más fácil romperla accidentalmente.

---

## ITEM_VACIO = 0 en lugar de std::optional

Los slots vacíos se representan con `ItemId = 0` como convención en lugar de
`std::optional<ItemId>`.

**Por qué:** el requisito de persistencia binaria con structs de tamaño fijo
hace que `std::optional` complique el `fwrite/fread` directo. Con `ItemId = 0`,
el struct es plano y serializable de forma más directa.

**Alternativa descartada:** `std::optional<ItemId>`. Semánticamente más
correcto en C++ moderno y elimina el magic number, pero requiere lógica
adicional de serialización y complica la garantía de tamaño fijo.

---

## Armadura, Casco y Escudo unificados en Defensa

Se unificaron en una clase `Defensa` con `TipoSlotDefensa` en lugar de tres
clases separadas.

**Por qué:** armadura, casco y escudo comparten los mismos atributos esenciales:
defensa mínima y defensa máxima. La diferencia relevante es el slot defensivo
que ocupan.

**Alternativa descartada:** clases `Armadura`, `Casco` y `Escudo` separadas.
Más explícito visualmente, pero no agrega comportamiento distinto y duplica
código.

---

## Criatura y NPCCiudad como entidades separadas

Se decidió no usar una jerarquía común `NPC` base.

**Por qué:** no comparten comportamiento real. `Criatura` tiene vida, aggro,
movimiento, drops y puede morir. `NPCCiudad` es estático, no tiene vida
relevante, no ataca y solo responde comandos.

Una clase base común hubiera sido casi vacía, señal de que la herencia no es la
abstracción correcta.

**Alternativa descartada:** jerarquía `NPC` con subclases. Más clásica en juegos
de rol, pero hubiera obligado a tener atributos irrelevantes en cada subclase o
una clase base sin responsabilidades claras.

---

## EstadoMiembro en Clan en lugar de tres listas

Los miembros pendientes, aceptados y baneados se unifican en `MiembroClan` con
`EstadoMiembro`, en lugar de tres colecciones separadas en `Clan`.

**Por qué:** un jugador transita entre estados. Con tres listas separadas habría
que coordinar inserciones y borrados entre ellas, lo cual es una fuente de bugs.
Con `EstadoMiembro`, la transición es un simple cambio de valor.

**Alternativa descartada:** tres listas `pendientes[]`, `miembros[]` y
`baneados[]` en `Clan`. Más explícito visualmente, pero introduce redundancia y
distribuye la lógica de transición.

---

## Banco como abstraccion global por jugador

`Banco` no pertenece a una sucursal ni a un mapa específico; se modela como
información asociada al jugador.

**Por qué:** el enunciado establece que los depósitos son accesibles desde
cualquier sucursal sin cargo. Asociar el banco a una sucursal hubiera requerido
sincronización entre instancias, complejidad innecesaria para esta regla.

**Alternativa descartada:** `Banco` como entidad del mapa con sincronización
entre sucursales. Más realista, pero agrega complejidad de consistencia sin
beneficio directo para el juego.

---

## ConfiguracionSpawn en el Mapa

Cada `Mapa` define sus propias reglas de spawn mediante `ConfiguracionSpawn`,
en lugar de que cada `Criatura` sepa en qué mapas puede aparecer.

**Por qué:** es más natural que el mapa defina su ecosistema. Agregar una
criatura a un mapa implica modificar la configuración del mapa, sin tocar la
definición de la criatura. Un mapa puede tener múltiples tipos de criaturas con
distintos límites e intervalos.

**Alternativa descartada:** cada `Criatura` tiene una lista de mapas donde puede
aparecer. Acopla la criatura al mapa y complica agregar nuevas zonas.

---

## Sacerdote mas cercano como regla, no como clase

La búsqueda del sacerdote más cercano para resurrección se modela como regla de
negocio y cálculo de distancia, no como una clase propia del dominio.

**Por qué:** el sacerdote más cercano no tiene identidad propia dentro de esta
regla. Es un cálculo puntual que el servidor resuelve en el momento usando las
posiciones del mundo.

**Alternativa descartada:** clase `RegistroSacerdotes` en el dominio. Sería un
detalle de implementación del servidor, no una entidad central del negocio.

---

## Bonificacion de clan y Vestimenta como reglas derivadas

La bonificación de ataque/defensa por proximidad de miembros del clan se
documenta como una regla derivada de las posiciones de los miembros. La
vestimenta se resuelve a partir del equipamiento.

**Por qué:** ninguna de las dos tiene estado propio persistente. Son cálculos
derivados de información ya existente: posiciones, clan y equipamiento.

**Alternativa descartada:** crear clases específicas para bonificación de clan o
vestimenta. Agrega tipos sin estado propio y puede terminar en sobrediseño.

---

## Structs de tamaño fijo para persistencia

Las entidades persistibles deben tender a representaciones de tamaño fijo para
permitir escritura y lectura binaria con acceso por offset.

**Por qué:** el enunciado propone dos archivos binarios: uno con los datos de
jugadores y otro como índice `nombre → offset`. Para ese esquema, los registros
de tamaño fijo simplifican el acceso directo.

**Alternativa descartada:** serialización con tamaño variable. Más flexible,
pero agrega complejidad de serialización y rompe el acceso directo simple por
offset.

Todo puede ser consultado aca: [Modelo de Dominio - Argentum Online](https://drive.google.com/file/d/1F0uCqrps5EBFfTYDl82vN3m1dYFfohUL/view?usp=sharing)

---

## ConfigJuego como struct de solo datos

`ConfigJuego` concentra los parámetros numéricos del juego: factores de vida,
maná, experiencia, oro, combate, recuperación, clanes, cheats y duración del
tick del gameloop.

**Por qué:** separa qué datos existen de cómo se cargan. Ninguna clase del
dominio debería depender directamente de archivos ni de TOML. Si el formato del
archivo cambia, solo cambia el lector; el resto del código no se toca.

**Alternativa descartada:** que cada clase del dominio lea sus propios valores
del archivo. Genera acoplamiento entre lógica de negocio e I/O, dificulta el
testing y obliga a abrir el archivo varias veces.

---

## ConfigCompleta para cargar juego e items una sola vez

`LectorConfigToml::cargar(ruta)` devuelve una estructura `ConfigCompleta`, que
contiene `ConfigJuego` y `CatalogoItems`.

**Por qué:** el archivo TOML se parsea una sola vez y desde esa misma estructura
se construyen tanto los parámetros del juego como el catálogo de items. Esto
evita duplicar lecturas y mantiene centralizada la carga de configuración.

**Alternativa descartada:** que `ConfigJuego` y `CatalogoItems` tengan cada uno
su propio método `cargar(ruta)`. Esa opción es simple inicialmente, pero puede
abrir y parsear dos veces el mismo archivo y dispersa la responsabilidad de
carga.

---

## ILectorConfiguracion como interfaz de carga

La carga de configuración puede abstraerse detrás de `ILectorConfiguracion`,
con un método `cargar(ruta) → ConfigCompleta`. `LectorConfigToml` es la
implementación concreta para el formato TOML.

**Por qué:** permite cambiar la fuente de configuración sin modificar el
dominio ni el gameloop. También facilita inyectar configuraciones de prueba.

**Alternativa descartada:** llamada directa a `LectorConfigToml` desde `Juego`.
Acopla el dominio a un formato de archivo concreto y dificulta reemplazar la
fuente de configuración en tests.

**Nota de consistencia:** si se mantiene `ILectorConfiguracion`, entonces
`LectorConfigToml` debe heredar de esa interfaz y devolver el mismo tipo
de configuración. Si no se usa, conviene eliminar la interfaz para evitar
arquitectura decorativa.

---

## Uso de toml++ en lugar de parser propio

Se decidió utilizar `toml++` para leer la configuración del juego.

**Por qué:** el enunciado exige que los archivos de configuración estén en
formato TOML y aclara que no se debe implementar un parser propio. Usar una
librería externa mantiene el proyecto alineado con esa restricción y evita
errores propios de parseo.

Además, `toml++` permite cargar estructuras TOML reales sin limitar el proyecto
a un subconjunto casero del formato.

**Alternativa descartada:** implementar `ParserTOML` propio. Aunque podría ser
más pequeño para un subconjunto reducido del archivo, incumple la restricción
del enunciado y aumenta el riesgo de incompatibilidades con TOML válido.

---

## CatalogoItems como fuente unica de tipos de item

`CatalogoItems` concentra los tipos de item disponibles y los indexa por ID.

**Por qué:** el inventario de un jugador solo necesita guardar IDs de item, no
copias completas de cada objeto. El catálogo permite resolver esos IDs a
definiciones completas como nombre, tipo, daño, defensa, costo de maná o efecto
de poción.

**Alternativa descartada:** guardar objetos completos dentro del inventario.
Facilita algunas consultas, pero duplica información, complica persistencia y
aumenta el costo de mantener consistencia si cambia la definición de un item.

---

## Gameloop como unico escritor del estado del mundo

El estado del mundo se modifica desde un único hilo: el gameloop.

**Por qué:** evita condiciones de carrera dentro del modelo. Los hilos de red
solo traducen paquetes a comandos y los depositan en colas thread-safe. La lógica
de juego se ejecuta secuencialmente dentro del gameloop.

**Alternativa descartada:** permitir que cada hilo de cliente modifique el
estado directamente. Puede parecer más simple al principio, pero obliga a
sincronizar cada estructura compartida y multiplica el riesgo de deadlocks o
estados intermedios inconsistentes.

---

## MensajeSalida como envoltorio interno

`MensajeSalida` agrega información de destino a un `MensajeServidor`.

**Por qué:** la lógica de juego no debería saber cómo se implementa la red ni
qué cola concreta tiene cada cliente. Solo decide si un mensaje va a un jugador
específico o a todos. El `Gameloop` y `MonitorClientes` se encargan del despacho.

**Alternativa descartada:** que `Juego` escriba directamente en colas de salida.
Acopla el modelo a la infraestructura de red y dificulta testear la lógica de
juego sin servidor real.

---

## Stubs explicitos para funcionalidades dependientes del mapa

Algunas acciones como atacar, tomar objetos, tirar objetos, comprar, vender,
curar, depositar o retirar requieren mapa, NPC cercano, zona segura o posición
del jugador.

**Por qué:** no conviene simular esas validaciones dentro de `Jugador`, porque
pertenecen al mundo. El modelo debe esperar a que exista `Mapa` para poder
validar proximidad, colisiones, zonas seguras y objetos en el suelo.

**Alternativa descartada:** implementar esas acciones sin mapa. Permitiría
avanzar más rápido, pero crearía lógica falsa que después habría que reescribir.

**Nota de consistencia:** mientras esas acciones estén pendientes, deben devolver
errores explícitos en lugar de una lista vacía silenciosa.
