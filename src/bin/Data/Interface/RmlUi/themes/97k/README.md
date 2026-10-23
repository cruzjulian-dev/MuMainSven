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
  shown while typing, draws over the bar (`[Stacking]` in `theme.ini`); the ear in it blocks
  whispers. There are no chat-type buttons: start a message with ~ (party), @ (guild) or
  $ (Gens).
- **Images are drawn sharp** (`NearestFiltering=1` in `theme.ini`), not smoothed, when the UI is
  scaled.

Windows move to the 0.97k look in the phases of
`docs/superpowers/specs/2026-10-23-interface97-theme-plan.md`. Until a phase lands, its windows
still show the `legacy` look.
