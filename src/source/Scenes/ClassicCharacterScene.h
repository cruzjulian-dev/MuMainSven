#pragma once

// ClassicCharacterScene.h - the MU 0.97k client's character scene (theme capability ClassicCharacterScene=1)

#include "Scenes/CharacterScene.h"

namespace Scenes::ClassicCharacter
{
// Builds the scene instead of the character world: the stone yard of Logo05 (loaded by
// OpenCharacterSceneData()), with the account's characters standing on it. Stands on no map, as the
// 0.97k client's did.
void Create();

// Leaves the scene; the next character scene may be the character world.
void Release();

// Whether the running character scene is this one.
bool IsActive();

// Per frame, before the characters move: the 0.97k client's camera.
void Move();

// The yard's spot for that slot (Scenes::CreateCharacterInSlot()): the characters the list made
// before the scene are moved onto theirs by Create(), and a new one is created on its own.
CharacterSpot SlotSpot(int slot);

// How high over a character's feet its name, class and level stand.
float NameHeight();

// Draws the 0.97k client's ring round the selected character's feet: two of Magic_Circle1 turning
// opposite ways, fading as they rise.
void CircleSelectedCharacter(const vec3_t feet);
} // namespace Scenes::ClassicCharacter
