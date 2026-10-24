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
| Cajas de mensaje y menú ESC | `window_shell`, `generic_menu_dialog`, `generic_confirm_dialog`, `message_box_view` | **Hecho** en el juego (caja `Message_box2`/`Message_box1`, `Ok`/`Cancel`, botón `Message_box`, menú ESC 0.97k). Faltan las de la escena de login (`msg_win`, `sys_menu`, `reconnect_dialog`): Fase 6. |
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
| Login, servidores, personajes, carga | `login*`, `server_select`, `char_sel_main`, `char_make`, `loading`, `title_scene` | Fase 6. |
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

- `login.rcss`, `login_main.rcss`, `login_scene.rcss`: caja `0account_new`, campos
  `0text_box`, botones `0on_botton*`, `0new_account01/02`.
- `server_select.rcss`: lista con `Level_box`/`Level01` como en la 97k.
- `char_sel_main.rcss`: paneles `Interface01..04`, `Delete01/02`, `Ok01/02`.
- `char_make.rcss`: `New_character*`, `New_character_ok/cancel`.
- `loading.rcss` / `title_scene.rcss`: `Progress_Back` + `Progress`; las imágenes
  `Local/Loading0N` de la 97k sólo si se decide traerlas (son arte de fondo, no UI).

La escena 3D de fondo sigue siendo la de Sven.

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
