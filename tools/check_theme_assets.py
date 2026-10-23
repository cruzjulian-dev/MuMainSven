#!/usr/bin/env python3
"""Theme image guard.

An image a theme names that isn't on disk fails without a word: the texture loader returns
nothing, logs nothing, and the element just draws without its art (theming-and-modding.md,
"Bringing your own images to a theme"). The same happens to a real .tga/.jpg dropped next to a
stylesheet, because the loader only reads the game's OZT/OZJ containers. This script finds both
ahead of time.

Every .tga/.jpg path in a document or stylesheet is resolved the way the client resolves it:

- relative to the file's own location; a shared document under RmlUi/ is loaded under a
  synthetic themes/<name>/ URL, so its paths resolve from there -- unless the stylesheets it
  links sit beside it (loading.rml), which marks a document loaded directly, outside the themes;
- a path that lands inside the theme folder is looked up through the theme's Base chain
  (theme.ini), child first, as ThemeInheritance::ResolveFile does;
- the file that has to exist is the OZT (for .tga) or OZJ (for .jpg) with the same name, matched
  without regard to case, as the client reads it on every platform.

Bound names (`src="banner_src"`) and sprite names (`image(inv-money)`) carry no extension and
are not paths, so they are skipped.

Usage: python3 check_theme_assets.py [--asset-root DIR]
Exit code 0 = clean, 1 = missing image(s) (printed to stderr).
"""
import argparse
import configparser
import os
import pathlib
import re
import sys

# A path token ending in an image extension, quoted or not: `src: "../../../a b.tga"`,
# `src="../x.jpg"`, `decorator: image(../../../x.tga cover)`.
LINK_HREF = re.compile(r'<link[^>]*href="([^"]+)"', re.IGNORECASE)
IMAGE_PATH = re.compile(r'"([^"\n]+?\.(?:tga|jpg))"|([^\s"\'(),;:]+\.(?:tga|jpg))\b', re.IGNORECASE)
RCSS_COMMENT = re.compile(r"/\*.*?\*/", re.DOTALL)
RML_COMMENT = re.compile(r"<!--.*?-->", re.DOTALL)
CONTAINER = {".tga": ".OZT", ".jpg": ".OZJ"}


def strip_comments(path: pathlib.Path, text: str) -> str:
    # Keep the line breaks so reported line numbers stay right.
    pattern = RCSS_COMMENT if path.suffix == ".rcss" else RML_COMMENT
    return pattern.sub(lambda m: "\n" * m.group(0).count("\n"), text)


def find_case_insensitive(path: pathlib.Path) -> bool:
    current = pathlib.Path(path.anchor) if path.is_absolute() else pathlib.Path(".")
    for part in path.parts[len(current.parts) if path.is_absolute() else 0:]:
        if part in ("", "."):
            continue
        if part == "..":
            current = current.parent
            continue
        exact = current / part
        if exact.exists():
            current = exact
            continue
        try:
            match = next((c for c in current.iterdir() if c.name.lower() == part.lower()), None)
        except OSError:
            return False
        if match is None:
            return False
        current = match
    return True


def container_file(path: pathlib.Path) -> pathlib.Path:
    return path.with_suffix(CONTAINER[path.suffix.lower()])


def theme_chain(themes_root: pathlib.Path, name: str) -> list[pathlib.Path]:
    chain: list[pathlib.Path] = []
    seen: set[str] = set()
    while name and name not in seen and (themes_root / name).is_dir():
        seen.add(name)
        chain.append(themes_root / name)
        ini = configparser.ConfigParser(inline_comment_prefixes=None, interpolation=None)
        ini.read(themes_root / name / "theme.ini", encoding="utf-8-sig")
        name = ini.get("Theme", "Base", fallback="").strip().lower()
    return chain


def image_exists(target: pathlib.Path, themes_root: pathlib.Path, chain: list[pathlib.Path]) -> bool:
    normalized = pathlib.Path(os.path.normpath(target))
    theme_dir = chain[0] if chain else None
    if theme_dir is not None:
        try:
            relative = normalized.relative_to(pathlib.Path(os.path.normpath(theme_dir)))
        except ValueError:
            relative = None
        if relative is not None:
            return any(find_case_insensitive(container_file(d / relative)) for d in chain)
    return find_case_insensitive(container_file(normalized))


def check_file(path: pathlib.Path, base_dir: pathlib.Path, themes_root: pathlib.Path,
               chain: list[pathlib.Path]) -> list[str]:
    text = strip_comments(path, path.read_text(encoding="utf-8"))
    errors: list[str] = []
    for line_number, line in enumerate(text.splitlines(), start=1):
        for match in IMAGE_PATH.finditer(line):
            reference = match.group(1) or match.group(2)
            target = base_dir / reference.replace("\\", "/")
            if not image_exists(target, themes_root, chain):
                where = f" (theme '{chain[0].name}')" if chain else ""
                errors.append(f"{path}:{line_number}: '{reference}' has no "
                              f"{container_file(pathlib.Path(reference)).name} on disk{where}")
    return errors


def loaded_outside_themes(document: pathlib.Path) -> bool:
    links = LINK_HREF.findall(document.read_text(encoding="utf-8"))
    return bool(links) and all((document.parent / link).is_file() for link in links)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--asset-root",
        type=pathlib.Path,
        default=pathlib.Path(__file__).resolve().parent.parent / "src" / "bin" / "Data" / "Interface" / "RmlUi",
        help="RmlUi asset root (default: src/bin/Data/Interface/RmlUi)",
    )
    args = parser.parse_args()
    root = args.asset_root.resolve()
    themes_root = root / "themes"
    if not themes_root.is_dir():
        print(f"check_theme_assets: themes folder not found: {themes_root}", file=sys.stderr)
        return 1

    themes = sorted(d.name for d in themes_root.iterdir() if d.is_dir())
    errors: list[str] = []
    checked = 0
    for name in themes:
        chain = theme_chain(themes_root, name)
        for path in sorted(chain[0].rglob("*")):
            if path.suffix in (".rml", ".rcss"):
                errors.extend(check_file(path, path.parent, themes_root, chain))
                checked += 1
    # Themed shared documents resolve under each theme's synthetic URL; "../" from any theme
    # folder reaches the same place, so one pass against the first theme's chain covers them.
    shared_chain = theme_chain(themes_root, themes[0]) if themes else []
    for path in sorted(root.glob("*.rml")):
        if loaded_outside_themes(path) or not shared_chain:
            errors.extend(check_file(path, path.parent, themes_root, []))
        else:
            errors.extend(check_file(path, shared_chain[0], themes_root, shared_chain))
        checked += 1
    for path in sorted(root.glob("*.rcss")):
        errors.extend(check_file(path, path.parent, themes_root, []))
        checked += 1

    if errors:
        print("Theme image check failed:", file=sys.stderr)
        for error in errors:
            print(f"  {error}", file=sys.stderr)
        return 1
    print(f"Theme image check: OK ({checked} files, {len(themes)} themes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
