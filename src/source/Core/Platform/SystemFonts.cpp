#include "stdafx.h"
#include "Core/Platform/SystemFonts.h"

#include "Core/Utilities/StringUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <tuple>

namespace
{
#ifdef _WIN32
constexpr DWORD kCollectionTable = 0x66637474; // 'ttcf', read little-endian
constexpr DWORD kOs2Table = 0x322F534F;        // 'OS/2'
constexpr DWORD kOs2SelectionOffset = 62;
constexpr int kSelectionItalicBit = 1 << 0;
constexpr int kSelectionBoldBit = 1 << 5;
constexpr int kSelectionObliqueBit = 1 << 9;
constexpr size_t kSfntHeaderSize = 12;
constexpr size_t kSfntTableRecordSize = 16;
constexpr size_t kCollectionOffsetsAt = 12;

// A font selected into a memory DC for as long as the object lives.
class ScopedGdiFont
{
public:
    ScopedGdiFont(const std::wstring& family, int cellHeight, bool bold, bool italic)
        : m_dc(CreateCompatibleDC(nullptr)),
          m_font(CreateFontW(cellHeight, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic, 0, 0, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, family.c_str())),
          m_previous(m_dc && m_font ? SelectObject(m_dc, m_font) : nullptr)
    {
    }

    ~ScopedGdiFont()
    {
        if (m_previous)
            SelectObject(m_dc, m_previous);
        if (m_font)
            DeleteObject(m_font);
        if (m_dc)
            DeleteDC(m_dc);
    }

    ScopedGdiFont(const ScopedGdiFont&) = delete;
    ScopedGdiFont& operator=(const ScopedGdiFont&) = delete;

    [[nodiscard]] HDC Dc() const { return m_previous ? m_dc : nullptr; }

private:
    HDC m_dc;
    HFONT m_font;
    HGDIOBJ m_previous;
};

// GDI silently substitutes a family it does not have.
bool SelectedFamilyIs(HDC dc, const std::wstring& family)
{
    wchar_t face[LF_FACESIZE] = {};
    return GetTextFaceW(dc, LF_FACESIZE, face) > 0 && _wcsicmp(face, family.c_str()) == 0;
}

std::vector<unsigned char> ReadFontTable(HDC dc, DWORD table, DWORD offset = 0)
{
    const DWORD size = GetFontData(dc, table, offset, nullptr, 0);
    if (size == GDI_ERROR || size == 0)
        return {};
    std::vector<unsigned char> bytes(size);
    if (GetFontData(dc, table, offset, bytes.data(), size) == GDI_ERROR)
        return {};
    return bytes;
}

std::uint32_t ReadBigEndian32(const unsigned char* bytes)
{
    return (static_cast<std::uint32_t>(bytes[0]) << 24) | (static_cast<std::uint32_t>(bytes[1]) << 16) |
           (static_cast<std::uint32_t>(bytes[2]) << 8) | static_cast<std::uint32_t>(bytes[3]);
}

// A collection holds a directory per face; GDI hands out the selected face's own, so the
// matching one gives its index.
int FindFaceInCollection(HDC dc, const std::vector<unsigned char>& collection)
{
    unsigned char header[kSfntHeaderSize] = {};
    if (GetFontData(dc, 0, 0, header, kSfntHeaderSize) == GDI_ERROR ||
        collection.size() < kCollectionOffsetsAt)
        return 0;

    const size_t tableCount = (static_cast<size_t>(header[4]) << 8) | header[5];
    const size_t directorySize = kSfntHeaderSize + tableCount * kSfntTableRecordSize;
    std::vector<unsigned char> directory(directorySize);
    if (GetFontData(dc, 0, 0, directory.data(), static_cast<DWORD>(directorySize)) == GDI_ERROR)
        return 0;

    const std::uint32_t faceCount = ReadBigEndian32(collection.data() + 8);
    for (std::uint32_t face = 0; face < faceCount; ++face)
    {
        const size_t entry = kCollectionOffsetsAt + face * sizeof(std::uint32_t);
        if (entry + sizeof(std::uint32_t) > collection.size())
            break;
        const size_t offset = ReadBigEndian32(collection.data() + entry);
        if (offset + directorySize <= collection.size() &&
            std::memcmp(collection.data() + offset, directory.data(), directorySize) == 0)
            return static_cast<int>(face);
    }
    return 0;
}

int ReadFaceSelection(HDC dc)
{
    unsigned char selection[2] = {};
    if (GetFontData(dc, kOs2Table, kOs2SelectionOffset, selection, sizeof(selection)) == GDI_ERROR)
        return 0;
    return (selection[0] << 8) | selection[1];
}

std::unique_ptr<SystemFontFace> LoadFace(const std::wstring& family, bool bold, bool italic)
{
    constexpr int probeCellHeight = 16;
    ScopedGdiFont font(family, probeCellHeight, bold, italic);
    const HDC dc = font.Dc();
    if (!dc || !SelectedFamilyIs(dc, family))
        return nullptr;

    auto face = std::make_unique<SystemFontFace>();
    face->data = ReadFontTable(dc, kCollectionTable);
    if (face->data.empty())
        face->data = ReadFontTable(dc, 0);
    else
        face->faceIndex = FindFaceInCollection(dc, face->data);
    if (face->data.empty())
        return nullptr;

    const int selection = ReadFaceSelection(dc);
    face->syntheticBold = bold && (selection & kSelectionBoldBit) == 0;
    face->syntheticItalic = italic && (selection & (kSelectionItalicBit | kSelectionObliqueBit)) == 0;
    return face;
}

int CALLBACK CollectFamily(const LOGFONTW* font, const TEXTMETRICW*, DWORD fontType, LPARAM families)
{
    // Bitmap faces have no file SDL_ttf reads; an '@' name is the vertical copy of a CJK family.
    if ((fontType & RASTER_FONTTYPE) == 0 && font->lfFaceName[0] != L'@')
        reinterpret_cast<std::set<std::string>*>(families)->insert(StringUtils::WideToNarrow(font->lfFaceName));
    return 1;
}
#endif

bool IsBundledFamily(std::string_view family)
{
    return family == ResolveBundledFont(family).family;
}
} // namespace

