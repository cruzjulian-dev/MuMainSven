# Interface97: la interfaz de MU 0.97k como tema de RmlUi

## Objetivo

Que MuMainSven pueda verse con la interfaz del cliente **MU Online 0.97k**
(repositorio `Mu-97k-Client-Source`): HUD inferior, chat, ventanas (personaje,
inventario, party, comercio, tienda NPC, baúl, Chaos Mix, Golden Archer,
eventos), cajas de mensaje, carteles y avisos, y las pantallas de login /
selección / creación de personaje.

Fuera de alcance por ahora: **la ventana de Guild (tecla G)**, que sigue con la
interfaz de Sven (`guild_info.rml`, `guild_make.rml`).

Los assets de la 97k van en una carpeta nueva: `src/bin/Data/Interface97/`.

## Cómo implementa interfaces MuMainSven (análisis)

La migración a RmlUi está terminada (`docs/rmlui-ui-system/README.md`). Toda la UI
visible del juego sigue el mismo esquema:

```text
C++ (estado, acciones, ciclo de vida)  →  modelo de datos (RmlModelBinder)
  →  RML (estructura, ids, bindings)  →  RCSS del tema (layout, tamaños, sprites)
  →  assets (OZJ/OZT)
```

Lo que importa para este trabajo:

1. **Un tema es una carpeta**, no código. `themes/<nombre>/` con `theme.ini`,
   `tokens.ini`, sus `.rcss` y, si hace falta, `.rml` propios. Se elige en
   **Opciones → UI → Tema**, en `config.ini` (`[UI] RmlTheme=<nombre>`) o en vivo
   con `$theme <nombre>`. No requiere recompilar
   (`docs/rmlui-ui-system/theming-and-modding.md`).
2. **Herencia**: `theme.ini` con `[Theme] Base=legacy` hace que todo archivo que el
   tema no sobreescriba se tome de `legacy`. Así un tema sólo contiene lo que cambia.
3. **Los sprites se referencian desde el RCSS** con `@spritesheet` /
   `decorator: image(...)`. Una ruta relativa se resuelve contra la ubicación del
   `.rcss`; si cae fuera de la carpeta de temas se usa tal cual. Por lo tanto
   `../../../../Interface97/Menu01_new.jpg` desde `themes/97k/main_frame.rcss`
   carga `Data/Interface97/Menu01_new.OZJ` por el mismo pipeline
   (`RmlUiRenderInterface::LoadTexture` → `ResolveThemeResource` → `CGlobalBitmap::LoadImageExclusive`,
   que cambia `.jpg`/`.tga` por `.OZJ`/`.OZT`). **No hace falta tocar C++ para
   cargar assets desde `Interface97`.**
4. **El contrato** de cada ventana (ids, callbacks, campos del modelo) lo define el
   C++; `tools/check_rml_rcss_drift.py` verifica que cada tema lo cumpla.
5. La política (`architecture-principles.md` §13–§16, §30–§31) pide que una
   apariencia distinta sea **un tema**, no una rama de código ni un toolkit paralelo,
   y prohíbe condicionar C++ al nombre del tema.

### Opciones evaluadas

| Opción | Veredicto |
|---|---|
| Portar el código de render de la 97k (`HUD_Pass*.cpp`, `RenderInventoryWindow`, …) | Descartada. Es código decompilado con coordenadas fijas 640x480 y `glBegin`; reintroduce exactamente el toolkit nativo que la migración eliminó y rompe escalado, workspace, temas y drag. |
| Clases C++ nuevas por ventana (`C97kInventory`, …) | Descartada. Duplica comportamiento ya resuelto (inventario, trade, tienda) y viola §12/§20 (un componente, varias presentaciones). |
| Reemplazar los sprites de `Data/Interface` | Descartada. Pisa la interfaz de Sven, hay nombres que colisionan con arte distinto y no permite volver atrás. |
| **Tema RmlUi `97k` que hereda de `legacy`, con assets en `Data/Interface97`** | **Elegida.** Cero C++ para casi todo, seleccionable y reversible en caliente, la Guild de Sven queda intacta por herencia, y cada ventana nueva de Sven sigue funcionando aunque la 97k no la tenga. |

## Diseño

### Estructura de archivos

