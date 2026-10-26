///////////////////////////////////////////////////////////////////////////////
// ClassicLoginScene.cpp - the MU 0.97k client's login scene
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClassicLoginScene.h"

#include "Audio/DSPlaySound.h"
#include "Data/DataHandler/LoadData.h"
#include "Engine/AI/GOBoid.h"
#include "Engine/AI/ZzzAI.h"
#include "Engine/Object/ZzzCharacter.h"
#include "Engine/Object/ZzzObject.h"
#include "Render/Effects/ZzzEffect.h"
#include "Render/Models/ZzzBMD.h"
#include "Render/Terrain/ZzzLodTerrain.h"
#include "Scenes/ClassicSceneStage.h"
#include "Scenes/LoginScene.h"
#include "World/MapInfra/MapManager.h"

#include <iterator>

// The scene as the 0.97k client built it (its login scene setup and Scene_Login()): positions,
// gestures and the view are its own. The objects' lights, and their scales but the logo's, are
// CreateObject()'s login cases, the sky's and the wakes' scrolling and the logo's fade-in MoveObject()'s.
namespace
{
// The scene stands round ClassicSceneStage.h's origin. RenderObjects() draws block by block, each in
// creation order: from there every object but the sun shares one block, so the sky and sea come
// first and the wakes after them, as the 0.97k client drew them.
using Scenes::ClassicStage::kNoWorld;
using Scenes::ClassicStage::kOriginX;
using Scenes::ClassicStage::kOriginY;

constexpr float kShipYaw = 180.f;

struct Placement
{
    int type;
    float x, y, z;
    float yaw;
    float scale = 0.f; // 0: CreateObject()'s own
};

// The sky, each ship with its wake, the MU logo over the water and the sun.
constexpr Placement kObjects[] = {
    {MODEL_LOGO, 0.f, -520.f, 0.f, 0.f},
    {MODEL_SHIP, -700.f, 700.f, 0.f, kShipYaw},
    {MODEL_WAVEBYSHIP, -700.f, 700.f, 0.f, kShipYaw},
    {MODEL_SHIP, 400.f, 400.f, 0.f, kShipYaw},
    {MODEL_WAVEBYSHIP, 400.f, 400.f, 0.f, kShipYaw},
    {MODEL_SHIP, -200.f, -400.f, 0.f, kShipYaw},
    {MODEL_WAVEBYSHIP, -200.f, -400.f, 0.f, kShipYaw},
    // The 0.97k client's MU logo stood smaller than the newer clients' login scenes drew it.
    {MODEL_MUGAME, 0.f, -600.f, 480.f, 0.f, 0.6f},
    {MODEL_LOGOSUN, -110.f, 1600.f, 50.f, 0.f},
};

// Data\Logo\Logo01..04.bmd: the sky, the wake, the MU logo and the sun.
constexpr int kLogoModelCount = 4;

// The characters on the decks, in their classes' plain clothes, each with two gestures it makes
// now and then: a knight and a wizard at the bow of the near ship, an elf with her guardian angel
// on the ship to the right.
struct Sailor
{
    int key;
    CLASS_TYPE characterClass;
    float x, y;
    int gestures[2];
    bool guardianAngel;
};

constexpr Sailor kSailors[] = {
    {1, CLASS_KNIGHT, -200.f, -750.f, {PLAYER_DIRECTION1, PLAYER_SEE1}, false},
    {2, CLASS_WIZARD, -240.f, -770.f, {PLAYER_SMILE1, PLAYER_SEE1}, false},
    {3, CLASS_ELF, 400.f, 50.f, {PLAYER_WIN1, PLAYER_CHEER1}, true},
};

constexpr float kDeckHeight = 250.f;
constexpr float kSailorScale = 0.6f;
constexpr float kSailorYaw = 180.f;
// A gesture's chance each 25 fps frame: one in this many.
constexpr int kGestureFrames = 128;
constexpr int kNoGesture = -1;

// The 0.97k client's 3D viewport on its 640x480 screen.
constexpr UI::Scaling::Viewport kViewport = {0, 80, 640, 320};

// The ship model's two torch bones and the torches' flame colour (shares of the flickering level).
constexpr int kShipTorchBones[] = {8, 9};
constexpr float kTorchGreen = 0.5f;
constexpr float kTorchBlue = 0.2f;
constexpr float kTorchScale = 1.f;

bool s_active = false;
// Whether the title sound has played: once per visit, as the logo lights up.
bool s_titlePlayed = false;
// The gesture each sailor made last: the 0.97k client's characters held a gesture's last pose, so
// rolling the same one again changed nothing and only the other one showed.
int s_lastGesture[std::size(kSailors)];

void LoadModels()
{
    gLoadData.AccessModel(MODEL_SHIP, L"Data\\Object1\\", L"Ship", 1);
    gLoadData.OpenTexture(MODEL_SHIP, L"Object1\\", GL_REPEAT, GL_LINEAR);
    for (int i = 0; i < kLogoModelCount; ++i)
    {
        gLoadData.AccessModel(MODEL_LOGO + i, L"Data\\Logo\\", L"Logo", i + 1);
        gLoadData.OpenTexture(MODEL_LOGO + i, L"Logo\\", GL_REPEAT, GL_LINEAR);
    }
}

void CreateObjects()
{
    for (const Placement& placement : kObjects)
    {
        vec3_t position = {kOriginX + placement.x, kOriginY + placement.y, placement.z};
        vec3_t angle = {0.f, 0.f, placement.yaw};
        OBJECT* o = CreateObject(placement.type, position, angle);
        if (o != nullptr && placement.scale > 0.f)
            o->Scale = placement.scale;
    }
}

// Every item slot from scratch: the slot may last have held a player from the world, whose
// excellent and ancient options would still light the clothes up.
void DressInPlainClothes(CHARACTER* c)
{
    for (PART_t& part : c->BodyPart)
        part = PART_t();
    c->Weapon[0] = PART_t();
    c->Weapon[1] = PART_t();
    c->Wing = PART_t();
    c->Helper = PART_t();
    c->Flag = PART_t();

    const int bodyModels[] = {static_cast<int>(MODEL_BODY_HELM), static_cast<int>(MODEL_BODY_ARMOR),
                              static_cast<int>(MODEL_BODY_PANTS), static_cast<int>(MODEL_BODY_GLOVES),
                              static_cast<int>(MODEL_BODY_BOOTS)};
    const int bodyParts[] = {BODYPART_HELM, BODYPART_ARMOR, BODYPART_PANTS, BODYPART_GLOVES, BODYPART_BOOTS};
    for (int i = 0; i < static_cast<int>(std::size(bodyParts)); ++i)
        c->BodyPart[bodyParts[i]].Type = bodyModels[i] + c->SkinIndex;
    c->Weapon[0].Type = -1;
    c->Weapon[1].Type = -1;
    c->Wing.Type = -1;
    c->Helper.Type = -1;
    c->Flag.Type = -1;
}

void CreateSailors()
{
    for (int& gesture : s_lastGesture)
        gesture = kNoGesture;
    for (const Sailor& sailor : kSailors)
    {
        CHARACTER* c = CreateHero(sailor.key, sailor.characterClass, 0, kOriginX + sailor.x, kOriginY + sailor.y,
                                  kSailorYaw);
        DressInPlainClothes(c);
        OBJECT* o = &c->Object;
        // CreateHero() stood them for the login scene's mounted outfit; they stand on foot from the
        // first frame, with no blend from the mounted pose.
        c->SafeZone = true;
        SetPlayerStop(c);
        o->PriorAction = o->CurrentAction;
        o->PriorAnimationFrame = o->AnimationFrame;
        o->Position[2] = kDeckHeight;
        o->Scale = kSailorScale;
        if (sailor.guardianAngel)
        {
            c->Helper.Type = MODEL_HELPER;
            CreateMount(MODEL_HELPER, o->Position, o);
        }
    }
}

// The 0.97k client only animated these characters: they never turned their heads, and a gesture
// started only when it was not the one made last, and one at a time.
void MoveSailors()
{
    for (int i = 0; i < static_cast<int>(std::size(kSailors)); ++i)
    {
        const Sailor& sailor = kSailors[i];
        OBJECT* o = &CharactersClient[sailor.key].Object;
        Vector(0.f, 0.f, 0.f, o->HeadAngle);
        Vector(0.f, 0.f, 0.f, o->HeadTargetAngle);
        for (const int gesture : sailor.gestures)
        {
            if (gesture != s_lastGesture[i] && rand_fps_check(kGestureFrames))
            {
                SetAction(o, gesture);
                s_lastGesture[i] = gesture;
                break;
            }
        }
    }
}
} // namespace

