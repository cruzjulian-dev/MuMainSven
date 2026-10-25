# Interface97: plan de implementación

Spec: [`2026-10-23-interface97-theme-design.md`](2026-10-23-interface97-theme-design.md).

Cada fase es un PR chico, se puede probar sola con `$theme 97k` y deja `legacy` y
`modern` sin cambios visuales. Orden pensado para que lo más visible (HUD y paneles)
llegue primero y lo que comparte marco se haga una sola vez.

## Verificación común (al cerrar cada fase)

Desde la raíz del repo:

```powershell
python tools/check_rml_rcss_syntax.py
python tools/check_rml_rcss_drift.py
python tools/check_rml_bound_geometry.py
python tools/check_layout_transform_users.py
python tools/rml_preview.py <documento> --theme 97k --png   # captura para el PR
```

En el cliente:

- `$theme 97k` → abrir/cerrar cada ventana de la fase; `$theme legacy` y
  `$theme modern` → sin cambios.
- Resoluciones 800x600, 1280x720, 1920x1080, 3440x1440; escala UI 75 % y 150 %.
- Cambiar de tema con la ventana abierta; arrastrar las que se arrastran.
- `MuError.log` sin `[RmlTheme]` ni tokens faltantes. Una imagen ausente no deja
  log: revisar a ojo cada sprite de la fase.

---

## Fase 0 — Assets y esqueleto del tema

Sin cambios visuales todavía: el tema `97k` existe y se ve igual que `legacy`.

1. Crear `src/bin/Data/Interface97/` y copiar los 96 archivos de
   `Mu-97k-Client-Source/bin/Client/Data/Interface/`, extensiones en mayúsculas
   (`.OZJ`, `.OZT`).
2. Crear `src/bin/Data/Interface97/Logo/` con las texturas 2D de login/personaje
   listadas en el spec (no `.bmd`, no cielos).
3. Agregar `src/bin/Data/Interface97/README.md`: origen (repo y MD5 del `main.exe`
   0.97k), qué se copió y por qué la carpeta es autocontenida.
4. Crear `src/bin/Data/Interface/RmlUi/themes/97k/`:
   - `theme.ini`: `[Theme] Base=legacy`, `[Meta] DisplayName=MU 0.97k`.
   - `tokens.ini`: sólo los tokens que cambian respecto de legacy (texto de aviso,
     fondo de tooltip, color de cabeceras), con comentario de la constante 97k de
     origen.
   - `README.md`: qué ventanas cubre, cuáles heredan de legacy (Guild y todo lo
     posterior a la 0.97k) y cómo elegir el tema.
5. Verificar que el staging (`cmake/CopyRuntimeAssets.cmake`) copie
   `Data/Interface97` (copia todo `Data/`; confirmar en un build limpio).
6. Script de chequeo `tools/check_theme_assets.py` (o extensión del de sintaxis):
   cada `src:` de un tema debe existir en disco con su extensión OZJ/OZT. Agregarlo
   a CTest junto a `rml_rcss_syntax`. Justificación: las imágenes faltantes fallan
   en silencio.

**Listo cuando:** `$theme 97k` funciona, aparece en Opciones → UI → Tema, el
chequeo de assets pasa.

## Fase 1 — HUD inferior

1. **C++** (`src/source/UI/HUD/MainFrameWindow.{h,cpp}`):
   - Campos de modelo `party_open`, `guild_open`, `party_tooltip`, `guild_tooltip`.
   - Callbacks `mainframe_party_click` y `mainframe_guild_click`, que abren/cierran
     las ventanas existentes de party y guild por el mismo camino que las teclas
     P y G (reutilizar, no duplicar).
2. `main_frame.rml` compartido: agregar `#btn_party` y `#btn_guild` dentro de
   `#buttons`; ocultarlos en `themes/legacy/main_frame.rcss` y
   `themes/modern/main_frame.rcss` (`display: none`) con un comentario.