```text
src/bin/Data/Interface97/                 ← assets 2D de la 97k (nuevo)
    *.OZJ / *.OZT                         ← los 96 archivos de Data/Interface de la 97k
    Logo/*.OZJ / *.OZT                    ← sólo las texturas 2D de login / personaje
src/bin/Data/Interface/RmlUi/themes/97k/  ← el tema (nuevo)
    theme.ini                             ← Base=legacy, DisplayName=MU 0.97k
    tokens.ini                            ← paleta de texto/fondos de la 97k
    README.md                             ← qué cubre el tema y cómo elegirlo
    <ventana>.rcss                        ← un override por ventana migrada
    <ventana>.rml                         ← sólo si la composición 97k no entra en el RML compartido
```

- El tema se llama `97k` (nombre válido: letras, dígitos, `-`, `_`).
- Todo lo que el tema no sobreescribe —Guild, MU Helper, Gens, Master Level,
  In-Game Shop, quests, eventos posteriores a la 0.97k— se ve como `legacy`.
- `Data/Interface97` es autocontenido: se copian también los 47 archivos que hoy
  son idénticos a los de `Data/Interface`, para que el tema no dependa de que Sven
  conserve esos sprites. Coste: ~1,2 MB.
- Extensiones normalizadas a mayúsculas (`.OZJ`, `.OZT`), como el resto de `Data/`.
  Los nombres de archivo se conservan los de la 97k.

### Inventario de assets (`bin/Client/Data/Interface` de la 97k)

Comparado byte a byte con `src/bin/Data/Interface` de Sven: **47 idénticos, 49
nuevos** (algunos con el mismo nombre conceptual pero arte distinto, p. ej.
`Menu01_new` vs `Menu01`). Las texturas y su uso en la 97k:

| Asset(s) | Slot 97k | Dónde lo dibuja la 97k |
|---|---|---|
| `Menu01_new`, `Menu02`, `Menu03_new` | 0xE6–0xE8 | Barra inferior: (0,432) 256x48, (256,432) 128x48, (384,432) 256x48 — `RenderMainFrameWindow` |
| `Menu04` (TGA) | 0xE9 | Remates laterales sobre la barra: (0,387) 108x45 y espejado en (532,387) |
| `Menu_Red`, `Menu_Green`, `Menu_Blue` | 0xEA, 0xF7, 0xEB | Orbe HP (97,432) 53x48 normal/envenenado; orbe MP (489,432); se vacían desde arriba |
| `Menu03_new_AG` | 0x101 | Medidor AG vertical en x=551, 15 px de ancho |
| `Menu_Party`, `Menu_Character`, `Menu_Inventory` | 0xEC–0xEE | Estado "abierto" de los botones (348/379/410, 452) 24x24 |
| `Guild` (JPG) | 0xFA | Botón Guild del HUD (582,459) 52x18 |
| `Dialogue1`, `Dialogue2`, `Dialogue2_1` | 0xF8, 0xF9, 0xEF | Caja de entrada del chat (186..454, 415) y su botón |
| `Nis_*` (rsframe, vframe, bar, back, btnarrow, btnchat, btnsize, btnalpha, hframe, btnmin/max/end) | 0x500–0x507 | Marco, scroll y botones del historial de chat (`ChatLB_renderBg`) |
| `Item_Back01`, `Item_Back02`, `Trading_line` | 0x104, 0x105, 0x117 | Marco de todos los paneles laterales 190x433 (x=450): personaje, inventario, party, trade, tienda, mix, baúl |
| `Item_cap/upper/lower/gloves/boots/weapon01/weapon02/wing/fairy/necklace/ring` | 0x107–0x114 | Fondos de los slots de equipo (40x40, 40x60, 20x20) — `RenderEquipmentBox` |
| `Item_box`, `InventoryBox2` | 0x115, 0x116 | Celda vacía / ocupada de la grilla |
| `Item_Money`, `Money_01`, `Money_02` | 0x10F–0x111 | Contador de zen 113x18 |
| `Exit_01/02`, `Buy_01/02`, `Sell_01/02`, `Repair_01/02`, `Level_01/02`, `Accept_box01/02`, `Mix_button1/2`, `Lock_01..04` | 0x118–0x129 | Botones de paneles (24x24 / 16x16): salir, comprar, vender, reparar, subir stat, aceptar trade, mezclar, candado de baúl |
| `Level_box`, `Level01` | 0xF5, 0xF6 | Filas de stats de la ventana de personaje; selección de servidor |
| `Message_box`, `Message_box1`, `Message_box2` | 0xF0, 0xFB, 0xFC | Caja de mensaje (213 de ancho: tapa, cuerpo repetible, base) |
| `Ok`, `Ok2`, `Cancel`, `Cancel2` | 0xF1–0xF4 | Botones de diálogo 70x21 normal/hover |
| `Bar`, `Back1..3` | 0xFD–0x100 | Barra de vida de mascota/ayudante y fondos de su panel |
| `Skill` | 0x12A | Atlas de íconos de skill |
| `Progress`, `Progress_Back` | — | Barra de carga |
| `Cursor*`, `Font*`, `Guild.ozt` | — | Idénticos a Sven; se copian pero no requieren trabajo |
| `MiniMap.OZT` | — | Presente pero sin uso en el código 97k; no se usa |

