# MU 0.97k theme

The `97k` theme dresses MuMainSven in the interface of the MU Online 0.97k client. Its art is in
`Data/Interface97/`; this folder holds only the stylesheets and markup that differ from `legacy`.

## Choosing it

- **Options → UI → Theme → MU 0.97k**, saved in `config.ini` as `[UI] RmlTheme=97k`.
- `$theme 97k` in the chat box for the current session only.

## What it covers

The theme inherits `legacy` (`theme.ini`). A window this folder doesn't restyle looks exactly as
in `legacy`:

- Windows the 0.97k client never had (MU Helper, Gens, Master Level, the in-game shop, the NPC
  quests and later events) keep the Sven look; the quest and pet windows are the Season 3
  client's (below).

## Restyled so far

- **The bottom bar** (`main_frame.rcss`): the 0.97k bar with its corner pieces, the HP and MP
  orbs (green while poisoned), the AG column, EXP, the Q/W/E potions, the current skill and its
  list above the bar, and the Party, Character, Inventory and Guild buttons. The 0.97k bar has no
  SD gauge, no skill hotkey row, no fourth potion box and no shop, friend or menu buttons; the
  skills keep working from their keys (the number keys). The build has no fourth potion hotkey,
  friend list, window menu, in-game shop, Gens ranking or server chat command list, so R repairs
  instead of L and F, U, X, B and J do nothing, in every theme (`Core/Globals/ClientProfile.h`).

- **The chat** (`chat_input.rcss`, `chat_log.rml/.rcss`, `chat_frame_buttons.rcss`,
  `workspace.rcss`), with the 0.97k client's two modes (`ChatLogModes=1` in `theme.ini`):
  - **History**: the log on the left without a frame, as in `legacy`.
  - **Box**: the 0.97k frame over the input box, down to the bar, with its scrollbar and resize
    grip. It stays up when typing ends.

  While nobody types, in either mode, moving the pointer over the bar's middle brings up three
  buttons over the closed input box: show or hide the chat (F2), the current mode's height (F4)
  and the box's transparency (from the history it switches to the box first).

  F5 switches the modes and F4 cycles the current one's height (3, 6, 9, 12 or 15 lines), typing
  or not; each mode keeps its own height, so switching brings back the one it had. Lines are 13
  units apart and the box is 40 units per 3 lines plus 10, as in the 0.97k client. The input box,
  shown while typing, draws over the bar (`[Stacking]` in `theme.ini`). Its ear is F3's switch:
  each character starts with whispering off (`WhisperStartsOff=1`), the ear grey and the
  recipient field red; F3 or the ear turns it gold and the field takes the name. There are no
  chat-type buttons: start a message with ~ (party), @ (guild) or $ (Gens).
- **The side panels' frame** (`docked_panel_frame.rcss`): the 0.97k panel (`Item_Back01` over
  `Item_Back02`) and its 24x24 exit for the character, party, quest and pet windows. The panel
  keeps its 429 units; the art reaches 4 more onto the bar, as in the 0.97k client. The corner
  close mark of the Sven frame is gone.
- **The inventory** (`my_inventory.rml/.rcss`, `item_grid.rcss`): the 0.97k equipment boxes at
  their 0.97k places and sizes, the 0.97k grid cells (`Item_box`, `InventoryBox2` under an item,
  no frame), the money box with "Zen" and the 0.97k exit and repair buttons. The 0.97k cells are
  every item grid's. The personal shop button is the Season 3 client's (`btn_pshop`, `btn_pshop2`
  while the shop is open) beside the 0.97k ones, and the expansion takes the same art for now;
  Sven's set and socket option header is hidden (the 0.97k client had no such items).