3. `themes/97k/main_frame.rcss` (copia de legacy como punto de partida):
   - Arte `Menu01_new`/`Menu02`/`Menu03_new` en `#frame_left/middle/right`
     (256/128/256 x 48) y `Menu04` en dos piezas laterales (una espejada con
     `transform: scaleX(-1)`).
   - Orbes HP/MP 53x48 con `Menu_Red`/`Menu_Green` (envenenado)/`Menu_Blue`,
     relleno desde abajo con el mismo `cover left bottom` de legacy.
   - `#bar_sd` oculto; `#bar_ag` vertical 15 px con `Menu03_new_AG`.
   - Botones Party/Character/Inventory (24x24) y Guild (52x18): sin arte en reposo
     (es parte de la barra), `Menu_Party/Character/Inventory` y `Guild` en `.open`.
     Ocultar cshop, friend y window.
   - Ocultar `skill_slot_0..4`; reubicar `#current_skill_slot` y `#skill_list`.
   - Ocultar `item_slot_3`; reubicar Q/W/E en x=226/257/288 (relativo a la barra).
   - EXP 198x4 con número a la derecha.
4. Tooltips con los textos de la 97k ya existentes en Sven (`GlobalText` 358–364
   equivalentes) vía `data-hint`.

**Listo cuando:** el HUD 97k se ve como el original a 640x480 de referencia y se
mantiene centrado en ultrawide; los botones abren sus ventanas; legacy/modern
idénticos a antes.

## Fase 2 — Chat

1. `themes/97k/chat_input.rcss`: caja `Dialogue1` + `Dialogue2` (134x29 cada una)
   y botón `Dialogue2_1`, anclada sobre la barra como en la 97k.
2. `themes/97k/chat_log.rcss`: marco `Nis_vframe`/`Nis_hframe`/`Nis_rsframe`,
   scrollbar `Nis_back` + `Nis_bar`, botones `Nis_btn*`.
3. `themes/97k/workspace.rcss` si la ubicación del chat requiere mover su slot.

## Fase 3 — Marco de paneles, Inventario y Personaje

1. Marco común 190x433: `Item_Back01` (190x256) + `Item_Back02` (190x177) y
   separador `Trading_line`, en `themes/97k/docked_panel_frame.rcss` y
   `window_shell.rcss`. Todos los paneles laterales lo heredan.
2. Inventario (`my_inventory.rml/.rcss`, `item_grid.rcss`):
   - Celdas `Item_box` / `InventoryBox2`.
   - Slots de equipo con `Item_*` en las medidas 97k (40x40, 40x60, 20x20 para
     anillos/collar) — la grilla nativa lee la geometría dibujada; probar celdas
     de las esquinas y el drag de ítems.
   - Zen con `Item_Money` (113x18).
   - Botón salir `Exit_01/02`; reparar `Repair_01/02`. Botones de Sven sin
     equivalente (tienda personal, expansión) ocultos con
     `rml-contract-optional-id` si su C++ lo tolera, o con arte 97k si no.
3. Personaje (`character_info.rcss`, fork de `.rml` sólo si hace falta): filas de
   stats con `Level_box`, botones de punto `Level_01/02`, salir `Exit_01/02`.

## Fase 4 — Paneles de NPC y comercio

Cada uno sobre el marco de la Fase 3:

| Ventana | Archivos | Assets |
|---|---|---|
| Party | `party_info.rcss`, `party_list.rcss` | marco, barra de HP 0.97k (colores de `RenderParty()`, sin textura) |
| Comercio | `trade.rcss` | `Trading_line`, `Accept_box01/02`, `Money_01/02`, `Item_Money` |
| Tienda NPC | `npc_shop.rml/.rcss` | `Repair_01/02`, `Item_Money`, `Exit_01/02` |
| Baúl | `storage.rml/.rcss` | `Lock_01..04`, `Money_01/02`, `Item_Money`, `Exit_01/02` |
| Chaos Mix | `mix_inventory.rml/.rcss` | `Mix_button1/2`, `Exit_01/02` |
| Golden Archer | `gold_bowman.rcss` | marco, `Ok/Ok2`, `Exit_01/02` |
| Entrada a eventos | `event_entry.rcss` (Devil Square y Blood Castle) | marco; bandas negras con borde dorado |
| Mascota / ayudante | `item_endurance.rcss` (los recuadros de mascota) | colores de `RenderBar()` |

Hecho así:

