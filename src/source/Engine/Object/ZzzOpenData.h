#pragma once

void DeleteNpcs();
void OpenNpc(int Type);
void DeleteMonsters();
void OpenMonsterModel(EMonsterModelType Type);

void OpenModel(int Type, wchar_t* Dir, wchar_t* ModelFileName, ...);
void OpenBasicData(HDC hDC);
void ReleaseMainData();
bool OpenFont();
void OpenLogoSceneData();
void ReleaseLogoSceneData();
// What the character scene loads: the newer stage and faces, or the 0.97k client's. The scene
// picks it (CreateCharacterScene()); the data only loads what it is told.
struct CharacterSceneLook
{
    bool classicStage = false; // Logo05 from Data/Interface97/Logo/, Scenes::ClassicCharacter's yard
    bool classicFaces = false; // Face01..04 and face05 instead of NewFace01..07
};
void OpenCharacterSceneData(const CharacterSceneLook& look);
void ReleaseCharacterSceneData();
// The look OpenCharacterSceneData() loaded, the one place the engine and the windows read it from;
// all false once released.
const CharacterSceneLook& LoadedCharacterSceneLook();

void OpenPlayerTextures();
void OpenItemTextures();
// Makes mesh 2 of the Grand Soul pants' model the grid their cloth moves
// (OpenItems once the item models are open; the item editor again after their
// slot took other data).
void PrepareGrandSoulPantsClothMesh();

void SaveOptions();
void SaveMacro(const wchar_t* FileName);
void OpenMacro(const wchar_t* FileName);

extern wchar_t AbuseFilter[][20];
extern wchar_t AbuseNameFilter[][20];
extern int AbuseFilterNumber;
extern int AbuseNameFilterNumber;