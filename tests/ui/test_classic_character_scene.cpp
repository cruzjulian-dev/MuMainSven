#include <doctest.h>

#include "Character/ClassicCreationClasses.h"
#include "Character/ClassicNameTag.h"

#include <set>

namespace
{
std::function<bool(CLASS_TYPE)> EnabledExcept(std::set<CLASS_TYPE> disabled)
{
    return [disabled](CLASS_TYPE classType) { return disabled.count(classType) == 0; };
}
} // namespace

using Character::ClassicCreation::ClassIndex;
using Character::ClassicCreation::FindStep;

TEST_CASE("the 0.97k creation's arrows step through its classes in its order [ui][classic-scene]")
{
    const auto all = EnabledExcept({});
    CHECK(ClassIndex(CLASS_WIZARD) == 0);
    CHECK(ClassIndex(CLASS_DARK_LORD) == 4);
    CHECK(ClassIndex(CLASS_SUMMONER) == -1);

    CHECK(FindStep(CLASS_WIZARD, 1, all) == CLASS_KNIGHT);
    CHECK(FindStep(CLASS_DARK, 1, all) == CLASS_DARK_LORD);
    CHECK(FindStep(CLASS_KNIGHT, -1, all) == CLASS_WIZARD);
    // The ends don't wrap round.
    CHECK(FindStep(CLASS_WIZARD, -1, all) == -1);
    CHECK(FindStep(CLASS_DARK_LORD, 1, all) == -1);
}

TEST_CASE("the 0.97k creation's arrows skip the classes the account can't create [ui][classic-scene]")
{
    const auto noGladiatorOrLord = EnabledExcept({CLASS_DARK, CLASS_DARK_LORD});
    CHECK(FindStep(CLASS_KNIGHT, 1, noGladiatorOrLord) == CLASS_ELF);
    CHECK(FindStep(CLASS_ELF, 1, noGladiatorOrLord) == -1);

    const auto onlyLord = EnabledExcept({CLASS_WIZARD, CLASS_KNIGHT, CLASS_ELF, CLASS_DARK});
    CHECK(FindStep(CLASS_WIZARD, 1, onlyLord) == CLASS_DARK_LORD);

    // A class the 0.97k creation doesn't have, or no direction, steps nowhere.
    CHECK(FindStep(CLASS_SUMMONER, 1, EnabledExcept({})) == -1);
    CHECK(FindStep(CLASS_KNIGHT, 0, EnabledExcept({})) == -1);
}

TEST_CASE("the 0.97k name tags use that client's font size and rise [ui][classic-scene]")
{
    using namespace Character::ClassicNameTag;
    CHECK(TextPx(640) == doctest::Approx(12.f));
    CHECK(TextPx(800) == doctest::Approx(13.f));
    CHECK(TextPx(1024) == doctest::Approx(14.f));
    CHECK(TextPx(1280) == doctest::Approx(15.f));
    CHECK(TextPx(1920) == doctest::Approx(15.f));
    // Sizes the 0.97k client didn't offer get its smallest font.
    CHECK(TextPx(1152) == doctest::Approx(12.f));

    // Text height plus 3 on the 640x480 screen, stretched over the window.
    CHECK(NameRise(12.f, 480) == doctest::Approx(15.f));
    CHECK(NameRise(15.f, 960) == doctest::Approx(36.f));
}
