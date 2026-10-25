#pragma once

// ClassicNameTag.h - where the 0.97k character scene writes a character's name and class
// (CCharInfoBalloonMng with the 0.97k scene)

namespace Character::ClassicNameTag
{
// The 0.97k client's font (WinMain): 12 pixels high at 640 wide, 13 at 800, 14 at 1024 and 15
// from 1280 on.
float TextPx(unsigned int windowWidth);

// The 0.97k client wrote a tag's class line at the character's projected point and the name above
// it, its text's height plus 3 higher on the 640x480 screen, which it stretched over the window.
float NameRise(float textPx, unsigned int windowHeight);
} // namespace Character::ClassicNameTag
