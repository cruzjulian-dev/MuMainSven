#include "Character/ClassicNameTag.h"

#include <array>

namespace Character::ClassicNameTag
{
namespace
{
// The 0.97k client's font for each window width it offered (WinMain); widths past the last take
// the last one's.
struct WidthFont
{
    unsigned int windowWidth;
    float textPx;
};

constexpr std::array<WidthFont, 3> kFontByWidth{{
    {800, 13.f},
    {1024, 14.f},
    {1280, 15.f},
}};
constexpr unsigned int kWidestOfferedWidth = kFontByWidth.back().windowWidth;

// Every other width, as that client's 640x480 default.
constexpr float kDefaultTextPx = 12.f;

// The 640x480 screen the 0.97k client stretched over the window, and the gap it left between a
// tag's two lines.
constexpr float kReferenceHeight = 480.f;
constexpr float kNameGap = 3.f;
} // namespace

float TextPx(unsigned int windowWidth)
{
    if (windowWidth >= kWidestOfferedWidth)
        return kFontByWidth.back().textPx;
    for (const WidthFont& font : kFontByWidth)
    {
        if (font.windowWidth == windowWidth)
            return font.textPx;
    }
    return kDefaultTextPx;
}

float NameRise(float textPx, unsigned int windowHeight)
{
    return (textPx + kNameGap) * static_cast<float>(windowHeight) / kReferenceHeight;
}
} // namespace Character::ClassicNameTag
