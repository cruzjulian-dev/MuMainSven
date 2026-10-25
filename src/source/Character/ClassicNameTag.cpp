#include "Character/ClassicNameTag.h"

namespace Character::ClassicNameTag
{
float TextPx(unsigned int windowWidth)
{
    if (windowWidth >= 1280)
        return 15.f;
    if (windowWidth == 1024)
        return 14.f;
    if (windowWidth == 800)
        return 13.f;
    return 12.f;
}

float NameRise(float textPx, unsigned int windowHeight)
{
    constexpr float kReferenceHeight = 480.f;
    constexpr float kNameGap = 3.f;
    return (textPx + kNameGap) * static_cast<float>(windowHeight) / kReferenceHeight;
}
} // namespace Character::ClassicNameTag
