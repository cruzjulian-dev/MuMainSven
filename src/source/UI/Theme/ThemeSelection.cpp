#include "stdafx.h"

#include "UI/Theme/ThemeSelection.h"

#include "Core/Platform/SystemFonts.h"
#include "Core/Utilities/StringUtils.h"
#include "Data/GameConfig/GameConfig.h"
#include "UI/RmlBridge/RmlTheme.h"

#include <string_view>

void ReinitializeFonts();

namespace UI::Theme
{
namespace
{
std::string_view NativeFontFamilyInUse()
{
    return ResolveUiFont(UI::RmlBridge::GetNativeFontFamily()).family;
}
} // namespace

bool Select(const std::string& name, Persistence persistence)
{
    if (!UI::RmlBridge::ThemeExists(name))
        return false;

    const std::string_view previousNativeFamily = NativeFontFamilyInUse();
    auto& config = GameConfig::GetInstance();
    config.SetRmlTheme(StringUtils::NarrowToWide(name));
    if (persistence == Persistence::Saved)
        config.Save();
    UI::RmlBridge::SetActiveThemeName(name);
    if (NativeFontFamilyInUse() != previousNativeFamily)
        ReinitializeFonts();
    UI::RmlBridge::ReloadAllThemedDocuments();
    return true;
}
} // namespace UI::Theme
