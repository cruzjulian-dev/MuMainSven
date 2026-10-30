#include "stdafx.h"
#include "Core/Platform/SystemFonts.h"

#include <filesystem>
#include <string>

namespace
{
std::filesystem::path SystemFontDirectory()
{
#ifdef _WIN32
    wchar_t windowsDirectory[MAX_PATH] = {};
    const UINT length = GetWindowsDirectoryW(windowsDirectory, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return {};
    return std::filesystem::path(windowsDirectory) / L"Fonts";
#else
    // No system font is read here: ResolveUiFont() falls back to the bundled default.
    return {};
#endif
}

bool IsFontFile(const std::string& path)
{
    std::error_code error;
    return std::filesystem::is_regular_file(path, error);
}
} // namespace

const BundledFont* FindSystemVerdana()
{
    static const std::filesystem::path directory = SystemFontDirectory();
    static const std::string regular = (directory / "verdana.ttf").generic_string();
    static const std::string bold = (directory / "verdanab.ttf").generic_string();
    static const BundledFont verdana{kVerdanaFamily, regular.c_str(), bold.c_str()};
    static const bool installed = !directory.empty() && IsFontFile(regular) && IsFontFile(bold);
    return installed ? &verdana : nullptr;
}

const BundledFont& ResolveUiFont(std::string_view configuredFamily)
{
    if (configuredFamily == kVerdanaFamily)
    {
        if (const BundledFont* verdana = FindSystemVerdana())
            return *verdana;
    }
    return ResolveBundledFont(configuredFamily);
}
