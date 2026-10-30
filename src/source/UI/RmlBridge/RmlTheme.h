#pragma once

#include <functional>
#include <string>
#include <vector>

namespace Rml
{
    class Context;
    class ElementDocument;
}

// Which RmlUi visual theme is active. A theme is identified purely by NAME (a folder), not
// hardcoded in C++: just Data/Interface/RmlUi/themes/<name>/<document>.rcss, no manifest file
// needed. This is deliberate: it's what makes a theme (including one a modder drops in, not just
// "legacy"/"modern") a plug-in-a-folder operation with zero source changes or recompilation.
//
// Every migrated window renders its own visuals entirely through RmlUi -- CWin (and any other
// legacy widget) never draws background/frame art for a migrated window, in any theme. A
// "legacy-look" theme reproduces the original art by pointing its own RCSS decorators at the same
// image files the old sprites used (e.g. themes/legacy/login.rcss's `decorator: image(...)`) --
// that's a choice of asset, not a choice of renderer.
//
// Seeded once from GameConfig::GetRmlTheme() at startup, then live-mutable via SetActiveThemeName()
// (e.g. the `$theme <name>` chat command) -- GameConfig's own value only changes what a *relaunch*
// picks up; the cache here is the actual "what's on screen right now" source of truth. Changing it
// alone does nothing visually: a window's Rml::ElementDocument/DataModel was already built against
// whatever theme was active when it was loaded, so a caller that wants the change to be visible
// must also call ReloadAllThemedDocuments() (below) after SetActiveThemeName().
namespace UI::RmlBridge
{
    // Cached on first call from GameConfig::GetRmlTheme() (e.g. "legacy", "modern", or any
    // modder-supplied folder name -- not a closed set). Reflects the live cache SetActiveThemeName()
    // writes to, not necessarily GameConfig's current value.
    const std::string& GetActiveThemeName();

    // Validates a saved selection; falls back to legacy, or returns empty if neither is usable.
    std::string ResolveStartupThemeName(const std::string& savedName);

    // Overwrites the live active-theme cache GetActiveThemeName() reads
    // from (lowercased, same normalization GetActiveThemeName() has always applied). Does not touch
    // GameConfig and does not rebuild any window's document by itself -- see this file's top comment.
    // Does clear RmlUi's own template cache (Rml::Factory::ClearTemplateCache()), since two themes'
    // same-named <template> forks (e.g. "window_shell_bg") would otherwise leave that name resolving
    // to whichever theme's copy was most recently freshly loaded, not necessarily the one now active.
    void SetActiveThemeName(const std::string& themeName);

    // True if the inheritance chain is valid and base.rcss exists and is readable in it -- the minimum a folder needs to
    // be a real theme (every window's document links it first). Meant as a pre-switch validation
    // gate (e.g. for the `$theme` command) so an unknown/misspelled name is rejected up front
    // instead of silently rendering every window unstyled.
    bool ThemeExists(const std::string& themeName);

    // Every valid theme (ThemeExists()) found directly under Data/Interface/RmlUi/themes/, sorted
    // alphabetically by folder name for deterministic, stable ordering across calls (a
    // std::filesystem::directory_iterator's own enumeration order is unspecified). A folder with
    // no base.rcss in its inheritance chain is skipped. This is what lets a newly
    // installed theme folder (including a modder's own) become selectable in COptionWindow's UI
    // Theme dropdown with zero C++ changes; see this file's own top comment.
    std::vector<std::string> DiscoverAvailableThemes();

    // themes/<themeName>/theme.ini's optional [Meta] DisplayName, e.g. "Legacy"/"Modern" for the
    // two bundled themes. Falls back to `themeName` with its first letter capitalized if the file
    // or key is missing -- the safe default for a theme that hasn't declared one, including a
    // modder-supplied theme with no theme.ini at all.
    std::string GetThemeDisplayName(const std::string& themeName);

    // Declared theme capabilities: an optional themes/<name>/theme.ini [Capabilities] section,
    // keys inherit from ancestors, then default to false. C++ never branches on a theme's name.
    // Whether the active theme sizes opted-in documents' text like the original client's
    // (NativeTextSize=1) -- see RmlNativeText.h.
    bool ThemeUsesNativeTextSize();
    // Whether the active theme wants its images drawn with nearest instead of linear filtering
    // (NearestFiltering=1), for pixel art that should stay sharp when the UI is scaled.
    bool ThemeUsesNearestFiltering();
    // Whether the chat log has the 0.97k client's two modes (ChatLogModes=1): the history without
    // its frame and the box with it. F5 switches them and F4 cycles the current one's height
    // whether or not the player types, each mode keeps its own height, the box stays up when
    // typing ends, F2 shows or hides the chat, and while nobody types the chat's buttons sit over
    // the closed input box (chat_frame_buttons.rml).
    bool ThemeHasChatLogModes();
    // Whether whispering starts switched off (WhisperStartsOff=1), as in the 0.97k client: F3
    // switches it on, and until then the recipient field takes no typing.
    bool ThemeWhisperStartsOff();
    // Whether the world labels are the 0.97k client's (ClassicWorldHud=1): a party member's "HP"
    // hover text in the tooltip box above the bar (RenderTipText()), and no health bar under the
    // selected monster's name.
    bool ThemeHasClassicWorldHud();
    // Whether the login scene is the 0.97k client's (ClassicLoginScene=1): the ships at sea under
    // the 3D MU logo, in a 640x320 strip between black bars, instead of the login world. Read when
    // the scene is created, so a theme switch shows on the next visit.
    bool ThemeHasClassicLoginScene();
    // Whether character creation is the 0.97k client's (ClassicCharacterCreation=1): the class's
    // face from Data/Logo/Face0N.bmd, picked with arrows among the dark wizard, dark knight, elf,
    // magic gladiator and dark lord. The faces are loaded with the character scene, so a theme
    // switch shows on the next visit.
    bool ThemeHasClassicCharacterCreation();
    // Whether the character scene is the 0.97k client's (ClassicCharacterScene=1): the characters on
    // the stone yard of Data/Interface97/Logo/Logo05.bmd, seen from that client's camera, instead of
    // the character world. Read when the scene is created, so a theme switch shows on the next visit.
    bool ThemeHasClassicCharacterScene();
    // Whether the damage and "Miss" over the characters are drawn from Data/Interface97/Fonttest
    // (ClassicHitNumbers=1) instead of Data/Interface/FontTest. The main scene reads it every
    // frame, so a theme switch shows at once.
    bool ThemeHasClassicHitNumbers();