namespace Scenes::ClassicLogin
{
void Create()
{
    gMapManager.DeleteObjects();
    ClearCharacters();
    gMapManager.WorldActive = kNoWorld;
    DeactivateTerrain();

    LoadModels();
    CreateObjects();
    CreateSailors();
    s_titlePlayed = false;
    s_active = true;
}

void Release()
{
    s_active = false;
}

bool IsActive()
{
    return s_active;
}

void GetOrigin(vec3_t origin)
{
    Vector(kOriginX, kOriginY, 0.f, origin);
}

void Move()
{
    MoveSailors();

    // The title sound as the logo lights up, when the camera starts its first walk. Only then: the
    // 0.97k client's decompiled MoveCamera() suggests one at every new walk (each 128 frames), but
    // the 0.97k client heard in game plays it once.
    if (!s_titlePlayed && GetLoginCameraWalkCut() == 1 && GetLoginCameraCount() == 0)
    {
        PlayBuffer(SOUND_TITLE01);
        s_titlePlayed = true;
    }
}

UI::Scaling::Viewport Viewport()
{
    return kViewport;
}

void RenderShipTorches(OBJECT* ship, float level)
{
    BMD* b = &Models[ship->Type];
    vec3_t light = {level, level * kTorchGreen, level * kTorchBlue};
    vec3_t offset = {0.f, 0.f, 0.f};
    vec3_t position;
    for (const int bone : kShipTorchBones)
    {
        b->TransformPosition(BoneTransform[bone], offset, position);
        CreateSprite(BITMAP_LIGHT, position, kTorchScale, light, ship);
    }
}
} // namespace Scenes::ClassicLogin
