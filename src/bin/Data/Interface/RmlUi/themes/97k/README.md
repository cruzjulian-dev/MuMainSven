# MU 0.97k theme

The `97k` theme dresses MuMainSven in the interface of the MU Online 0.97k client. Its art is in
`Data/Interface97/`; this folder holds only the stylesheets and markup that differ from `legacy`.

## Choosing it

- **Options → UI → Theme → MU 0.97k**, saved in `config.ini` as `[UI] RmlTheme=97k`.
- `$theme 97k` in the chat box for the current session only.

## What it covers

The theme inherits `legacy` (`theme.ini`). A window this folder doesn't restyle looks exactly as
in `legacy`:

- **The guild window (G)** keeps the Sven interface on purpose.
- Windows the 0.97k client never had (MU Helper, Gens, Master Level, the in-game shop, quests and
  later events) keep the Sven look.

## Restyled so far

- **The bottom bar** (`main_frame.rcss`): the 0.97k bar with its corner pieces, the HP and MP
  orbs (green while poisoned), the AG column, EXP, the Q/W/E potions, the current skill and its
  list above the bar, and the Party, Character, Inventory and Guild buttons. The 0.97k bar has no
  SD gauge, no skill hotkey row, no fourth potion box and no shop, friend or menu buttons; those
  keep working from their keys (R, the number keys, F, U).

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
  every item grid's. Sven's personal-shop and expansion buttons stay beside the 0.97k ones.
- **The character window** (`character_info.rcss`): the level and each attribute in a
  `Level_box`, the derived lines on black bands under it, the 0.97k point buttons, the points to
  spend on an orange band. The value stands beside its box: the 0.97k client wrote both in it
  ("Fuerza: 28"), which a longer label doesn't fit. Sven's fruit-probability line is hidden; the quest, pet and master-level shortcuts stay
  beside the exit.
- **Trade** (`trade.rcss`): the 0.97k panel with `Trading_line` across it, the partner's offer
  above and the hero's below in the 0.97k cells, both amounts in `Item_Money` boxes, and the
  0.97k zen, accept (`Accept_box01`, `Accept_box02` once accepted) and exit buttons on one row.
  The line sits a little lower than in the 0.97k client so Sven's warning and notices fit above
  it; the partner's accept box stands beside the partner's money.
- **The NPC shop** (`npc_shop.rml/.rcss`), **the storage** (`storage.rml/.rcss`) and **the chaos
  mix** (`mix_inventory.rml/.rcss`): the 0.97k panel and cells, their amounts in `Item_Money`
  boxes and the 0.97k buttons (`Repair_01`; `Money_01` and the lock, `Lock_01`/`Lock_03`;
  `Mix_button1`). The 0.97k panel has no corner close, so each fork of the markup adds only the
  0.97k exit; Sven's own buttons (repair all, storage expansion) take 0.97k art or keep theirs
  beside the 0.97k ones.
- **The party** (`party_info.rcss`, `party_list.rcss`): the window lists each member as the 0.97k
  client did (the leader's name on a blue band, the 0.97k health bar, the kick as the 0.97k
  exit); the list down the right side is the 0.97k one, dark cards with gold sides and a health
  bar of ten red steps.
- **The Golden Archer** (`gold_bowman.rcss`): the 0.97k exit, and Register as the 0.97k dialog's
  Ok. **The Devil Square and Blood Castle entries** (`event_entry.rcss`): each level a black band
  edged in the 0.97k gold.
- **The pet and summon life** (`item_endurance.rcss`): the name on a half-black band over the
  0.97k teal-and-green bar. The durability icons keep the `legacy` look until their phase.
- **Images are drawn sharp** (`NearestFiltering=1` in `theme.ini`), not smoothed, when the UI is
  scaled.

Windows move to the 0.97k look in the phases of
`docs/superpowers/specs/2026-10-23-interface97-theme-plan.md`. Until a phase lands, its windows
still show the `legacy` look.
