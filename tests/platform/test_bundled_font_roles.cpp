#include <filesystem>
#include <iterator>
#include <string>
#include <string_view>

#include <doctest.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Core/Platform/BundledFonts.h"
#include "Core/Platform/SystemFonts.h"
#include "UI/Scaling/UITransform.h"

TEST_CASE("bundled font roles resolve deterministically [platform][bundled_font]")
{
    CHECK(ResolveBundledFont("").family == std::string_view("DejaVu Sans"));
    CHECK(ResolveBundledFont("unknown").family == std::string_view("DejaVu Sans"));
    CHECK(ResolveBundledFont("Liberation Sans").regular ==
          std::string_view("fonts/LiberationSans-Regular.ttf"));
    CHECK(ResolveBundledFont("Liberation Sans").bold ==
          std::string_view("fonts/LiberationSans-Bold.ttf"));
    CHECK(std::string_view(kBundledFixedFont.family) == "Cousine");
    CHECK(std::string_view(kBundledFixedFont.regular) == "fonts/Cousine-Regular.ttf");
    CHECK(ResolveBundledFont("Noto Sans TC").regular == std::string_view("fonts/NotoSansTC-Regular.otf"));
    CHECK(ResolveBundledFont("Noto Sans TC").bold == std::string_view("fonts/NotoSansTC-Bold.otf"));
    REQUIRE(std::size(kBundledFallbackFonts) == 3);
    CHECK(std::string_view(kBundledFallbackFonts[0].family) == "DejaVu Sans");
    CHECK(std::string_view(kBundledFallbackFonts[1].family) == "Noto Sans TC");
    CHECK(std::string_view(kBundledFallbackFonts[2].family) == "Nanum Gothic");
    CHECK(std::string_view(kBundledFallbackFonts[2].regular) == "fonts/NanumGothic-Regular.ttf");
}

TEST_CASE("a system family draws from the installed face or the bundled default [platform][bundled_font]")
{
    const std::string family = ResolveUiFontFamily("Verdana");
    if (const SystemFontFace* face = FindSystemFontFace("Verdana", false, false))
    {
        CHECK(family == "Verdana");
        CHECK_FALSE(face->data.empty());
        CHECK_FALSE(face->syntheticBold);
        CHECK(FindSystemFontFace("Verdana", true, true)->data != face->data);
    }
    else
    {
        CHECK(family == "DejaVu Sans");
    }
    CHECK(ResolveUiFontFamily("Missing Test Family") == "DejaVu Sans");
    CHECK(ResolveUiFontFamily("") == "DejaVu Sans");
    CHECK(ResolveUiFontFamily("Liberation Sans") == "Liberation Sans");
}

TEST_CASE("a cell height gives a smaller em [platform][bundled_font]")
{
    const int em = FontEmPixels("DejaVu Sans", 13);
    CHECK(em >= 1);
    CHECK(em <= 13);
    CHECK(FontEmPixels("DejaVu Sans", 26) > em);
}

TEST_CASE("bundled font paths follow the normalized runtime directory [platform][bundled_font]")
{
    const std::filesystem::path relativePath = "fonts/DejaVuSans.ttf";
    CHECK(ResolveBundledFontPath(relativePath) == std::filesystem::current_path() / relativePath);
}

TEST_CASE("bundled font roles load their maximum responsive size [platform][bundled_font]")
{
    using UI::Scaling::FontRole;
    CHECK(UI::Scaling::MaximumFontPointSize(FontRole::Normal) == 16);
    CHECK(UI::Scaling::MaximumFontPointSize(FontRole::Bold) == 16);
    CHECK(UI::Scaling::MaximumFontPointSize(FontRole::Big) == 32);
    CHECK(UI::Scaling::MaximumFontPointSize(FontRole::Fixed) == 18);
}

TEST_CASE("bundled fallback font covers Hangul and Latin accents [platform][bundled_font]")
{
    REQUIRE(TTF_Init());
    const std::filesystem::path fallbackPath =
        std::filesystem::path(MU_TEST_ASSET_SOURCE) / kBundledFallbackFonts[2].regular;
    const std::filesystem::path basePath =
        std::filesystem::path(MU_TEST_ASSET_SOURCE) / ResolveBundledFont("DejaVu Sans").regular;
    TTF_Font* base = TTF_OpenFont(basePath.string().c_str(), 16.0f);
    TTF_Font* fallback = TTF_OpenFont(fallbackPath.string().c_str(), 16.0f);
    REQUIRE(base != nullptr);
    REQUIRE(fallback != nullptr);

    CHECK_FALSE(TTF_FontHasGlyph(base, 0xD55C)); // 한
    REQUIRE(TTF_AddFallbackFont(base, fallback));
    CHECK(TTF_FontHasGlyph(base, 0xD55C)); // 한
    CHECK(TTF_FontHasGlyph(base, 0xAE00)); // 글
    CHECK(TTF_FontHasGlyph(base, 0x00E1)); // á
    CHECK(TTF_FontHasGlyph(base, 0x00E3)); // ã

    TTF_CloseFont(base);
    TTF_CloseFont(fallback);
    TTF_Quit();
}
