#pragma once

// ClassicCreationClasses.h - the classes the 0.97k character creation (CCharMakeWin with the 0.97k
// faces) steps through with its arrows

#include "Core/Platform/WinCompat.h" // BYTE, used by _enum.h
#include "Core/Globals/_enum.h"

#include <array>
#include <functional>

namespace Character::ClassicCreation
{
// In the 0.97k client's order; the dark lord, which came after that client, follows the magic
// gladiator.
inline constexpr std::array<CLASS_TYPE, 5> kClasses{
    CLASS_WIZARD, CLASS_KNIGHT, CLASS_ELF, CLASS_DARK, CLASS_DARK_LORD
};

// The class's place in kClasses, or -1 for a class the 0.97k creation doesn't have.
int ClassIndex(CLASS_TYPE classType);

// The class an arrow (direction -1 or 1) steps to from `selected`: the nearest one that way the
// account may create (`enabled`), or -1 when there is none or `selected` isn't one of kClasses.
int FindStep(CLASS_TYPE selected, int direction, const std::function<bool(CLASS_TYPE)>& enabled);
} // namespace Character::ClassicCreation
