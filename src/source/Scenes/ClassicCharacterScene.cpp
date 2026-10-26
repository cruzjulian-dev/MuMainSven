///////////////////////////////////////////////////////////////////////////////
// ClassicCharacterScene.cpp - the MU 0.97k client's character scene
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClassicCharacterScene.h"

#include "Camera/CameraState.h"
#include "Engine/Object/ZzzCharacter.h"
#include "Engine/Object/ZzzObject.h"
#include "Render/Effects/ZzzEffect.h"
#include "Render/Terrain/ZzzLodTerrain.h"
#include "Scenes/ClassicSceneStage.h"
#include "Scenes/LoginScene.h"
#include "World/MapInfra/MapManager.h"

// The scene as the 0.97k client built it (its character scene setup, ReceiveCharacterList() and
// Scene_CharSelect()): the stage, the characters' spots and the view are its own.
namespace
{
// The stage stands where the 0.97k login scene does (ClassicSceneStage.h).
using Scenes::ClassicStage::kNoWorld;
using Scenes::ClassicStage::kOriginX;
using Scenes::ClassicStage::kOriginY;

// Logo05, loaded by OpenCharacterSceneData(); CreateObject() lights it.
constexpr int kStageModel = MODEL_LOGO + 4;

// The yard's floor: the stage's paving lies at the model's own height 0.
constexpr float kFloorHeight = 0.f;

// The 0.97k client stood slot n at (100n, 50n - 50), turned (n - 1) * 15 degrees: a diagonal
// row climbing away to the right.
constexpr float kSlotStepX = 100.f;
constexpr float kSlotStepY = 50.f;
constexpr float kFirstSlotY = -50.f;
constexpr float kSlotTurn = 15.f;

// The 0.97k client projected each character's name and class this high over its feet.
constexpr float kNameHeight = 190.f;

// The 0.97k client's selection ring (Scene_CharSelect()): Magic_Circle1 on a band widening from
// round the feet up to the knees, white at the bottom and gone at the top, turning a tenth of a
// degree each millisecond (once round in 3.6 seconds).
constexpr int kRingTexture = BITMAP_MAGIC + 2;
constexpr float kRingBottomRadius = 35.f;
constexpr float kRingTopRadius = 55.f;
constexpr float kRingHeight = 150.f;
constexpr int kRingTurnPeriod = 3600;
constexpr float kRingTurnPerMs = 0.1f;

// The 0.97k client's camera over the yard (its camera walk's sixth point), with the login
// scene's lens.
constexpr float kCameraFov = 45.f;
constexpr float kCameraX = 200.f;
constexpr float kCameraY = -800.f;
constexpr float kCameraZ = 300.f;
constexpr float kCameraPitch = -80.f;
constexpr float kCameraYaw = -10.f;
// Far enough for the whole yard behind the last character; the near plane is the login scene's,
// which keeps the depth buffer precise.
constexpr float kViewFar = 3000.f;
constexpr float kViewNear = 100.f;

bool s_active = false;

void CreateStage()
{
    vec3_t position = {kOriginX, kOriginY, kFloorHeight};
    vec3_t angle = {0.f, 0.f, 0.f};
    CreateObject(kStageModel, position, angle);
}

// No map is loaded, but the terrain still holds the last map's heights and attributes: the
// characters' shadows fell on its heights, and its safe zone hid the mounts standing on it
// (MoveMount() fades a Fenrir, Dark Horse, Uniria or Dinorant out there). The terrain is laid
// flat at the yard's floor with no attributes instead, as the 0.97k client's empty terrain was.
// The next map loads its own.
void ClearTerrain()
{
    for (int i = 0; i < TERRAIN_SIZE * TERRAIN_SIZE; ++i)
    {
        BackTerrainHeight[i] = kFloorHeight;
        TerrainWall[i] = 0;
    }
    MarkTerrainHeightsChanged();
}

void AimCamera()
{
    vec3_t origin = {kOriginX, kOriginY, 0.f};
    vec3_t position = {kCameraX, kCameraY, kCameraZ};
    vec3_t angle = {kCameraPitch, 0.f, kCameraYaw};
    g_Camera.FOV = kCameraFov;
    g_Camera.ViewFar = kViewFar;
    g_Camera.ViewNear = kViewNear;
    MoveCharacterCamera(origin, position, angle);
}

// The character list comes before the scene is created, so it placed and sized its characters for
// the character world; once the scene is active they move onto the yard, at its size.
void MoveListedCharactersOntoYard()
{
    for (int slot = 0; slot < MAX_CHARACTERS_PER_ACCOUNT; ++slot)
    {
        CHARACTER* c = &CharactersClient[slot];
        OBJECT* o = &c->Object;
        if (!o->Live)
            continue;

        const Scenes::CharacterSpot spot = Scenes::ClassicCharacter::SlotSpot(slot);
        o->Position[0] = spot.x;
        o->Position[1] = spot.y;
        o->Position[2] = spot.z;
        o->Angle[2] = spot.angle;
        SetCharacterScale(c);
    }
}
} // namespace

namespace Scenes::ClassicCharacter
{
void Create()
{
    gMapManager.DeleteObjects();
    gMapManager.WorldActive = kNoWorld;
    DeactivateTerrain();
    ClearTerrain();

    CreateStage();
    s_active = true;
    MoveListedCharactersOntoYard();
}

void Release()
{
    s_active = false;
}

bool IsActive()
{
    return s_active;
}

void Move()
{
    AimCamera();
}

Scenes::CharacterSpot SlotSpot(int slot)
{
    return {kOriginX + slot * kSlotStepX, kOriginY + slot * kSlotStepY + kFirstSlotY, kFloorHeight,
            (slot - 1) * kSlotTurn};
}

float NameHeight()
{
    return kNameHeight;
}

void CircleSelectedCharacter(const vec3_t feet)
{
    vec3_t position;
    VectorCopy(feet, position);
    const float turn = static_cast<float>(static_cast<int>(WorldTime) % kRingTurnPeriod) * kRingTurnPerMs;

    EnableAlphaBlend();
    RenderCircle(kRingTexture, position, kRingBottomRadius, kRingTopRadius, kRingHeight, turn, 0.f, 0.f);
    RenderCircle(kRingTexture, position, kRingBottomRadius, kRingTopRadius, kRingHeight, -turn, 0.f, 0.f);
    DisableAlphaBlend();
}
} // namespace Scenes::ClassicCharacter
