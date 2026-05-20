# Correcciones Pendientes — Alcance Gameloop

---

| Implementado                                                                                                                                             | Archivo / línea                                                                                                                                                                                                                                                        |
| -------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Cheat de **suicidio** server-side (sección 13.3)                                                                                                         | [jugador.cpp:204-206](../server/game/jugador.cpp#L204-L206), [config_juego.h:102](../server/game/config/config_juego.h#L102), [lector_config_toml.cpp:191](../server/game/config/lector_config_toml.cpp#L191), [game_config.toml:300](../config/game_config.toml#L300) |
| Criatura aplica **`fuerza`** al daño (regla 5.1)                                                                                                         | [criatura.cpp:31-38](../server/game/criatura.cpp#L31-L38)                                                                                                                                                                                                              |
| Spawn de criatura valida jugadores en celda                                                                                                              | [juego.cpp:1292-1305](../server/game/juego.cpp#L1292-L1305)                                                                                                                                                                                                            |
| Items no se pueden agregar sobre criatura                                                                                                                | [mapa.cpp:119-127](../server/game/mapa/mapa.cpp#L119-L127)                                                                                                                                                                                                             |
| `estadoEntidadDe` con mapeo explícito (sin asumir orden del enum)                                                                                        | [juego.cpp:14-28](../server/game/juego.cpp#L14-L28)                                                                                                                                                                                                                    |
| Defensa `% 0` en tick de criaturas                                                                                                                       | [juego.cpp:421](../server/game/juego.cpp#L421)                                                                                                                                                                                                                         |
| Cercanía a NPC en `Curar`, `Comprar`, `Vender`, `DepositarItem/Oro`, `RetirarItem/Oro`, `Resucitar`                                                      | [juego.cpp:694-1072](../server/game/juego.cpp#L694-L1072)                                                                                                                                                                                                              |
| `recibir_ataque_fisico` con **esquiva (5.4)** y **defensa combinada (5.5)**                                                                              | [jugador.cpp:91-133](../server/game/jugador.cpp#L91-L133)                                                                                                                                                                                                              |
| Daño de criaturas pasa por esquiva + defensa del jugador                                                                                                 | [juego.cpp:1237](../server/game/juego.cpp#L1237)                                                                                                                                                                                                                       |
| `ejecutarAtacar` PVP: rango adyacente, fair-play newbie (9.1), diff nivel (9.2), fuego amigo de clan (10.5), zona segura (7.1), daño + esquiva + defensa | [juego.cpp:791-896](../server/game/juego.cpp#L791-L896)                                                                                                                                                                                                                |
| Drop de inventario al morir por PVP                                                                                                                      | [juego.cpp:880-894](../server/game/juego.cpp#L880-L894)                                                                                                                                                                                                                |
| Drop de inventario al morir por criatura                                                                                                                 | [juego.cpp:1273-1287](../server/game/juego.cpp#L1273-L1287)                                                                                                                                                                                                            |
| `agregarItemEnSueloCercano` busca celda libre cuando la celda de muerte está ocupada                                                                     | [juego.cpp:254-268](../server/game/juego.cpp#L254-L268)                                                                                                                                                                                                                |
| `Jugador::vaciar_inventario` agregado                                                                                                                    | [jugador.cpp:327-329](../server/game/jugador.cpp#L327-L329)                                                                                                                                                                                                            |

---

## 🔴 Correcciones bloqueantes (rompen reglas del enunciado)

### 1. Criaturas inmortales — sin daño, sin esquiva, sin XP por kill

**Reglas violadas:** 3.3.2, 3.3.3, 4.2, 5.x (lado PVE)

`Criatura` declara `vidaActual`, `vidaMaxima`, `agilidad` y `nivel`
([criatura.h:32-36](../server/game/criatura.h#L32-L36)) pero **falta
`recibir_danio`, `esquivar` y `morir`**. Por ahora las criaturas no se pueden matar
y el flujo `ejecutarAtacar` no las contempla como objetivo.

**Pendiente:**

- Agregar `void recibir_danio(uint16_t)`, `bool esquiva_ataque(const ConfigJuego&)`,
  `bool esta_viva() const`, `uint16_t getVidaMax() const`, `uint8_t getNivel() const`,
  `void morir()` (o equivalente) en `Criatura`.
- Extender `ComandoAtacar` (o `Juego::ejecutarAtacar`) para identificar el
  objetivo como jugador **o** criatura. Hoy [juego.cpp:802](../server/game/juego.cpp#L802)
  solo intenta `buscarJugador(cmd.idObjetivo)`.
- Al matar una criatura, calcular **drop de oro** con
  `Oro = rand(0, cfg.oroDropNpcMax) * vidaMaxima` (regla 4.2) y agregarlo al
  mapa vía `agregarItemEnSueloCercano` (necesita modelar oro como item o
  agregar un endpoint de drop de oro al mapa).

### 2. XP por impacto y por eliminación no se otorga (reglas 3.3.2 y 3.3.3)

`Jugador::ganar_experiencia` existe ([jugador.cpp:209](../server/game/jugador.cpp#L209))
pero **nadie la invoca**. `ejecutarAtacar` aplica daño y dropea inventario,
pero no suma XP al atacante.

**Pendiente:** en [juego.cpp:840 (PVP)](../server/game/juego.cpp#L840) y en el
futuro flujo PVE:

```cpp
atacante->ganar_experiencia(
    danioAplicado * std::max(objetivo->getNivel() - atacante->getNivel() + cfg.expBonusNivel, 0));

if (!objetivo->estaVivo()) {
    float factor = std::uniform_real_distribution<float>(0.0f, cfg.expKillMax)(rng);
    atacante->ganar_experiencia(
        factor * objetivo->getVidaMax() *
        std::max(objetivo->getNivel() - atacante->getNivel() + cfg.expBonusNivel, 0));
}
```

Centralizar esa fórmula en `ReglasJuego` (regla 12.1).

### 3. Oro en exceso se evapora en vez de caer al suelo (regla 4.4)

[jugador.cpp:562-573](../server/game/jugador.cpp#L562-L573):

```cpp
void Jugador::morir() {
    estado = Estado::Fantasma;
    if (!es_newbie()) { perder_experiencia(...); }
    oroExceso = 0;          // ← se pierde silenciosamente
}
```

El enunciado **4.4** exige: _"deja caer al suelo el 100% del oro en exceso"_.

**Pendiente:**

- Agregar `uint32_t quitar_oro_exceso()` en `Jugador` que devuelva el monto y
  setee `oroExceso = 0` (idealmente antes de `morir()`).
- En `Juego::ejecutarAtacar` y `Juego::atacarJugadorConCriatura`, leer el
  oro exceso y dropearlo al mapa. Hoy se modela inventario (ítems) pero no
  oro en el suelo: hay dos opciones:
  - **Opción A**: tratar al oro como un item especial con id reservado.
  - **Opción B**: extender `ItemEnSuelo` con campo `uint32_t monto` opcional
    o agregar `OroEnSuelo` aparte y un opcode `ORO_EN_SUELO` para el cliente.

### 4. `criaturasCerca` declarado y no implementado — error de linker latente

[juego.h:60](../server/game/juego.h#L60) declara
`std::list<uint16_t> criaturasCerca(Posicion);` pero no hay definición.
Si alguien lo llama, falla en linker. Borrar la declaración.

### 5. `ejecutarListar` no valida cercanía a NPC

A diferencia de los otros comandos NPC, [juego.cpp:1053-1058](../server/game/juego.cpp#L1053-L1058)
sigue siendo un stub sin chequeo.

**Pendiente:** mismo patrón que `ejecutarComprar`:

- chequear jugador vivo;
- chequear `mapa.hayNpcCercano(..., TipoNpc::Comerciante, cfg.rangoInteraccionNpc)`
  o `Sacerdote` (porque `/comprar` aplica a ambos según 7.2 y 7.3).

### 6. Resurrección (regla 8.3) no tiene timing por distancia

[juego.cpp:694-707](../server/game/juego.cpp#L694-L707) hoy chequea fantasma
y sacerdote cercano, **pero exige sacerdote adyacente** (mismo radio que el
resto de NPCs). El enunciado **8.3** dice:

> _"Si se ejecuta el comando fuera de la ciudad, el servidor calcula la
> distancia hacia el sacerdote más cercano. El fantasma quedará completamente
> inmovilizado durante un tiempo proporcional a dicha distancia. Transcurrido
> ese lapso, el personaje es teletransportado al lado del sacerdote y revive."_

**Pendiente:**

- Si el fantasma no está adyacente al sacerdote: buscar el sacerdote **más
  cercano del mapa** (sin rango), calcular `segundos = distancia * cfg.factorTiempoResurreccion`.
- Marcar al jugador como inmovilizado por X segundos (nuevo estado o flag).
- En `actualizar(deltaSegundos)` consumir el contador y, al llegar a cero,
  teleportar a la celda libre adyacente al sacerdote y llamar a `resucitar(x,y)`.
- Bloquear `ejecutarMover` mientras el contador esté activo (regla 8.3
  _"completamente inmovilizado"_).

### 7. Notificaciones de clan ausentes (regla 10.5)

El enunciado pide: _"notificará por chat a todos los miembros online del clan
cuando un aliado inicie sesión, cierre sesión o esté recibiendo daño"_.

**Pendiente:**

- En `Juego::conectarJugador` y `desconectarJugador`: si el jugador tiene
  clan, recorrer `jugadoresConectados` filtrando por `getClan() == idClan` y
  emitir un `MENSAJE_CLAN` (o `MENSAJE_CHAT` de sistema) tipo
  `AliadoOnline` / `AliadoOffline`.
- En `ejecutarAtacar` y `atacarJugadorConCriatura`, cuando el objetivo
  recibe daño: notificar a los aliados online del clan.
- Requiere agregar variantes en `TipoMensajeClan` si no existen.

### 8. Bonus de clan por proximidad ausente (regla 10.5)

> _"Si dos o más miembros del mismo clan están en un radio de proximidad
> cercano, reciben un bonus multiplicador en su daño de ataque y en su
> defensa, proporcional a la cantidad de aliados presentes."_

**Pendiente:**

- Definir el radio y la curva del bonus en TOML
  (`[clanes] radio_bonus`, `factor_bonus_por_aliado`).
- Calcular en `ReglasJuego` un `multiplicadorClan(jugadores, jugador, mapa, cfg)`.
- Aplicarlo tanto en `Jugador::calcular_danio` como en `recibir_ataque_fisico`
  (esto requiere pasar el contexto del clan).

### 9. Fuego amigo en daño PVE no contemplado

[juego.cpp:830-833](../server/game/juego.cpp#L830-L833) bloquea PVP entre
miembros del mismo clan. **Bien**. Pero si en el futuro una criatura
pertenece a "facción" o se permite ataques en área, conviene reusar la
misma chequeo. Hoy no es bloqueante, marcar como TODO.

---

## ⚠️ Correcciones medianas (no rompen reglas pero degradan calidad)

### 10. Métodos duplicados de búsqueda de jugador por posición

[juego.cpp:120-128](../server/game/juego.cpp#L120-L128) (`posicionOcupadaPorJugador`),
[juego.cpp:1198-1206](../server/game/juego.cpp#L1198-L1206) (`posicionOcupadaPorAlgunJugador`)
y [juego.cpp:1212-1222](../server/game/juego.cpp#L1212-L1222) (`buscarIdJugadorEn`)
hacen casi lo mismo, ninguno usa el `Posicion::operator==` ya existente
([posicion.h:12-14](../server/game/modelo/posicion.h#L12-L14)).

**Pendiente:** colapsar a un único helper:

```cpp
std::optional<uint16_t> idJugadorEn(const Posicion& p, std::optional<uint16_t> excluir = std::nullopt) const;
```

### 11. `puedeMeditar` y `puedeUsarMagia` son idénticas

[jugador.cpp:437-443](../server/game/jugador.cpp#L437-L443): ambas devuelven
`manaMax > 0`. Decidí cuál querés y eliminá la otra (o diferenciá: por ejemplo,
`puedeMeditar` requiere clase ≠ Guerrero y `puedeUsarMagia` solo `manaMax > 0`).

### 12. `Mapa` es una única instancia para todos los `mapaId`

`Posicion` tiene `mapaId` y los métodos del mapa filtran por él en
`mismaPosicion`, pero `ancho/alto` y `paredes/ciudades/npcs/criaturas` son
globales. Si la entrega contempla varios mapas, hay que refactorizar a
`unordered_map<uint16_t, Mapa>` y propagar `mapaId` en todas las queries.

**Decisión:** o se modela bien (deuda técnica grande) o se elimina `mapaId`
de `Posicion` y se documenta que es escenario único. Hoy es ambigüedad
peligrosa.

### 13. Drop al morir por criatura: oro exceso tampoco se dropea

[juego.cpp:1273-1287](../server/game/juego.cpp#L1273-L1287) dropea **items**
del inventario pero no oro exceso. Mismo bug que (3). Cuando se resuelva (3),
contemplar ambos flujos de muerte (PVP y PVE).

### 14. `Jugador::cfg` se copia entera por jugador

[jugador.h:129](../server/game/jugador.h#L129): cada `Jugador` tiene una
copia completa del struct `ConfigJuego`. Con cientos de jugadores conectados
es desperdicio. Cambiar a `const ConfigJuego&` (cuidando life-time) o
puntero compartido.

### 15. `Mapa::moverCriatura` y `agregarCriatura` lanzan excepciones

[mapa.cpp:230-249](../server/game/mapa/mapa.cpp#L230-L249) y
[mapa.cpp:213-225](../server/game/mapa/mapa.cpp#L213-L225) lanzan
`std::invalid_argument`. Los callers (`Juego::moverCriaturaHacia`,
`Juego::moverCriaturaAleatoriamente`, `Juego::agregarCriatura`) **no las
atrapan**. Si una invariante falla en runtime, **muere todo el tick**.

**Pendiente:** cambiar firmas a `bool` o atrapar la excepción del caller.

### 16. `Criatura::persiguiendoJugador` map es código muerto

[criatura.h:41](../server/game/criatura.h#L41) declarado, expuesto por
`estaPersiguiendo()` ([criatura.cpp:40-42](../server/game/criatura.cpp#L40-L42))
pero **nunca seteado**. Eliminar o implementar el "olvido de objetivo".

### 17. Chat global broadcastea a todos los mapas

[juego.cpp:464-465](../server/game/juego.cpp#L464-L465) usa
`TipoDestino::TODOS`. El enunciado no es explícito si es server-wide o
mapa-wide. **Decidir y documentar**.

### 18. `ejecutarChatPrivado` y `ejecutarGestionMiembroClan` permiten emisor fantasma de borde

`ejecutarChatPrivado` ([juego.cpp:468-480](../server/game/juego.cpp#L468-L480))
exige `emisor->estaVivo()`. **Bien**. Pero el flujo de clan
`ejecutarGestionMiembroClan` exige `lider->estaVivo()` —
¿es razonable que un líder fantasma no pueda aceptar miembros? El enunciado
no obliga, mantenelo si es decisión propia y documentalo.

### 19. CMakeLists.txt mezcla .cpp en sección PUBLIC

[server/CMakeLists.txt:52, 65-66, 69](../server/CMakeLists.txt#L52):
`reglas_juego.cpp`, `mapa.cpp`, `npc.cpp`, `criatura.cpp` están en PUBLIC en
vez de PRIVATE. Compila, pero está mal categorizado.

---

## 🟡 Higiene y limpieza

### 20. Indentación rota en `ejecutarGestionMiembroClan`

[juego.cpp:600-607](../server/game/juego.cpp#L600-L607): la llave `}` del
`if (!idObjetivo)` está alineada al margen izquierdo en vez del nivel
correcto.

### 21. `recuperar` tiene dos bloques que salen de meditación

[jugador.cpp:174-194](../server/game/jugador.cpp#L174-L194): el bloque
[L182-185](../server/game/jugador.cpp#L182-L185) y el bloque
[L190-194](../server/game/jugador.cpp#L190-L194) hacen lo mismo. Unificar.

### 22. `Mapa::cantidadCriaturas`, `obtenerNpcs`, `obtenerNpcsPorTipo`,

`buscarNpcEn` declarados pero sin callers

Si no son API pública futura, eliminar. Si lo son, documentar el por qué.

### 23. `Juego::ejecutarAtacar` tiene indentación de bloque mal

[juego.cpp:862-895](../server/game/juego.cpp#L862-L895): el bloque
`if (!objetivo->estaVivo()) { ... }` cierra sin indentación. Funciona pero
es difícil de leer.

### 24. `Posicion::mapaId` en constructores de `Posicion{x,y,mapaId}`

[juego.cpp:1121, 1124, 1127, 1130, 1140, 1142, 1146, 1148](../server/game/juego.cpp#L1121)
arman `Posicion{...}` con braced-init. Usar `Posicion{x, y, mapaId}` explícito
ayuda a no equivocarse en el orden.

---

## ❌ Reglas del enunciado todavía no implementadas (alcance gameloop)

| §           | Regla                                           | Estado                                                       |
| ----------- | ----------------------------------------------- | ------------------------------------------------------------ |
| 2.2         | Árboles/edificios como obstáculos               | ⚠️ `hayParedEn` funciona, **faltan paredes reales cargadas** |
| 3.3.2-3.3.3 | XP por impacto y kill                           | ❌ ítem 2                                                    |
| 4.2         | Drop oro NPC                                    | ❌ ítem 1                                                    |
| 4.4         | Drop oro exceso PVP/PVE                         | ❌ ítem 3                                                    |
| 5.x         | Ataque jugador → criatura                       | ❌ ítem 1                                                    |
| 7.2         | `/curar`, `/comprar`, `/resucitar` (sacerdote)  | ⚠️ cercanía sí, **lógica del NPC no**                        |
| 7.3         | `/comprar`, `/vender` (comerciante)             | ⚠️ ídem                                                      |
| 7.4         | `/depositar`/`/retirar` (banquero)              | ⚠️ ídem                                                      |
| 8.3         | Resurrección remota con timing                  | ❌ ítem 6                                                    |
| 10.5        | Notif login/logout/daño al clan                 | ❌ ítem 7                                                    |
| 10.5        | Bonus proximidad clan                           | ❌ ítem 8                                                    |
| 11          | Persistencia binaria (datos + índice)           | ❌ no iniciada                                               |
| 14.1        | Cobertura GoogleTest del protocolo y del modelo | ❌ solo `foo.cpp`                                            |

> **Sobre 11 (persistencia)**: el enunciado pide dos archivos binarios
> (`.bin` + índice) con structs de tamaño fijo. El gameloop hoy no tiene
> hook de autosave (sección 11.1 _"de forma periódica y automática"_). En
> `Juego::actualizar` corresponde un contador `ticksDesdeUltimoGuardado`
> que dispare un `Persistor::guardarTodo(jugadores)` cada N segundos.

> **Sobre 14.1**: aunque el protocolo binario no es scope gameloop, **`Juego`,
> `Jugador`, `Clan`, `Mapa` y `ReglasJuego` son perfectamente testeables sin
> sockets**. La sección 14.1 va a evaluarse igual.

---

## 🧪 Plan mínimo de tests recomendado (sección 14.1)

`tests/` solo tiene `foo.cpp`. Casos sugeridos para cubrir el modelo del
gameloop:

**`ReglasJuego`**

- `calcularVidaMaxima` y `calcularManaMaximo` con todas las clases × razas.
- Guerrero da `manaMax == 0`.
- `calcularLimiteExperiencia` para nivel 1, 2, 10.
- `calcularOroSeguro` y `calcularOroMaximoTotal` para varios niveles.
- `esGolpeCritico` deterministico con `valorAleatorio` controlado.
- `calcularPerdidaExperienciaMuerte` para `expPerdidaMuertePct = 0`, `1`, `0.5`.

**`Jugador`**

- Construcción inicial: `vidaActual == vidaMax`, `manaActual == manaMax`.
- `recibir_danio` reduce vida; al llegar a 0 cambia a `Fantasma`.
- `recibir_danio` con `cfg.invulnerable` no hace nada.
- `recuperar(seg)` regenera vida/maná naturalmente.
- `meditar` + `recuperar` regenera más rápido; al llenarse vuelve a `Vivo`.
- `cancelarMeditacion` al moverse.
- `equipar_item` con poción de vida cura y consume.
- `equipar_item` arma desplaza báculo y viceversa (regla 6.2).
- `sumar_oro` respeta `OroMax * (1 + oroExcesoPct)`.
- `morir` setea `oroExceso = 0` y, una vez resuelto el ítem 3, expone el
  oro perdido.
- `ganar_experiencia` sube de nivel al pasar el límite.

**`Clan`**

- Fundación y cantidad inicial = 1 (el fundador).
- `pedirUnirse`, `agregarMiembro`, `eliminarMiembro`, `banearMiembro`.
- `cantidadMiembros` solo cuenta aceptados.
- `obtenerMiembros` excluye al fundador (decisión a documentar).
- `estaBaneado` bloquea re-postulación.

**`Mapa`**

- `posicionValida` y `hayParedEn`.
- `agregarItem` falla si hay pared / NPC / criatura / otro item.
- `tomarItem` retorna el id correcto y elimina del mapa.
- `actualizarItemsEnSuelo` expira correctamente.
- `puedeOcuparCriatura` y `moverCriatura` chequean paredes/NPCs/criaturas.
- `hayNpcCercano` con rango 1 y rango 5.

**`Juego` (integración sin sockets)**

- Conectar / reconectar por nombre.
- Rechazo de conexión con id duplicado.
- Cancelación de meditación al recibir otro comando.
- Movimiento bloqueado por colisión con jugador / NPC.
- `ejecutarAtacar` rechaza newbie, diff > 10, mismo clan, zona segura.
- `ejecutarAtacar` aplica daño con esquiva/defensa, dropea inventario al matar.
- `ejecutarTomar` y `ejecutarTirar` con catálogo válido / inválido.
- `ejecutarDepositarOro` rechaza sin banquero cercano.

---

## 📦 Items no del gameloop pero que conviene tener en cuenta

- **`main.cpp`** ([server/main.cpp](../server/main.cpp)) sigue siendo
  `return foo(6);`. No arranca el gameloop ni la red. **No es scope del
  gameloop**, pero hasta que esto cambie nadie puede probar el sistema end
  to end. Es razonable agregar en algún momento una llamada
  `LectorConfigToml::cargar()` + `Juego juego(...)` que valide al menos
  que la config carga.
- **Documentación del gameloop**: `doc/Gameloop.md` está desactualizada
  (la tabla de miembros nombra `criaturasEnMapa`, el enum incluye
  `Resucitando`, etc.). Pendiente de actualizar — pero por instrucción
  explícita queda **fuera de esta revisión**.

---

## Resumen de prioridad

**Bloqueantes (rompen reglas):** 1, 2, 3, 5, 6, 7.

**Importantes para calidad funcional:** 4, 8, 9, 13, 15.

**Higiene y refactor:** 10, 11, 12, 14, 16-24.

**Fuera de scope inmediato pero exigidos por el enunciado:** persistencia
(11), tests (14.1).
