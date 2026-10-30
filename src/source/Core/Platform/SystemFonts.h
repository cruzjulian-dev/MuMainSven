// UI fonts read from the player's operating system instead of ./fonts.
//
// Installed fonts are not ours to ship, so the client asks Windows for the bytes of
// the file behind a family. Where a family is missing (Linux, macOS) the choice falls
// back to the bundled default, DejaVu Sans, which every platform has and whose wide
// letters fit the same layouts.
#pragma once

#include "Core/Platform/BundledFonts.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// One installed face in memory. The data lives until the process exits: SDL_ttf and
// RmlUi read from it for as long as a font they opened from it is alive.
struct SystemFontFace
{
    std::vector<unsigned char> data;
    int faceIndex = 0;          // inside a .ttc collection
    bool syntheticBold = false; // the family has no such face: the renderer must fake it
    bool syntheticItalic = false;
};

// Every installed family, sorted. Empty where the system's fonts are not read.
[[nodiscard]] std::vector<std::string> EnumerateSystemFontFamilies();

// The installed face for a family and style, or nullptr when the family is not installed.
[[nodiscard]] const SystemFontFace* FindSystemFontFace(std::string_view family, bool bold, bool italic);

// The family a configured name draws with: a bundled or installed family, or else the
// bundled default (ResolveBundledFont()).
[[nodiscard]] std::string ResolveUiFontFamily(std::string_view configuredFamily);

// The em size GDI gives a font created with this cell height (CreateFont's positive
// height, which also holds the family's internal leading). SDL_ttf and RmlUi size by em.
[[nodiscard]] int FontEmPixels(std::string_view family, int cellHeight);
