# Argentum Online — Decisiones de Diseño del Modelo de Dominio

## Posicion como Value Object

Se modeló `Posicion` como clase separada en lugar de atributos sueltos
`x`, `y`, `mapaId` en cada entidad.

**Por qué:** es reutilizada por `Personaje`, `Criatura`, `NPCCiudad` y
`ObjetoEnSuelo`. Sin value object, un cambio en su representación requeriría
modificar todas las entidades que la usan.

**Alternativa descartada:** atributos sueltos en cada clase. Más simple
inicialmente pero genera duplicación y acopla todas las entidades a la
representación interna de la posición.

---

## Herencia para Items

Se eligió herencia (`Arma`, `Baculo`, `Defensa`, `Pocion` heredan de `Item`)
sobre un único struct con todos los campos posibles.

**Por qué:** cada subtipo tiene atributos genuinamente distintos sin
superposición. `Arma` tiene `esDistancia`, `Baculo` tiene `hechizo` y
`costoMana`, `Pocion` tiene `tipo`. Un struct único tendría campos irrelevantes
según el tipo, lo cual es confuso y desperdicia espacio en la persistencia.

**Alternativa descartada:** un único struct `Item` con todos los campos y un
enum `TipoItem` para discriminar. Más simple para serialización pero viola el
principio de responsabilidad única y genera structs con campos que no aplican
según el tipo.

---

## Separacion de Inventario y Equipamiento

Se separaron en dos clases en lugar de tener todo en `Inventario`.

**Por qué:** tienen responsabilidades distintas. `Inventario` gestiona el
almacenamiento. `Equipamiento` gestiona qué está en uso activo e impone la
invariante `NOT (arma AND baculo equipados simultaneamente)`. Mezclar ambas
responsabilidades dificulta hacer cumplir la invariante y complica el testing.

**Alternativa descartada:** una sola clase `Inventario` con slots especiales
para equipados. Más simple pero la invariante queda dispersa y es más fácil
romperla accidentalmente.

---

## ITEM_VACIO = 0 en lugar de std::optional

Los slots vacíos se representan con `ItemId = 0` como convención en lugar de
`std::optional<ItemId>`.

**Por qué:** el requisito de persistencia binaria con structs de tamaño fijo
hace que `std::optional` complique el `fwrite/fread` directo. Con `ItemId = 0`
el struct es plano y serializable trivialmente.

**Alternativa descartada:** `std::optional<ItemId>`. Semánticamente más
correcto en C++ moderno y elimina el magic number, pero requiere lógica
adicional de serialización y rompe la garantía de tamaño fijo del struct.

---

## Armadura, Casco y Escudo unificados en Defensa

Se unificaron en una clase `Defensa` con `TipoSlotDefensa` en lugar de tres
clases separadas.

**Por qué:** los tres tienen exactamente los mismos atributos (`defMin`,
`defMax`). Tres clases separadas hubieran sido duplicación pura sin ningún
beneficio.

**Alternativa descartada:** clases `Armadura`, `Casco` y `Escudo` separadas.
Más explícito visualmente pero no agrega información ni comportamiento distinto.

---

## Criatura y NPCCiudad como entidades separadas

Se decidió no usar una jerarquía común `NPC` base.

**Por qué:** no comparten comportamiento real. `Criatura` tiene vida, aggro,
movimiento, drops y puede morir. `NPCCiudad` es estático, no tiene vida
relevante, no ataca y solo responde comandos. Una clase base común hubiera
sido casi vacía, señal de que la herencia no es la abstracción correcta.

**Alternativa descartada:** jerarquía `NPC` con subclases. Más clásico en
diseño de juegos de rol pero hubiera obligado a tener atributos irrelevantes
en cada subclase o una clase base vacía que no aporta valor.

---

## EstadoMiembro en Clan en lugar de tres listas

Los miembros pendientes, aceptados y baneados se unifican en `MiembroClan`
con `EstadoMiembro` en lugar de tres colecciones separadas en `Clan`.

**Por qué:** un jugador transita entre estados. Con tres listas separadas
habría que coordinar inserciones y borrados entre ellas, fuente de bugs. Con
`EstadoMiembro` la transición es un simple cambio de valor.

**Alternativa descartada:** tres listas `pendientes[]`, `miembros[]`,
`baneados[]` en `Clan`. Más explícito visualmente pero introduce redundancia
y distribuye la lógica de transición.

---

## Banco como abstraccion global por jugador

`Banco` no pertenece a ninguna sucursal ni mapa — es una entidad indexada
únicamente por `personajeId`.

**Por qué:** el enunciado establece que los depósitos son accesibles desde
cualquier sucursal sin cargo. Asociar el banco a una sucursal hubiera requerido
sincronización entre instancias, complejidad innecesaria.

**Alternativa descartada:** `Banco` como entidad del mapa con sincronización
entre sucursales. Más realista pero agrega complejidad de consistencia sin
ningún beneficio para el juego.

---

## ConfiguracionSpawn en el Mapa

Cada `Mapa` define sus propias reglas de spawn mediante `ConfiguracionSpawn`
en lugar de que cada `Criatura` sepa en qué mapas puede aparecer.

**Por qué:** es más natural que el mapa defina su ecosistema. Agregar una
criatura a un mapa es modificar el TOML de ese mapa, sin tocar la definición
de la criatura. Un mapa puede tener múltiples tipos de criaturas con distintos
límites e intervalos.

**Alternativa descartada:** cada `Criatura` tiene una lista de mapas donde
puede aparecer. Acopla la criatura al mapa y complica agregar nuevas zonas.

---

## Sacerdote mas cercano como regla, no como clase

La búsqueda del sacerdote más cercano para resurrección se modela como regla
de negocio en las ecuaciones del `Personaje`, no como una clase del dominio.

**Por qué:** no tiene identidad propia ni estado persistido. Es un cálculo
puntual (distancia euclídea) que el servidor resuelve en el momento. Crear
una clase para esto hubiera sido sobrediseño.

**Alternativa descartada:** clase `RegistroSacerdotes` en el dominio. Es un
detalle de implementación del servidor, no una entidad del negocio.

---

## Bonificacion de clan y Vestimenta como reglas, no como clases

La bonificación de ataque/defensa por proximidad de miembros del clan se
documenta como ecuación en `EcuacionesClan`. La vestimenta se resuelve
enteramente en el cliente.

**Por qué:** ninguna de las dos tiene estado propio ni se persiste. Son
cálculos derivados de información que ya existe (`Equipamiento` para
vestimenta, posiciones de miembros para bonificación). Crear clases para
ellas hubiera sido sobrediseño.

---

## Structs de tamaño fijo para persistencia

Todas las entidades persistibles se diseñaron con structs de tamaño fijo
para permitir `fwrite/fread` directo con acceso por offset.

**Por qué:** el enunciado requiere explícitamente dos archivos binarios donde
el segundo es un índice de nombre → offset en el primero. Esto es imposible
con structs de tamaño variable (`std::string`, `std::vector`).

**Alternativa descartada:** serialización con tamaño variable. Más flexible
pero agrega complejidad de serialización y rompe el requisito de acceso
directo por offset del enunciado.

Todo puede ser consultado aca: [Modelo de Dominio - Argentum Online](https://drive.google.com/file/d/1F0uCqrps5EBFfTYDl82vN3m1dYFfohUL/view?usp=sharing)
