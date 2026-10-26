#pragma once

// ClassicSceneStage.h - where the MU 0.97k client's login and character scenes stand in the world

namespace Scenes::ClassicStage
{
// The 0.97k client placed its scenes round (0, 0); objects only live on the terrain's blocks
// (1600 units square), so the scenes stand on them instead, round this point.
inline constexpr float kOriginX = 13600.f;
inline constexpr float kOriginY = 13600.f;

// "No world": nothing is loaded and no map's rules apply, as the 0.97k client's scenes stood on
// no map.
inline constexpr int kNoWorld = -1;
} // namespace Scenes::ClassicStage
