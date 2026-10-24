# La ventana Guild (G) clásica de Season 3 en el tema 97k

## Objetivo

Que la ventana de Guild (tecla G) del tema `97k` se vea como la del cliente **MU Season 3**
(`Season 3/Client/main.exe`, interfaz clásica previa a NewUI), en lugar de heredar la de Sven.
El spec del tema (`2026-10-23-interface97-theme-design.md`) la había dejado fuera a propósito;
este trabajo llena ese hueco.

La 0.97k tenía una ventana Guild mínima (`RenderGuildList`: nombre, aviso y lista). La de Season 3
usa el mismo arte de panel pero ya trae las tres pestañas de Sven (Guild, Miembros, Alianza), así
que su comportamiento coincide con `CGuildInfoWindow` y sólo cambia la presentación.

## De dónde salen las medidas

Análisis en Ghidra 12.1.4 de `main.exe` (3 716 608 bytes, x86 PE). Funciones relevantes:

| Función | Qué hace |
|---|---|
| `FUN_004534f6` | Constructor de la ventana: posición 450,0, tamaño 190x380; crea los 6 botones (50x18). |
| `FUN_00455777` | Render principal: panel, título, nombre, salida, pestañas; despacha la página. |
| `FUN_00453f2c` | Página Guild: marca, cifras, aviso y botón de salida del guild. |
| `FUN_004547ea` | Página Miembros: lista y los tres botones del maestro. |
| `FUN_00454d98` | Página Alianza: explicación o lista de alianzas y sus botones. |
| `FUN_0042aa41` | Caja de grupo: tiras de `Trading_line` (horizontal) e `Item_Back01` (vertical), 2 de grosor. |
| `FUN_004170a0` | Botón: `win_button` estirado; 80% normal, 100% con el puntero, 60% pulsado. |
| `FUN_00426176`, `FUN_0042569e`, `FUN_004271ff` | Listas de miembros, aviso y alianza (filas de 13). |
| `FUN_0042648d`, `FUN_004258f6`, `FUN_004274d7` | Fondo 40% negro y scrollbar `win_scrollbar` de cada lista. |
| `FUN_00426b93` | Fila de miembro: banda por cargo, nombre, cargo, servidor. |
| `FUN_0063a540`, `FUN_0063c177` | Carga de texturas (ID → archivo): `0x104/0x105` Item_Back01/02, `0x117` Trading_line, `0x118` exit_01, `0x745` win_button, `0x748` win_scrollbar. |

Las texturas que usa ya estaban en Sven y en `Interface97` (idénticas byte a byte), salvo
`win_button.OZT` y `win_scrollbar.OZJ`, que se copiaron a `Interface97` para que el tema siga
siendo autocontenido.

## Layout (unidades de referencia dentro del panel de 190 de ancho)

| Pieza | Season 3 |
|---|---|
| Panel | `Item_Back01` (190x256) sobre `Item_Back02` (190x177): el marco del tema. |
| Título | 35,12, 120 de ancho, centrado, negrita, (220,220,220). |
| Nombre del guild | `"%s ( Score:%d )"`, 35,53, 120 de ancho, centrado, negrita, (200,255,100). |
| Sin guild | Tres líneas en 20,50 / 20,65 / 20,80, (230,230,230). |
| Pestañas | `win_button` (recorte 48x15), 52 de ancho desde x 15, 69 y 123. Abierta: 73, alto 18, 100%. Cerradas: 75, alto 16, 60%, con una línea de 1 (146,144,141) debajo. Etiqueta en negrita blanca a 79 (78 la abierta). |
| Salida | `exit_01` 24x24 en 25,395: el marco del tema. |
| Guild: marca | Caja 69,104 53x53; la marca 48x48 en 72,107. |
| Guild: cifras | Caja 25,170 140x62; líneas en x 30, y 178/191/204/217, negrita blanca. |
| Guild: aviso | Etiqueta 30,250; caja 25,246 140x95; lista 136x80 en 28,260. |
| Guild: botón | Salir / disolver, 70,362. |
| Miembros | Lista 157x235 en 16,125 (18 filas de 13); títulos 12 por encima en x 30, 81, 122. Nombre a 8, cargo centrado en 70 desde 53, servidor a 127 en (255,196,0). Botones en 18/70/122, 365. |
| Cargos | Banda al 50%: maestro (255,100,50), asistente (255,150,80), battle master (255,200,100). La fila seleccionada va bajo blanco al 50% con texto negro. |
| Alianza | Lista 160x80 en 15,110; títulos en 30,98 y 128,98. Botones en 70,220 y 70,350. Sin alianza: explicación en x 25 desde 106; desde la sexta línea, sobre banda (204,0,0). |
| Listas | Fondo negro al 40%, una unidad más ancho por lado. Flechas 13x13 de `win_scrollbar`, rieles (106,97,88) y pulgar de 11 de ancho. Texto (230,220,200). |

## Las preguntas de la ventana

Season 3 no usaba la caja 0.97k `Message_box` para las preguntas del guild sino su popup
(`FUN_004650ae` lo crea, `FUN_00465b86` lo dibuja, `FUN_0045d047` dibuja la caja):

| Pieza | Season 3 |
|---|---|
| Caja | 213 de ancho en x 214, al 80%: `Message_box2` (5 de alto) arriba y abajo, `Message_box1` en filas de 40 (la última recortada). Centrada en y 130 (no más arriba de 40). |
| Popup genérico | Líneas blancas centradas desde 20, 15 de separación; un campo de texto en `Item_Money`; 9 más abajo los botones `win_button` 50x18 (OK en 46 y Cancel en 116 de la caja, uno solo en 81), 16 sobre el borde inferior. Alto = 15 por línea + 63. |
| Nombramiento (`FUN_004532a4`) | La caja de 150 en y 55. Las dos opciones como cajas de grupo de 170x20 en 20,19 y 20,39, la elegida rellena (146,144,141,200); la pregunta en 75 y 93; OK y Cerrar en 55,117 y 110,117. |

En el tema 97k: la caja de nombramiento es el `MessageBoxView` de tipo `guild-appoint`, y los botones
de opción marcan cuál está elegida (`selected`). Las demás preguntas de `CGuildInfoWindow`
(disolver, expulsar, quitar cargo, alianzas, la contraseña) llevan `GenericDialogConfig::kind =
"guild"`, que el tema dibuja como el popup; `legacy` y `modern` no lo leen. Un OK o Cancel de
serie lleva la palabra traducida como etiqueta, para un tema que rotula el botón él mismo.

## Cambios

- `themes/97k/guild_info.rcss`: la presentación completa (tema 97k).
- `guild_info.rml` (compartido): `data-class-current` en las áreas y etiquetas de pestaña, y
  `master`/`assistant`/`battle-master` en la fila de miembro. `legacy` y `modern` no las usan y
  se ven igual que antes.
- `GuildInfoRmlModel.h` / `GuildInfoWindow.cpp`: la fila de miembro expone qué cargo tiene
  (`master`, `assistant`, `battle_master`); antes sólo decía si tenía alguno (`officer`). No hay
  condición sobre el nombre del tema (§30).

## Diferencias conscientes

- Las coordenadas son de referencia dentro del panel acoplado: escala de UI, workspace y arrastre
  siguen funcionando como en el resto del tema.
- Las listas usan el scroll de RmlUi con el arte clásico; la barra mide 14 unidades.
- El botón pulsado no se desplaza un píxel ni cambia su recorte de tamaño; sólo se oscurece y su
  etiqueta baja uno.
- Sven oculta los botones que no aplican; Season 3 los mostraba deshabilitados en rojo.