- Los grids nativos leen su posición del RCSS (`FollowGridPx`), así que van donde los dibujaba la
  0.97k: comercio en 15,70 y 15,270; tienda y baúl en 15,50; mix en 15,110.
- El panel 0.97k no tiene cierre en la esquina y la tienda, el baúl y el mix de Sven no tenían
  botón de salir: cada uno tiene un fork del `.rml` que sólo agrega `#btn_exit` (`window_close`).
- En el comercio la `Trading_line` baja de 225 a 240 para que entren el aviso y las notas de Sven.
- La barra de vida de la mascota de la 0.97k (`RenderEquipedHelperLife()`) es la que en Sven dibuja
  `CItemEnduranceInfo`, no `pet_info` ni `hud_status`: `pet_info` ya toma el panel 0.97k por el
  marco de la Fase 3 y la barra de macro de `hud_status` ya era la de `RenderBar()`. `Bar` es la
  caja del PIN del baúl, no de la mascota, y `Back1..3` no se usan en el código 0.97k.
- El marco 97k define `docked-bg-top-a/b` (con `Item_Back01`), que piden los `.rcss` de legacy.

## Pendiente después de la Fase 4 (relevamiento)

Lo que todavía se ve con estética Sven o sólo a medias en el tema `97k`, con la función 0.97k de
referencia (`Mu-97k-Client-Source/src/Render/HUD_Pass*.cpp`):

| Interfaz | Documento(s) Sven | Qué falta, según la 0.97k |
|---|---|---|
| Guild (G) | `guild_info.rcss` | Queda con el contenido de Sven por ahora (decisión del usuario). |
| Creación de guild | `guild_make.rcss` | **Hecho**: editor 0.97k, `Ok`/`Cancel` en 20,350 y 100,350. |
| Cajas de mensaje y menú ESC | `window_shell`, `generic_menu_dialog`, `generic_confirm_dialog`, `message_box_view` | **Hecho** en el juego (caja `Message_box2`/`Message_box1`, `Ok`/`Cancel`, botón `Message_box`, menú ESC 0.97k). En la escena de login y de personajes también: `msg_win`, `reconnect_dialog` y `sys_menu` (el menú ESC 0.97k, sin caja). |
| División de servidor | — | Sven no la implementa (sólo marca `g_bServerDivisionEnable`): no hay ventana que tematizar. Construirla sería una ventana nueva. |
| Avisos | `notices`, `slide_notice`, `server_msg` | `RenderNotices()`: 6 líneas centradas desde y=300, paso 13, fondo negro semitransparente, parpadeo. |
| Texto de sistema | `system_log` | Texto de sistema de la 0.97k. |
| Durabilidad | `item_endurance` | **Hecho**: líneas de texto de `RenderBrokenItem()`, sin íconos. |
| Tooltips | `tooltip` | **Hecho**: la caja de legacy ya era la 0.97k; el texto de los botones la toma también. `item_help` sin revisar. |
| Marcador de guerra de guild / soccer | `battle_soccer_score` | **Hecho**: banner arriba al centro. Falta el reloj del soccer (Sven no lo tiene en el modelo) y `match_status`. |
| Nombres y vida sobre personajes | `world_labels` (C++) | **Hecho** lo que difiere con certeza (capacidad `ClassicWorldHud`): el "HP" del party en caja, sin barra bajo el monstruo elegido. Los nombres y globos quedan como Sven hasta tener capturas de la 0.97k. |
| Contador de flechas del elfo | `item_endurance` (`#arrows`) | **Hecho**: línea de texto en la columna de durabilidad. |
| Texto de carga de mapa | `map_name` | **Hecho**: oculto. |
| Minimapa, buffs | `mini_map`, `buff_strip` | No existen en la 0.97k: decidir si se ocultan en el tema. |
| Login, servidores, personajes, carga | `login*`, `server_select`, `char_sel_main`, `char_make`, `loading`, `title_scene` | Fase 6. **Hecho**: escena de login (`ClassicLoginScene=1`), `login`, `login_main`, `server_select`, `char_sel_main` (+ `char_info_balloon`), escena de personajes (`ClassicCharacterScene=1`) y `char_make` (`ClassicCharacterCreation=1`). Falta la carga. |
| Ventanas que la 0.97k no tenía | MU Helper, Gens, quests, Master Level, In-Game Shop, amigos, castillo, eventos posteriores | Fase 8 opcional: marco y botones 0.97k genéricos. |

