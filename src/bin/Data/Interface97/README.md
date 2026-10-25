# Interface97

2D interface art of the MU Online 0.97k client, used by the `97k` UI theme
(`Data/Interface/RmlUi/themes/97k/`). Nothing else in the client reads this folder.

## Where it comes from

`Mu-97k-Client-Source` (the reconstructed 0.97k client, `main.exe` MD5
`eb95ac0785e40a7ad60c9ddb5d8bef34`):

- every file of `bin/Client/Data/Interface/` (96 files), here at the top level;
- the 2D login, server and character screen textures of `bin/Client/Data/Logo/`, in `Logo/`.
  The models, skies and other 3D scenery of the 0.97k login are not included: the login scene
  stays the MuMainSven one.

`Logo/Logo05.bmd` and its seven `scs-*.OZJ` textures are the stone yard of the theme's character
scene (`ClassicCharacterScene=1`), from the 0.97k data the project was given. The textures shed the
`-jmk` their file names carried, and the model names them without it too (only the names in its
texture slots changed). `Data/Logo/Logo05.bmd` is MuMainSven's own and stays for the other themes.

`win_button.OZT` and `win_scrollbar.OZJ` come from the Season 3 client's `Data/Interface/`
instead, for the theme's guild window (the 0.97k client had no such art); both are
byte-identical to the MuMainSven ones. So do `btn_pshop.OZJ`, `btn_pshop2.OZJ` (the inventory's
personal shop button), `Quest1.OZJ`, `Quest2.OZJ`, `pet1.OZJ` and `pet2.OZJ` (the character
window's shortcuts), which MuMainSven does not have, and `CursorID_Wnd.OZJ` (the command window's
target box), which differs from MuMainSven's `newui_Cursorid_wnd`.

File names are the 0.97k ones; only the extensions are written `.OZJ`/`.OZT`, like the rest of
`Data/`. The client reads them without regard to case.

## Why a folder of its own

Several 0.97k files share a name with a different MuMainSven picture, and the `97k` theme must
not change how `legacy` and `modern` look. This folder is also complete on its own: it holds the
47 files that are byte-identical to `Data/Interface/` too, so the theme keeps working if those
are ever replaced there.

A theme stylesheet names a file here as `../../../../Interface97/<name>.jpg` (OZJ) or `.tga`
(OZT); `tools/check_theme_assets.py` reports a name with no file behind it.
