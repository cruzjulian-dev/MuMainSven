#include "stdafx.h"

#include "UI/Theme/ThemeSelection.h"

#include "Core/Platform/SystemFonts.h"
#include "Core/Utilities/StringUtils.h"
#include "Data/GameConfig/GameConfig.h"
#include "UI/RmlBridge/RmlTheme.h"

#include <string>
#include <utility>

void ReinitializeFonts();

namespace UI::Theme
{
namespace
{
// What the native fonts are built from, so a switch can tell whether they need rebuilding.
std::pair<std::string, bool> NativeFontInUse()
{
    return {ResolveUiFontFamily(UI::RmlBridge::GetNativeFontFamily()), UI::RmlBridge::ThemeHasClassicFontControls()};
}
} // namespace

bool Select(const std::string& name, Persistence persistence)
{
    if (!UI::RmlBridge::ThemeExists(name))
        return false;

    const auto previousNativeFont = NativeFontInUse();
    auto& config = GameConfig::GetInstance();
    config.SetRmlTheme(StringUtils::NarrowToWide(name));
    if (persistence == Persistence::Saved)
        config.Save();
    UI::RmlBridge::SetActiveThemeName(name);
    if (NativeFontInUse() != previousNativeFont)
        ReinitializeFonts();
    UI::RmlBridge::ReloadAllThemedDocuments();
    return true;
}
} // namespace UI::Theme