## Fase 5 — Carteles, avisos y diálogos

1. Cajas de mensaje: `Message_box` (tapa), `Message_box1` (cuerpo, `repeat-y`),
   `Message_box2` (base), 213 de ancho; botones `Ok/Ok2`, `Cancel/Cancel2` 70x21.
   Aplicar a `generic_confirm_dialog`, `generic_menu_dialog`, `msg_win`,
   `message_box_view`, `reconnect_dialog` y `sys_menu` (menú ESC).
2. `notices.rcss`: 6 líneas centradas, paso 13, fondo negro semitransparente,
   color de aviso 97k y parpadeo (animación RCSS, no C++). `slide_notice.rcss` y
   `server_msg.rcss` con la misma estética.
3. `system_log.rcss`, `item_endurance.rcss` (avisos de durabilidad),
   `battle_soccer_score.rcss` / `match_status.rcss` (marcadores), `map_name.rcss`
   si la 97k tiene equivalente.
4. `tooltip.rcss` / `item_help.rcss`: fondo y borde de `RenderTipText`. Los colores
   de rareza de ítem quedan como literales (son contenido, no tema).

## Fase 6 — Login, selección de servidor y personajes, carga

Con `Interface97/Logo/`:

- Escena de login (**hecho**): capacidad `ClassicLoginScene=1` (`Scenes::ClassicLogin`). Sin mapa
  (`WorldActive = -1`, sin terreno ni sus luces), como la 0.97k: cielo, tres barcos con su estela,
  el logo MU 3D (`Logo03`, escala 0.6) y el sol de `Data/Logo` y `Object1/Ship01`, más tres
  personajes en cubierta (caballero, mago, elfa con su ángel) con sus gestos al azar. La escena
  está corrida a (13600, 13600) del terreno porque `CreateObject()` no acepta posiciones
  negativas; desde ahí todo menos el sol cae en un mismo bloque de objetos, así que se dibujan en
  el orden en que se crean (cielo y mar antes que las estelas), como en la 0.97k. La escena se
  dibuja como la 0.97k: objetos, personajes y después el hada (sus alas y las estelas no escriben
  profundidad); la cámara recorre `WALK_PATHS` desde ese origen. Vista 3D en la franja 640x320 desde
  y=80 con barras negras, sin niebla, sin música, con `iTitle.wav` una sola vez, al encenderse el
  logo (no en cada tramo de la cámara: el 0.97k en juego no lo repite), y las dos antorchas de cada barco. `login_scene.rcss` no cambia: sin el modo tour el logo
  2D ya no se dibuja y los textos de abajo son los mismos.
  Los personajes solo hacen el gesto que no hicieron último (la 0.97k quedaba en la última pose) y
  no giran la cabeza; el hada de la elfa vuela a 25 fps (`GOBoid`); los avisos del servidor se
  dibujan solo en el mundo (`RenderInfomation`).
  Al volver del mundo: los personajes se visten desde cero (sin opciones excelentes ni ancient que
  queden del jugador que ocupó el slot) y empiezan parados, sin la pose montada que `CreateHero()`
  les da en el login; la escena no mueve ni dibuja las hojas del clima.
- `login.rml/.rcss` (**hecho**): caja `0account_new` en 195,247 (17 más arriba que la 0.97k, para no tapar el copyright), campos de 119 en 100,50 y 100,72,
  OK `0on_botton` en 128,95 y Cancelar `0on_botton2` en 90,180. `#panel` queda sin posición (la que
  le pone `LoginWin.cpp` no aplica). Sin las opciones de recordar, el nombre del servidor ni el
  aviso: la 0.97k no los tenía. Capacidad `LoginAttemptLimit=1`: a la tercera contraseña errónea
  de una conexión el cliente la cierra (`EndConnectionOnWrongPassword()` en `WSclient.cpp`), como
  la cerraba el servidor de la 0.97k (código 8); OpenMU contesta todos los intentos.
  `CheckServerConnection()` muestra "You are disconnected from the server" y su OK cierra el juego.
