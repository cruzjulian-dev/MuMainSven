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

- **The chat** (`chat_input.rcss`, `chat_log.rml/.rcss`, `workspace.rcss`): centred over the
  bar as in the 0.97k client. The input box shows while typing; the ear in it blocks whispers, and
  the show-chat, size and transparency buttons sit right of it. The log has the 0.97k frame,
  scrollbar and resize grip (F5 shows the frame). There are no chat-type buttons: start a message
  with ~ (party), @ (guild) or $ (Gens).
- **Images are drawn sharp** (`NearestFiltering=1` in `theme.ini`), not smoothed, when the UI is
  scaled.

Windows move to the 0.97k look in the phases of
`docs/superpowers/specs/2026-10-23-interface97-theme-plan.md`. Until a phase lands, its windows
still show the `legacy` look.