std::vector<std::string> EnumerateSystemFontFamilies()
{
#ifdef _WIN32
    std::set<std::string> families;
    LOGFONTW filter = {};
    filter.lfCharSet = DEFAULT_CHARSET;
    const HDC dc = CreateCompatibleDC(nullptr);
    if (dc)
    {
        EnumFontFamiliesExW(dc, &filter, CollectFamily, reinterpret_cast<LPARAM>(&families), 0);
        DeleteDC(dc);
    }
    return {families.begin(), families.end()};
#else
    return {};
#endif
}

const SystemFontFace* FindSystemFontFace(std::string_view family, bool bold, bool italic)
{
#ifdef _WIN32
    static std::map<std::tuple<std::string, bool, bool>, std::unique_ptr<SystemFontFace>> faces;
    const auto key = std::make_tuple(std::string(family), bold, italic);
    auto found = faces.find(key);
    if (found == faces.end())
        found = faces.emplace(key, LoadFace(StringUtils::NarrowToWide(std::get<0>(key)), bold, italic)).first;
    return found->second.get();
#else
    // No system font is read here: ResolveUiFontFamily() falls back to the bundled default.
    return nullptr;
#endif
}

std::string ResolveUiFontFamily(std::string_view configuredFamily)
{
    if (IsBundledFamily(configuredFamily) || FindSystemFontFace(configuredFamily, false, false))
        return std::string(configuredFamily);
    return ResolveBundledFont(configuredFamily).family;
}

int FontEmPixels(std::string_view family, int cellHeight)
{
#ifdef _WIN32
    ScopedGdiFont font(StringUtils::NarrowToWide(std::string(family)), cellHeight, false, false);
    TEXTMETRICW metrics = {};
    if (font.Dc() && GetTextMetricsW(font.Dc(), &metrics))
        return std::max(static_cast<int>(metrics.tmHeight - metrics.tmInternalLeading), 1);
#endif
    // DejaVu Sans' em against its cell, the bundled default every platform draws with.
    constexpr float kDefaultFamilyEmPerCell = 0.86f;
    return std::max(static_cast<int>(std::lround(static_cast<float>(cellHeight) * kDefaultFamilyEmPerCell)), 1);
}
