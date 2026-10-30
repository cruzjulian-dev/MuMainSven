// UI fonts read from the player's operating system instead of ./fonts.
//
// Verdana is Microsoft's and is not ours to ship, so the client reads the files
// Windows installs. Where they are missing (Linux, macOS) the choice falls back to
// the bundled default, DejaVu Sans, which every platform has and whose wide letters
// fit the same layouts.
#pragma once

#include "Core/Platform/BundledFonts.h"

#include <string_view>

inline constexpr const char* kVerdanaFamily = "Verdana";

// Verdana's installed files as absolute paths, or nullptr when it is not installed.
[[nodiscard]] const BundledFont* FindSystemVerdana();

// The font a configured family draws with: the installed Verdana, or else ResolveBundledFont().
[[nodiscard]] const BundledFont& ResolveUiFont(std::string_view configuredFamily);
