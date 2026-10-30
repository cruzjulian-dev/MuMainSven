
#include "stdafx.h"
#include "CharInfoBalloonMng.h"
#include "Character/CharMakeWin.h"

#include "CharInfoBalloon.h"
#include "ClassicNameTag.h"
#include "Core/Globals/_enum.h"
#include "Core/Utilities/StringUtils.h"
#include "Data/GameConfig/GameConfig.h"
#include "Render/RmlUi/RmlUiRuntime.h"
#include "Render/Textures/ZzzOpenglUtil.h"
#include "UI/RmlBridge/RmlDocumentVisibility.h"
#include "UI/RmlBridge/RmlTheme.h"
#include "UI/Core/SceneUICoordinator.h"
#include "UI/Scaling/UITransform.h"
#include <RmlUi/Core/ElementDocument.h>

// Replaces CUIMng's old `CCharInfoBalloonMng m_CharInfoBalloonMng;` member, same convention as
// g_CreditWin.
CCharInfoBalloonMng g_CharInfoBalloonMng;

namespace
{
// The height of a tag's line, which the name rises by: the player's font under ClassicFontControls.
float TagLineHeight()
{
    if (UI::Scaling::IsClassicFontActive())
        return static_cast<float>(GameConfig::GetInstance().GetFontHeight());
    return Character::ClassicNameTag::TextPx(WindowWidth);
}

float TagTextPx()
{
    if (UI::Scaling::IsClassicFontActive())
    {
        return UI::Scaling::NativeTextPixelSize(UI::Scaling::FontRole::Normal, static_cast<int>(WindowWidth),
                                                static_cast<int>(WindowHeight));
    }
    return TagLineHeight();
}

// Key char_info_balloon.rml compares against; the theme's .rcss styles each (.name-<key>).
const char* NameStatusKey(BalloonNameStatus status)
{
    switch (status)
    {
    case BalloonNameStatus::BlockedCharacter:
        return "blocked-character";
    case BalloonNameStatus::BlockedItems:
        return "blocked-items";
    case BalloonNameStatus::Operator:
        return "operator";
    case BalloonNameStatus::Normal:
        break;
    }
    return "normal";
}
}

CCharInfoBalloonMng::~CCharInfoBalloonMng()
{
    Release();
}

void CCharInfoBalloonMng::Release()
{
    if (!m_isInitialized)
        return;

    m_isInitialized = false;

    // See CLoginMainWin::PreRelease()'s identical comment -- this class isn't a CWin, so it never
    // had any shared-list sweep to rely on; Release() is called explicitly at every character-scene
    // exit point instead (CSceneUICoordinator::CreateLoginScene()/CreateMainScene()/Release()),
    // which is exactly the right place to hide the document too.
    m_RmlView.Hide();
}

//*****************************************************************************
// 함수 이름 : Create()
// 함수 설명 : 캐릭터 정보 풍선 매니저 생성.
//			   (캐릭터 선택씬에서 쓰임. 풍선 5개 생성.)
//*****************************************************************************
void CCharInfoBalloonMng::Create()
{
    for (std::size_t i = 0; i < kBalloonCount; ++i)
        m_charInfoBalloons[i].Create(&CharactersClient[i]);

    m_isInitialized = true;

    // Builds once; Create() re-runs on resolution change.
    m_RmlView.Ensure();

    CSceneUICoordinator::Instance().GetNewStyleMng().AddUIObj(mu::ui::window::INTERFACE_CHAR_INFO_BALLOON, this);
}

void CCharInfoBalloonMng::BindRmlModel(Rml::DataModelConstructor& c, BalloonListModel& model)
{
    model.balloons.resize(kBalloonCount);
    // Set before the document's first layout as well as every frame (SyncRmlModel()), so no tag is
    // ever laid out at a font size of 0.
    model.classicTextPx = TagTextPx();
    model.classicNameRise = Character::ClassicNameTag::NameRise(TagLineHeight(), WindowHeight);

    // See CCharMakeWin::BindRmlModel()'s comment on why this must re-run in full every
    // call, including on a theme switch -- no guard here.
    auto entry = c.RegisterStruct<BalloonEntry>();
    entry.RegisterMember("hidden", &BalloonEntry::hidden);
    entry.RegisterMember("screen_x", &BalloonEntry::screenX);
    entry.RegisterMember("screen_y", &BalloonEntry::screenY);
    entry.RegisterMember("name_status", &BalloonEntry::nameStatus);
    entry.RegisterMember("name", &BalloonEntry::name);
    entry.RegisterMember("guild", &BalloonEntry::guild);
    entry.RegisterMember("klass", &BalloonEntry::klass);
    c.RegisterArray<std::vector<BalloonEntry>>();

    c.Bind("balloons", &model.balloons);
    c.Bind("char_make_open", &model.charMakeOpen);
    c.Bind("classic_text_px", &model.classicTextPx);
    c.Bind("classic_name_rise", &model.classicNameRise);
}

