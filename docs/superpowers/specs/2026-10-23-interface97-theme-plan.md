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
| Party | `party_info.rcss`, `party_list.rcss` | marco, `Bar` para HP de miembros |
| Comercio | `trade.rml/.rcss` | `Trading_line`, `Accept_box01/02`, `Money_01/02` |
| Tienda NPC | `npc_shop.rml/.rcss` | `Buy_01/02`, `Sell_01/02`, `Repair_01/02` |
| Baúl | `storage.rml/.rcss` | `Lock_01..04`, `Money_01/02` |
| Chaos Mix | `mix_inventory.rml/.rcss` | `Mix_button1/2` |
| Golden Archer | `gold_bowman*.rcss` | marco, `Ok/Cancel` |
| Entrada a eventos | `devil_square_enter.rcss`, `blood_castle_enter.rcss`, `event_entry.rcss` | marco, `Ok/Cancel` |
| Mascota / ayudante | `pet_info.rcss`, `hud_status.rcss` | `Bar`, `Back1..3` |

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
2. Opcional: dejar `97k` como tema por defecto de este fork (`config.ini`
   `[UI] RmlTheme=97k` en el que se distribuye); el fallback a `legacy` ya existe.
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

## Decisiones abiertas

1. **Nombre del tema**: `97k` (propuesto). Alternativas: `classic97`, `season0`.
2. **Botón Guild del HUD**: el plan lo incluye con arte 97k abriendo la Guild de
   Sven. Si se prefiere no tocar nada de Guild, se oculta y queda la tecla G.
3. **Login/selección**: incluido como Fase 6; si "todas" se refería sólo al juego,
   se puede omitir sin afectar el resto.
4. **Tema por defecto**: dejar `legacy` y que el jugador elija, o distribuir con
   `97k` seleccionado.