Assets de `Data/Logo` de la 97k que entran (sólo 2D): `0account_new`, `0on_botton*`,
`0text_box`, `0new_account01/02`, `0box`, `Interface01..04`, `New_character*`,
`Delete01/02`, `Ok01/02`. Los `.bmd`, cielos y escenario 3D del login **no** se
traen: la escena 3D de login sigue siendo la de Sven.

### Mapeo 97k → documentos de Sven

| Interfaz 97k (función en el port) | Documento(s) Sven a sobreescribir en `themes/97k/` | Notas |
|---|---|---|
| Barra inferior, orbes, AG, botones, EXP, skill actual, pociones (`RenderMainFrameWindow`, `sub_4BD650`, `RenderExperience`) | `main_frame.rcss` | Ver "HUD" abajo. Única ventana que necesita C++. |
| Entrada de chat (`Render_ChatBox`) | `chat_input.rcss` | Se ancla dentro de la barra, como en la 97k. |
| Historial de chat (`ChatListBox`, `Nis_*`) | `chat_log.rcss` | Marco y scrollbar con `Nis_*`. |
| Personaje — C (`RenderCharacterInfoWindow`) | `character_info.rcss` (+ `.rml` si la grilla de stats no encaja) | Filas con `Level_box`, botones `Level_01/02`. |
| Inventario — I/V (`RenderInventoryWindow`, `RenderEquipmentBox`) | `my_inventory.rml/.rcss`, `item_grid.rcss` | Slots de equipo 97k; botones extra de Sven (tienda personal, expansión) ocultos o con arte 97k. |
| Marco común de paneles (`RenderInventoryInterface`) | `docked_panel_frame.rcss`, `window_shell.rcss` | Se hace una vez y lo heredan todos los paneles laterales. |
| Party — P (`RenderParty`) + barras de HP del party | `party_info.rcss`, `party_list.rcss` | |
| Comercio (`RenderTrade`) | `trade.rml/.rcss` | `Trading_line`, `Accept_box01/02`. |
| Tienda NPC (`RenderShopInterface`) | `npc_shop.rml/.rcss` | `Buy`, `Sell`, `Repair`. |
| Chaos Mix (`RenderChaosMix`) | `mix_inventory.rml/.rcss` | `Mix_button1/2`. |
| Baúl (`RenderWarehouse`) | `storage.rml/.rcss` | `Lock_01..04`. `storage_ext` hereda legacy (no existe en 0.97k). |
| Golden Archer (`RenderGoldenArcherWindow`) | `gold_bowman.rcss`, `gold_bowman_lena.rcss` | |
| Ventana de evento (`RenderEventWindow`) | `devil_square_enter.rcss`, `blood_castle_enter.rcss`, `event_entry.rcss` | Las de eventos que la 0.97k no tenía heredan legacy. |
| Cajas de mensaje y menú ESC (`FUN_0051af50`) | `generic_confirm_dialog`, `generic_menu_dialog`, `msg_win`, `message_box_view`, `reconnect_dialog`, `sys_menu` (`.rcss`) | `Message_box*` + `Ok/Cancel`. |
| Avisos del servidor (`RenderNotices`: 6 líneas centradas en y=300, paso 13, fondo negro semitransparente, titila) | `notices.rcss`, `slide_notice.rcss`, `server_msg.rcss` | Carteles/avisos completos de la 97k. |
| Texto de sistema (`FUN_00480980`) | `system_log.rcss` | |
| Avisos de durabilidad (`RenderBrokenItem`) | `item_endurance.rcss` | |
| Vida de mascota/ayudante (`RenderEquipedHelperLife`, `Bar`) | `pet_info.rcss`, `hud_status.rcss` | |
| Tooltips (`RenderTipText`: fondo negro, borde) | `tooltip.rcss`, `item_help.rcss` | Los colores de rareza de ítem siguen siendo literales de contenido (ver theming-and-modding). |
| Marcador de guerra de guild / soccer (`Render_CharInfoPanel`) | `battle_soccer_score.rcss`, `match_status.rcss` | Es un cartel del HUD, no la ventana G. |
| Login, selección de servidor, selección y creación de personaje, carga (`Scene_Login*`, `Scene_CharSelect`, `Scene_Loading`) | `login*.rcss`, `server_select.rcss`, `char_sel_main.rcss`, `char_make.rcss`, `loading.rcss`, `title_scene.rcss` | Texturas de `Interface97/Logo`. |
| **Guild — G** (`RenderGuildList`, `RenderGuildCreation`) | **ninguno** | Se hereda `guild_info` / `guild_make` de legacy = Sven. |