//*****************************************************************************
// 함수 이름 : Render()
// 함수 설명 : 캐릭터 정보 풍선들 렌더.
//*****************************************************************************
bool CCharInfoBalloonMng::Render()
{
    if (!m_isInitialized)
        return true;

    // Each balloon.Render() call recomputes its own live world->screen projection (position only
    // now -- see CCharInfoBalloon's header comment); SyncRmlModel() then pushes the result, plus
    // the text/color SetInfo() already cached, into the shared RmlUi array. This runs every
    // frame, during the normal legacy-2D-content recording phase -- strictly before
    // RmlUiRuntime's SetPreSubmitCallback fires later the same frame, so the position is always
    // fresh by the time RmlUi actually renders it.
    for (auto& balloon : m_charInfoBalloons)
        balloon.Render();

    // The original drew the balloons before the scene's windows (CUIMng::Render()), so the
    // character-creation dialog, a message window and the system menu covered them while the
    // balloons stayed visible around them. The stacking table (RmlStackingOrder.cpp) keeps that
    // order: the balloon document sits under the scene windows' documents.
    UI::RmlBridge::SyncDocumentVisibility(m_RmlView.Document(), true);

    SyncRmlModel();
    return true;
}

//*****************************************************************************
// 함수 이름 : UpdateDisplay()
// 함수 설명 : 캐릭터 정보를 업데이트.
//*****************************************************************************
void CCharInfoBalloonMng::UpdateDisplay()
{
    if (!m_isInitialized)
        return;

    for (auto& balloon : m_charInfoBalloons)
        balloon.SetInfo();
}

void CCharInfoBalloonMng::SyncRmlModel()
{
    if (!m_RmlView.Document()) return;

    auto& balloons = m_RmlView.GetModel().balloons;
    for (std::size_t i = 0; i < kBalloonCount; ++i)
    {
        CCharInfoBalloon& balloon = m_charInfoBalloons[i];
        BalloonEntry& entry = balloons[i];

        entry.hidden = !balloon.IsShow();
        // CSprite::SetPosition() already subtracts the (59, 54) anchor offset internally when
        // computing what GetXPos()/GetYPos() return (Sprite.cpp: m_aScrCoord[LT].fX = nXCoord -
        // m_fDatumX) -- GetXPos()/GetYPos() already ARE the anchor-adjusted top-left corner, the
        // same thing an RmlUi element's left/top needs. Subtracting the offset again here (an
        // earlier version of this code did) double-applies it, shifting the balloon uniformly
        // off to the upper-left of every character instead of centered above it.
        entry.screenX = balloon.GetXPos();
        entry.screenY = balloon.GetYPos();
        entry.nameStatus = NameStatusKey(balloon.GetNameStatus());
        entry.name = StringUtils::WideToNarrow(balloon.GetName());
        entry.guild = StringUtils::WideToNarrow(balloon.GetGuildText());
        entry.klass = StringUtils::WideToNarrow(balloon.GetClassText());
    }
    m_RmlView.MarkDirty("balloons");

    BalloonListModel& model = m_RmlView.GetModel();
    if (model.charMakeOpen != g_CharMakeWin.IsVisible())
    {
        model.charMakeOpen = g_CharMakeWin.IsVisible();
        m_RmlView.MarkDirty("char_make_open");
    }

    const float textPx = TagTextPx();
    const float nameRise = Character::ClassicNameTag::NameRise(TagLineHeight(), WindowHeight);
    if (model.classicTextPx != textPx || model.classicNameRise != nameRise)
    {
        model.classicTextPx = textPx;
        model.classicNameRise = nameRise;
        m_RmlView.MarkDirty("classic_text_px");
        m_RmlView.MarkDirty("classic_name_rise");
    }
}