    // Builds the virtual source URL a themed document should be loaded against, e.g.
    // "Data/Interface/RmlUi/themes/modern/login.rml" -- this path need not exist on disk (the RML
    // itself is loaded from memory). Relative links retain the selected theme's path even when
    // files come from an ancestor, so child overrides apply to inherited markup.
    std::string ThemedDocumentSourceUrl(const char* documentName, const std::string& themeName);

    class ThemedDocuments;

    // Loading a document against the active theme belongs to ThemedView (RmlThemedView.h): only
    // ThemedDocuments may call this, so no window can load a themed document no view owns.
    class ThemedDocumentLoader
    {
        friend class ThemedDocuments;

        // Reads `documentPath` (e.g. "Data/Interface/RmlUi/login.rml") and instantiates it against
        // the active theme's stylesheet via LoadDocumentFromMemory, so "add a new theme" stays a
        // drop-a-folder-of-RCSS operation. Sets the document's stacking depth and scene (below).
        // Returns nullptr if the file couldn't be read or the document failed to parse (logged via
        // g_ErrorReport either way).
        static Rml::ElementDocument* Load(Rml::Context* context, const char* documentPath);

        // Load() for a document instantiated once per window (the friends family's chat rooms and
        // letters): every occurrence of `modelPlaceholder` in the markup becomes `modelName`, so
        // each instance binds its own data model.
        static Rml::ElementDocument* Load(Rml::Context* context, const char* documentPath,
                                          const std::string& modelPlaceholder, const std::string& modelName);
    };

    // Sets the original's layer depth of `documentName` (e.g. "loading.rml", RmlStackingOrder.h)
    // as the document's z-index. Every themed document gets it when it loads; call it only for a
    // document loaded some other way.
    void ApplyStackingDepth(Rml::ElementDocument* document, const std::string& documentName);

    // Scene gate (RmlStackingOrder.h, DocumentScene). The original drew CNewUIManager's windows
    // only while the main scene ran; the main-scene windows sync their documents from their own
    // Update(), which stops with the main scene, so a document open at logout kept rendering over
    // character selection. Outside the main scene every main-scene document is suspended
    // (display: none -- never drawn, never hit; its own Show()/Hide() state is kept, and a Show()
    // while suspended has no effect).
    //
    // Marks `documentName`'s document as a main-scene one when the stacking table says so; every
    // themed document gets it when it loads.
    void ApplyDocumentScene(Rml::ElementDocument* document, const std::string& documentName);
    // Every frame before RmlUi updates and renders (RmlUiRuntime::Update()): outside the main
    // scene, suspends every marked document, including one loaded since (a theme switch).
    void SuspendMainSceneDocumentsOutsideMainScene();
    // At the start of CSystem::Update(), before the windows sync their documents: lifts the
    // suspension, so a window closed meanwhile hides its document in the same frame instead of
    // showing it for one.
    void ResumeMainSceneDocuments();
    // Every frame before RmlUi updates: a DocumentScene::Every document takes its depth for the
    // running scene.
    void ApplySceneStackingDepths();

    // Following theme switches belongs to ThemedView too: only ThemedDocuments may register.
    class ThemeReloadRegistry
    {
    public:
        using Callback = std::function<void()>;
        // The owner's document, for where it sits among its context's documents; null while it has
        // none.
        using Document = std::function<Rml::ElementDocument*()>;

    private:
        friend class ThemedDocuments;

        // Registers or replaces `owner`'s callback. A reloaded document goes on top of its context,
        // so the owners that name their document reload from the bottom of its context's stack up,
        // and the stacking survives a theme switch. Owners without one keep registration order.
        static void Register(const void* owner, Callback callback, Document document = {});
        // A no-op if `owner` isn't registered.
        static void Unregister(const void* owner);
    };

    // Rebuilds every themed document against whatever SetActiveThemeName() most recently set: the
    // one call a theme switch needs after SetActiveThemeName(). Works on a copy of the registry, so
    // an owner registering or unregistering another during the sweep is safe.
    void ReloadAllThemedDocuments();

    // A [UI] Font change: records the family for the font tokens (SetSelectedFontFamily()), drops
    // the cached stylesheets that still carry the previous family, and rebuilds every themed
    // document. Destroys documents, so it must run outside RmlUi's event dispatch, like a theme switch.
    void ApplySelectedFontFamily(const std::string& family);

    // The family the native text renderer is asked for: the [UI] Font choice, or the active
    // theme's font-native token on the Default choice. ResolveUiFont() (SystemFonts.h) turns it
    // into the font actually drawn.
    std::string GetNativeFontFamily();
}
