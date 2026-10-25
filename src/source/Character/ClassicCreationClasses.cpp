#include "stdafx.h"
#include "ClassicCreationClasses.h"

#include <algorithm>

namespace Character::ClassicCreation
{
int ClassIndex(CLASS_TYPE classType)
{
    const auto found = std::find(kClasses.begin(), kClasses.end(), classType);
    return found == kClasses.end() ? -1 : static_cast<int>(found - kClasses.begin());
}

int FindStep(CLASS_TYPE selected, int direction, const std::function<bool(CLASS_TYPE)>& enabled)
{
    const int index = ClassIndex(selected);
    if (direction == 0 || index < 0)
        return -1;

    const int step = direction < 0 ? -1 : 1;
    for (int i = index + step; i >= 0 && i < static_cast<int>(kClasses.size()); i += step)
    {
        if (enabled(kClasses[i]))
            return kClasses[i];
    }
    return -1;
}
} // namespace Character::ClassicCreation