- `sys_menu.rcss` (**hecho**): el menú ESC de la escena de login y de personajes es el de la 0.97k
  (estado 110 de `RenderErrorMessage()`), igual que en el mundo: botones `Message_box` de 120x22
  centrados en la pantalla de 640x480 dp, desde y=60 y cada 30, sin caja ni oscurecer.
- `login_main.rcss` (**hecho**): barra de menú y créditos oculta; ESC abre el menú.
- `server_select.rml/.rcss` (**hecho**): la lista de `Scene_Login_ServerSelect()` con `0box`
  (no `Level_box`) y la barra de carga de `Level01`; las columnas Non-PVP/PVP salen de
  `CServerGroup::m_bPvPServer` (campo `pvp` del modelo; la 0.97k miraba si el nombre terminaba en
  "2") y la carga cruda de `load_percent`. Las x (287, 367, 512) son las de las capturas de la
  0.97k, 137 a la derecha de las del código decompilado (150, 230, 375). `CServerSelWin::UpdateMouseEvent()` pasa a preguntar a
  RmlUi (`IsPointerOver`) en vez de un rectángulo fijo, que no coincidía con este layout.
- `char_sel_main.rml/.rcss` (**hecho**): marco `Interface01..04` (de `UI_DrawLoginBg`), panel
  `New_character201/202` arriba, `Delete01/02` y `Ok01/02` a los lados; 1rem = un píxel de la
  pantalla 640x480 a la altura de la ventana (`font-size: calc(100vh / 480)` en el body). Sin
  botón de menú (ESC). Delete y OK entran deslizándose desde su lado al elegir un personaje;
  New Character sube fuera de pantalla con todos los slots ocupados. Los paneles arrancan fuera de
  pantalla y entran con `.shown` (`data-class-shown="!..._disabled"`; el modelo arranca y vuelve a
  "deshabilitado" al ocultarse la ventana), así que al entrar no aparece ni se desliza el que no
  corresponde. Cada clic en un personaje (otro o el mismo) manda Delete y OK fuera de pantalla y
  los vuelve a entrar, como la 0.97k (`panels_away`, 280 ms afuera: sus 7 cuadros a 25 por segundo).
- Escena de personajes (**hecho**): capacidad `ClassicCharacterScene=1`
  (`Scenes::ClassicCharacter`). Sin mapa (`WorldActive = -1`, sin terreno, con sus alturas en 0 y
  sin atributos: la zona segura del último mapa ocultaba las monturas), como la 0.97k: el patio
  de piedra de `Logo05.bmd` (`Data/Interface97/Logo/`, con las texturas `scs-*` sin el `-jmk`; el
  `Logo05` de `Data/Logo` queda para los otros temas) en (13600, 13600), como la escena de login.
  Los personajes en la fila diagonal de `ReceiveCharacterList()` (slot n en (100n, 50n − 50), girado
  (n − 1)·15°), tal cual, sobre el piso (z 0) y a su escala del mundo (no la 1.2 de Sven), igual que
  sus monturas (`RenderMount()`); la cámara es el sexto punto del camino de la 0.97k ((200, −800,
  300), pitch −80, yaw −10, FOV 45), puesta por la escena (`DefaultCamera` no la pisa). Sin niebla;
  en vez del aura y la columna de partículas de Sven, el seleccionado lleva el anillo de la 0.97k:
  dos `RenderCircle()` de `Magic_Circle1` (radio 35 al pie, 55 a 150 de alto, blanco abajo y apagado
  arriba) girando en sentidos opuestos, una vuelta en 3,6 s. Se elige un personaje por la caja de
  su modelo (`o->OBB`: de (−60, −60) a (40, 40) desde sus pies), como la 0.97k, pero de 200 de
  alto en vez de sus 120 (hasta la cintura), para poder clickearlo también por la cabeza; no por la
  caja de 144×144×300 que Sven usa en su escena (`BuildCharacterScenePickOBB()`).