- **The character window** (`character_info.rml/.rcss`): the name and class on the 0.97k title
  bands, the level and each attribute written in a `Level_box` ("Fuerza: 28"), each derived line
  on its own black band as tall as the text, the 0.97k point buttons, the points to spend on the
  0.97k blue band. Stamina stands one line lower than in the 0.97k client, clear of agility's
  third line. Sven's fruit-probability line is hidden; the quest and pet shortcuts beside the
  exit are the Season 3 client's (`Quest1`, `pet1`). Season 3 had no master skill tree, so the
  theme leaves out its shortcut; the build has no master skill tree either (`ClientProfile.h`).
- **Trade** (`trade.rcss`): the 0.97k panel with `Trading_line` across it, the partner's offer
  above and the hero's below in the 0.97k cells, both amounts in `Item_Money` boxes, and the
  0.97k zen, accept (`Accept_box01`, `Accept_box02` once accepted) and exit buttons on one row,
  every piece at the place measured from the 0.97k client ("Zen" on a half-black band left of each box,
  the notices over each other where the 0.97k ones overlap).
- **The NPC shop** (`npc_shop.rml/.rcss`), **the storage** (`storage.rml/.rcss`) and **the chaos
  mix** (`mix_inventory.rml/.rcss`): the 0.97k panel and cells, their amounts in `Item_Money`
  boxes and the 0.97k buttons (`Repair_01`; `Money_01` and the lock, `Lock_01`/`Lock_03`;
  `Mix_button1`). The 0.97k panel has no corner close: the shop's and the mix's forks of the
  markup add the 0.97k exit, and the storage has none, as in the 0.97k client (the inventory's
  exit closes it); Sven's own buttons (repair all, storage expansion) take 0.97k art or keep theirs
  beside the 0.97k ones.
- **The party** (`party_info.rcss`, `party_list.rcss`): the window lists each member as the 0.97k
  client did (the leader's name on a dark red band, the 0.97k health bar, the kick as the 0.97k
  exit); the list down the right side is the 0.97k one, dark cards with gold sides, its gold caps
  (`party_list.rml`'s fork adds them) and a health bar of ten red steps, without Sven's leave
  button: the party is left from the window.
- **The quest window** (`my_quest_info.rml/.rcss`): the Season 3 client's, the class-change quest
  alone: its name in bold (150, 255, 240) at 35,12, its lines centred on x 100, 18 apart, on the
  seven rows from 66, its state under them in (223, 191, 103), `Trading_line` across at 225.
  Sven's tabs, quest list and castle/temple page have no place in it and are hidden.
- **The pet window** (`pet_info.rml/.rcss`): the Season 3 client's, the Dark Horse and Dark
  Spirit tabs as `win_button` (the open one taller and bright), `Trading_line` at 80 and 380 with a
  thin gold frame between them, the pet's level on a `Level_box`, its life over the S3 red bar and
  each other line 15 apart on a black band. The raven's commands keep Sven's slots under their own
  `Level_box`; Sven's group boxes are hidden.
- **The command window (D)** (`command_window.rcss`): the Season 3 client's, its commands as
  `Message_box` buttons 100x25 from 45,69, 28 apart, the label bold in gold under the pointer or
  while armed, the S3 target box (`CursorID_Wnd`) at the pointer. There is no Special or Add
  Friend button: the build has no chat command list or friend list (`ClientProfile.h`).
- **The guild window (G)** (`guild_info.rcss`): the classic window of the Season 3 client, which
  already had Sven's three tabs (`docs/superpowers/specs/2026-10-24-guild-s3-classic-design.md`):
  the `win_button` tabs, the open one taller and bright; the gold group boxes round the mark, the
  figures and the announcement; the lists on 40% black with the `win_scrollbar` arrows and thumb;
  each office's own band in the member list (master, assistant, battle master); the 50x18
  `win_button` buttons. Its questions (`generic_confirm_dialog.rcss`, kind `guild`) and the
  appointment box (`message_box_view.rcss`, kind `guild-appoint`) are the Season 3 popup: the 0.97k
  box in 40-unit rows, opaque, its lines 15 apart, the 0.97k `Ok` and `Cancel`, the two
  appointments as framed rows with the chosen one filled. The question that expels a member is
  the theme's plain 0.97k box instead.
- **The guild creation** (`guild_make.rcss`): the 0.97k editor (the name in an `Item_Money` box,
  the 8x8 mark grid with no frame, the palette, the selected colour) with the 0.97k `Ok` and
  `Cancel` at 20,350 and 100,350; Sven's question page has the 0.97k `Message_box` button.
- **The message boxes and the ESC menu** (`window_shell.rcss`, `generic_menu_dialog.rcss`,
  `generic_confirm_dialog.rcss`, `message_box_view.rcss`): the 0.97k box (`Message_box2` caps
  over `Message_box1`, 213 wide; a confirm box such as the trade request is the 0.97k
  `Message_box`, 213x64 from y 60, its lines 12 apart), the 0.97k `Ok`/`Cancel` where the 0.97k
  box had them (`Ok2`/`Cancel2` under the pointer) and the 0.97k menu button (`Message_box`) for
  every other button; the ESC menu is the 0.97k one, its
  buttons 120x22 centred on the screen, 30 apart from 60, with no box.
- **The login and character-scene message box** (`msg_win.rcss`), e.g. "You are disconnected from
  the server": the 0.97k `Message_box` (213x64 from y 60) with its white centred lines and the
  0.97k `Ok`/`Cancel` (`Ok2`/`Cancel2` under the pointer) where `RenderErrorMessage()` drew them.
- **The login and character-scene ESC menu** (`sys_menu.rcss`): the same 0.97k ESC menu as in the
  world, its buttons 120x22 centred on the 640x480 screen, 30 apart from 60, with no box and no
  dimming: Exit Game, Option and Close, with Select Server too in the character scene.
- **The reconnect dialog** (`reconnect_dialog.rcss`): the art of the 0.97k connecting box,
  `Message_box` at half brightness (213 wide, as tall as the dialog), the `Progress_back` track
  with the yellow band the 0.97k `Progress` fill showed over it, and the 0.97k `Cancel` (`Cancel2`
  under the pointer).
- **The tooltips** (`tooltip.rml/.rcss`): legacy's box is already the 0.97k one (80% black in a
  1-unit black frame); a button's hover text takes it too, as in the 0.97k client.
