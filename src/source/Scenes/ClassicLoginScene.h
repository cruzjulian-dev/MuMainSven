#pragma once

// ClassicLoginScene.h - the MU 0.97k client's login scene (theme capability ClassicLoginScene=1)

#include "Render/Textures/ZzzOpenglUtil.h" // vec3_t
#include "UI/Scaling/UITransform.h"

class OBJECT;

namespace Scenes::ClassicLogin
{
// Builds the scene instead of the login world: three ships with their wakes on the sea under the
// sky, the sun on the horizon, the MU logo standing over the water and three characters on the
// decks. Stands on no map, as the 0.97k client's did.
void Create();

// Leaves the scene; the next login scene may be the login world.
void Release();

// Whether the running login scene is this one.
bool IsActive();

// Where the scene stands in the world: the camera's walk is relative to it.
void GetOrigin(vec3_t origin);

// Per frame, after the characters moved: the characters' random gestures, their heads held still,
// and the title sound.
void Move();

// The 3D view: the 0.97k client's 640x320 strip from y=80, black above and below.
UI::Scaling::Viewport Viewport();

// The two torches of one of the scene's ships at RenderObjectVisual()'s flickering `level`.
void RenderShipTorches(OBJECT* ship, float level);
} // namespace Scenes::ClassicLogin