- `char_info_balloon.rcss` (**hecho**): nombre en negrita sobre clase y nivel, sin globo, donde los
  escribía la 0.97k: proyectados 190 sobre los pies, la clase desde ese punto hacia abajo y el
  nombre su alto + 3 más arriba en la pantalla de 640x480, con el tamaño de su fuente (12/13/14/15
  px según el ancho de la ventana: 640/800/1024/1280+), que C++ pasa como `font-size` de `#balloons`
  (un tamaño de letra tomado de una variable no se rehace en RmlUi) y `--classic-name-rise`; las
  dos líneas van 3 px más arriba que en la 0.97k.
- `char_make.rml/.rcss` (**hecho**): el panel `New_character01/02` de los estados de creación de
  `Scene_CharSelect` en 214,13; la cara 3D de la clase (74x79 en 71,89, uno más arriba que la 0.97k
  para que el cuello no pase el marco) con la cámara de la 0.97k (a 800 delante, FOV 10, luz 0.8
  con la luz de modelo frontal, `HighLight` apagado) y su animación como la 0.97k: 0.3 frames por
  tick de 25 Hz, un solo paso por tick (`MoveCharacterClient`; Sven además la duplicaba y la
  avanzaba otra vez en `UpdateCreateCharacter`), cada acción entera (sin el corte de 2 frames de
  la escena de personajes de Sven) y al terminar acción 0 ó 1 al azar (`MoveCharacter`, ver
  `IsClassicFaceModel()`); nombre en 71,180, clase en blanco en 71,200 y sus flechas
  `New_character001..004` (presionadas sólo al hacer click); Cancel y OK `New_character_cancel/ok`. El panel baja desde arriba al abrirse; la cara del DL se
  mira desde 20 unidades más abajo (está más baja en su modelo). Con `ClassicCharacterCreation=1`
  las caras son `Data/Logo/Face01..04` y `face05` (DL) en vez de `NewFace0N`, y las flechas pasan
  por DW, DK, Elf, MG y DL (las que el servidor habilite); sin Summoner ni Rage Fighter. Mientras
  está abierto se ocultan los paneles del select y los nombres, y el marco (`charsel_frame.rcss`,
  compartido) se dibuja encima del panel, como en la 0.97k.
- `loading.rcss` / `title_scene.rcss`: `Progress_Back` + `Progress`; las imágenes
  `Local/Loading0N` de la 97k sólo si se decide traerlas (son arte de fondo, no UI).

## Fase 7 — Cierre

1. Documentación de uso: sección "Tema MU 0.97k" en
   `docs/rmlui-ui-system/theming-and-modding.md` (cómo elegirlo, qué hereda de
   legacy, botones del HUD ocultos y su atajo de teclado).
2. Dejar `97k` como tema por defecto de este fork (`[UI] RmlTheme=97k` en el
   `config.ini` que se distribuye y en el valor por defecto de
   `GameConfig::GetRmlTheme()`); el fallback a `legacy` ya existe.
3. Barrido completo de la verificación común con todas las ventanas.
4. **Opcional, fase 8:** aplicar el marco 97k genérico (Fase 3/5) a ventanas
   posteriores a la 0.97k (MU Helper, Gens, quests, Kanturu…) para unificar la
   estética. Guild queda excluida salvo que se pida.

## Archivos tocados (resumen)

| Tipo | Archivos |
|---|---|
| Assets nuevos | `src/bin/Data/Interface97/**` |
| Tema nuevo | `src/bin/Data/Interface/RmlUi/themes/97k/**` |
| RML compartido | `main_frame.rml` (2 botones) |
| Temas existentes | `themes/legacy/main_frame.rcss`, `themes/modern/main_frame.rcss` (ocultar los 2 botones) |
| C++ | `UI/HUD/MainFrameWindow.{h,cpp}` (2 callbacks + 4 campos) |
| Herramientas | chequeo de assets de tema + entrada en CTest |
| Docs | `theming-and-modding.md`, README del tema y de `Interface97` |

## Decisiones tomadas

1. **Nombre del tema**: `97k`.
2. **Botón Guild del HUD**: se incluye con el arte 97k (`Guild`) y abre la ventana
   de Guild de Sven.
3. **Login y selección de personaje**: incluidos (Fase 6), con las texturas 2D de
   la 97k.
4. **Tema por defecto**: `97k`. La Fase 7 deja `[UI] RmlTheme=97k` como valor por
   defecto del fork; si el tema no está disponible, el cliente vuelve a `legacy`.