- **The guild war and soccer score** (`battle_soccer_score.rcss`, `workspace.rcss`): the 0.97k
  banner at the top centre, no back, two rows of mark, score and name in the 0.97k green and blue.
- **The Golden Archer** (`gold_bowman.rcss`): the 0.97k exit, and Register as the 0.97k dialog's
  Ok. **The Devil Square and Blood Castle entries** (`event_entry.rcss`,
  `devil_square_enter.rcss`, `blood_castle_enter.rcss`): the 0.97k event window, each level a
  `Message_box` button in the 0.97k red, the panes scrolling with the 0.97k chat's scrollbar.
- **The pet and summon life** (`item_endurance.rcss`): in the top right corner, or beside the
  party list while there is one, one under the other (a Dark Lord's raven under the horse), the name on a band the size of the text over `RenderBar()`'s bar. The worn
  items are the 0.97k text lines ("name (durability/max)" in the band's colour) under the pets,
  not icons.
- **The world** (`map_name.rcss`, `item_endurance.rcss`, `theme.ini`'s `ClassicWorldHud`): no map
  title when a map loads; the elf's arrow count as the 0.97k line of text over the worn items; a
  party member's HP hover text in the 0.97k tooltip box and no bar under the selected monster's
  name.
- **The login scene** (`ClassicLoginScene=1` in `theme.ini`): the 0.97k one instead of the login
  world. Three ships with their torches sail under the sky, the sun low on the horizon, with the
  3D MU logo over the water lighting up as the camera starts its slow walk; a knight and a wizard
  stand at the bow of the near ship and an elf with her guardian angel on the right one, each
  gesturing now and then as the 0.97k ones did (one of their two gestures at a time, heads still).
  Server notices show only in the world. The view is the 0.97k strip between black bars, with the version and
  copyright lines on the bottom one. No music, just the title sound once, as the logo lights up.
  The menu and credits buttons are gone: ESC opens the menu. A theme switch shows on the next
  visit to the login scene.
- **The login box** (`login.rml/.rcss`): the 0.97k box (`0Account_new`) under the logo, the
  account and password fields, OK in the art's top right slot and Cancel under the frame, each lit
  under the pointer; 17 higher than the 0.97k box so it clears the copyright line. The 0.97k box had no remember options: they keep their saved state but are
  not shown.
- **The server list** (`server_select.rml/.rcss`): the 0.97k list, its groups as `0box` buttons
  70x16 in two columns from y 220, Non-PVP on the left and PVP on the right (a group goes right
  when it has no Non-PVP channel in `ServerList.json`), the selected one brighter and 10 nearer
  the middle. Its channels stand between the columns, centred on the group, as `0box` rows 134x20
  tinted by the load (white while low, light yellow for a gold channel, red while busy,
  orange-red when full, brighter under the pointer), each with the 0.97k load bar under its name:
  20 dots, one more lit per 5% of the load, from cyan to red. The group's description is under
  the columns; the 0.97k list had no PvP notice.
- **The character screen** (`char_sel_main.rml/.rcss`, `char_info_balloon.rcss`): the 0.97k
  dragon frame round the window (its sides keep their shape, the top and bottom stretch between
  them on a wide window), the New Character panel at the top, Delete on the left dragon and OK on
  the right one, each lit under the pointer. The panels slide in easing out, as the 0.97k ones
  did: New Character drops in while a slot is free and rises off the screen when every slot is
  taken; Delete and OK come in from their sides once a character is picked. A panel that is not
  wanted when the screen opens stays off it without sliding. The screen fills the window's height,
  as the 0.97k one did, whatever the UI scale. There is no menu button: ESC opens the menu. Over
  each character, where the 0.97k client wrote them (190 over its feet, in that client's font
  size, 13 pixels at 800x600), the name in bold over its class and level, each on a
  half-black band (a flagged character's name black on yellow, cyan or blue), with no balloon
  and no guild line.
- **The character scene** (`ClassicCharacterScene=1` in `theme.ini`): the 0.97k one instead of the
  character world. The characters stand on the stone yard of `Logo05` (`Data/Interface97/Logo/`) at
  their world size, their mounts too, in the 0.97k diagonal row climbing away to the right, seen
  from the 0.97k camera with no fog. The selected one is circled by the 0.97k ring (two bands of
  `Magic_Circle1` round its feet, turning opposite ways), without Sven's aurora and light column. A
  theme switch shows on the next visit.
- **Character creation** (`char_make.rml/.rcss`, `ClassicCharacterCreation=1` in `theme.ini`):
  the 0.97k New Character panel under the frame, dropping in from the top each time it opens, the
  class's 3D face from `Data/Logo/Face01..04` (and `face05` for the dark lord) in its window at
  71,89 (the dark lord's raised to fill it as the others do), lit as the 0.97k client did and
  animated as it did: each action at its own speed, every frame of it, then one of
  the face's two actions at random. The name line, and the class line in white with two arrows
  (pressed only while clicked) that step through the dark wizard, dark knight, elf, magic
  gladiator and dark lord (those the server allows; no summoner or rage fighter). Cancel and OK
  at the foot, lit under the pointer. While it is open the character panels and name tags are
  hidden and the scene behind is dimmed.
- **Images are drawn sharp** (`NearestFiltering=1` in `theme.ini`), not smoothed, when the UI is
  scaled.

Windows move to the 0.97k look in the phases of
`docs/superpowers/specs/2026-10-23-interface97-theme-plan.md`. Until a phase lands, its windows
still show the `legacy` look.