### HUD (única pieza con cambio de C++)

`main_frame.rml` es compartido y cada tema muestra/oculta sus partes
(`.hud-part`). El HUD de la 97k difiere en contenido, no sólo en apariencia:

| 97k | Sven hoy | Resolución |
|---|---|---|
| Orbes HP/MP 53x48 en los extremos del centro | Gauges HP/MP verticales 45x39 | Sólo RCSS: mover `#gauges_left/right`, sprites `Menu_Red/Green/Blue`. |
| Sin SD | `#bar_sd` | `display: none` en el tema. |
| AG vertical en x=551 | `#bar_ag` | Sólo RCSS. |
| Botones Party, Character, Inventory, Guild | cshop, chainfo, myinven, friend, window | **Agregar `btn_party` y `btn_guild`** al RML compartido (ocultos en `legacy` y `modern`) con callbacks `mainframe_party_click` / `mainframe_guild_click` y campos `party_open` / `guild_open` en `MainFrameWindow`. cshop/friend/window se ocultan en `97k` (siguen accesibles por teclado). El botón Guild del HUD usa el arte 97k pero abre la ventana de Guild de Sven. |
| Sin fila de skills con atajo; sólo skill actual (304,444) + grilla emergente | 5 slots + skill actual + grilla | Ocultar `skill_slot_0..4`; reubicar `#current_skill_slot` y `#skill_list`. |
| Tres contadores de pociones (x=226/257/288) | Q/W/E/R | Ocultar el slot R (`item_slot_3`), reubicar Q/W/E. |
| EXP de 198x4 en (221,439) con número | `#exp` a lo ancho | Sólo RCSS. |
| Entrada de chat dentro de la barra | Documento `chat_input` aparte | Sólo RCSS/workspace del tema. |

Los botones nuevos siguen la política: C++ define la acción y el estado ("abrir
party"), el tema decide si se muestran. No hay ninguna condición sobre el nombre
del tema (§30).

### Layout y escalado

- La 97k es 640x480 fijo. La identidad visual se conserva, pero **no** sus
  coordenadas absolutas (§3, §31): los paneles laterales van en los slots del
  workspace (`dock-right`), el HUD centrado abajo con ancho lógico 640 en `dp`,
  igual que `legacy` hoy. Escala de UI, ultrawide y drag siguen funcionando.
- Medidas de la 97k → `dp`/`px` de referencia usando la misma convención que
  `legacy` (x original − 320 para `margin-left`, 480 − borde inferior para `bottom`).
- Texto: `NativeTextSize=1` se hereda de `legacy`.

### Lo que no cambia

- Cursor (archivos idénticos), fuentes, render 3D de ítems (sigue en
  `RenderTarget`), lógica de juego, red y paquetes.
- Los temas `legacy` y `modern` se ven exactamente igual que antes.

## Riesgos

| Riesgo | Mitigación |
|---|---|
| Un `.rml` propio del tema se desincroniza del contrato C++ | Preferir sólo `.rcss`; forks de `.rml` sólo donde la composición lo exija; `check_rml_rcss_drift.py` en cada fase. |
| Imagen faltante o con extensión equivocada falla en silencio | Checklist de assets por fase + script que verifica que cada `src:` del tema exista en disco. |
| Paneles 97k de 433 px de alto vs. 429/432 de Sven | El alto lo fija el RCSS del tema; la grilla nativa lee la geometría dibujada. Probar celdas de las esquinas. |
| Funciones de Sven sin equivalente en la 0.97k quedan con estética Sven | Esperado (herencia). Una fase opcional aplica el marco 97k genérico a esas ventanas. |
| Ocultar botones del HUD quita el acceso por mouse a cshop/amigos/menú | Siguen por teclado; documentarlo en el README del tema. |
