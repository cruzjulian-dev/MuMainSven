#include "Render/RmlUi/RmlDigitStripDecorator.h"

#include <RmlUi/Core/ComputedValues.h>
#include <RmlUi/Core/Decorator.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementText.h>
#include <RmlUi/Core/Factory.h>
#include <RmlUi/Core/Geometry.h>
#include <RmlUi/Core/Mesh.h>
#include <RmlUi/Core/MeshUtilities.h>
#include <RmlUi/Core/PropertyDefinition.h>
#include <RmlUi/Core/PropertyDictionary.h>
#include <RmlUi/Core/RenderManager.h>

namespace
{
// RenderNumber()'s quad: 12 x 16 at scale 1, the next digit 0.8 of a width further on.
constexpr float DigitWidthPerHeight = 12.f / 16.f;
constexpr float DigitAdvancePerWidth = 0.8f;

// Where a digit's window is in the strip, in texels.
struct DigitStrip
{
    float cellWidth = 0.f;
    float glyphX = 0.f;
    float glyphY = 0.f;
    float glyphWidth = 0.f;
    float glyphHeight = 0.f;
};

// The geometry of one element's run, rebuilt when its text changes.
struct DigitRunData
{
    Rml::Geometry geometry;
    Rml::String text;
};

const Rml::String& TextOf(Rml::Element* element)
{
    static const Rml::String noText;
    if (element->GetNumChildren() == 0)
        return noText;
    const auto* text = rmlui_dynamic_cast<Rml::ElementText*>(element->GetChild(0));
    return text != nullptr ? text->GetText() : noText;
}

// The run's left edge: RenderNumber() put it half of the digits' full widths left of the centre.
float RunLeft(Rml::Style::TextAlign align, float contentLeft, float contentWidth, float digitWidth, int length)
{
    const float runWidth = digitWidth * DigitAdvancePerWidth * static_cast<float>(length - 1) + digitWidth;
    switch (align)
    {
    case Rml::Style::TextAlign::Right:
        return contentLeft + contentWidth - runWidth;
    case Rml::Style::TextAlign::Center:
        return contentLeft + contentWidth * 0.5f - digitWidth * static_cast<float>(length) * 0.5f;
    case Rml::Style::TextAlign::Left:
    case Rml::Style::TextAlign::Justify:
        break;
    }
    return contentLeft;
}

class DigitStripDecorator final : public Rml::Decorator
{
public:
    DigitStripDecorator(Rml::Texture texture, const DigitStrip& strip, Rml::NumericValue height)
        : m_strip(strip), m_height(height)
    {
        AddTexture(texture);
    }

    Rml::DecoratorDataHandle GenerateElementData(Rml::Element* element, Rml::BoxArea) const override
    {
        auto* data = new DigitRunData;
        data->text = TextOf(element);
        data->geometry = BuildRun(element, data->text);
        return reinterpret_cast<Rml::DecoratorDataHandle>(data);
    }

    void ReleaseElementData(Rml::DecoratorDataHandle element_data) const override
    {
        delete reinterpret_cast<DigitRunData*>(element_data);
    }

    void RenderElement(Rml::Element* element, Rml::DecoratorDataHandle element_data) const override
    {
        // A data binding changes the text without resizing the box, which would not regenerate the
        // data: check it here, rebuilding only when it differs.
        auto* data = reinterpret_cast<DigitRunData*>(element_data);
        const Rml::String& text = TextOf(element);
        if (text != data->text)
        {
            data->text = text;
            data->geometry = BuildRun(element, text);
        }
        if (data->geometry)
            data->geometry.Render(element->GetAbsoluteOffset(Rml::BoxArea::Border), GetTexture());
    }

private:
    Rml::Geometry BuildRun(Rml::Element* element, const Rml::String& text) const
    {
        if (text.empty())
            return {};

        const Rml::Vector2i textureSize = GetTexture().GetDimensions();
        if (textureSize.x <= 0 || textureSize.y <= 0)
            return {};
        const Rml::Vector2f texel(1.f / static_cast<float>(textureSize.x), 1.f / static_cast<float>(textureSize.y));

        const float height = element->ResolveLength(m_height);
        const float width = height * DigitWidthPerHeight;
        const Rml::Vector2f size(width, height);
        const Rml::ComputedValues& computed = element->GetComputedValues();
        const Rml::ColourbPremultiplied colour = computed.image_color().ToPremultiplied(computed.opacity());

        const Rml::Box& box = element->GetBox();
        const Rml::Vector2f content = box.GetPosition(Rml::BoxArea::Content);
        Rml::Vector2f origin(RunLeft(computed.text_align(), content.x, box.GetSize(Rml::BoxArea::Content).x, width,
                                     static_cast<int>(text.size())),
                             content.y);

        Rml::Mesh mesh;
        for (const char character : text)
        {
            if (character >= '0' && character <= '9')
            {
                const float u = static_cast<float>(character - '0') * m_strip.cellWidth + m_strip.glyphX;
                const Rml::Vector2f topLeft(u * texel.x, m_strip.glyphY * texel.y);
                const Rml::Vector2f bottomRight((u + m_strip.glyphWidth) * texel.x,
                                                (m_strip.glyphY + m_strip.glyphHeight) * texel.y);
                Rml::MeshUtilities::GenerateQuad(mesh, origin, size, colour, topLeft, bottomRight);
            }
            origin.x += width * DigitAdvancePerWidth;
        }
        if (mesh.indices.empty())
            return {};
        return element->GetRenderManager()->MakeGeometry(std::move(mesh));
    }

    DigitStrip m_strip;
    Rml::NumericValue m_height;
};

class DigitStripDecoratorInstancer final : public Rml::DecoratorInstancer
{
public:
    DigitStripDecoratorInstancer()
    {
        m_imageId = RegisterProperty("image", "").AddParser("string").GetId();
        m_cellWidthId = RegisterProperty("cell-width", "0").AddParser("number").GetId();
        m_glyphXId = RegisterProperty("glyph-x", "0").AddParser("number").GetId();
        m_glyphYId = RegisterProperty("glyph-y", "0").AddParser("number").GetId();
        m_glyphWidthId = RegisterProperty("glyph-width", "0").AddParser("number").GetId();
        m_glyphHeightId = RegisterProperty("glyph-height", "0").AddParser("number").GetId();
        m_heightId = RegisterProperty("height", "0px").AddParser("length").GetId();
        RegisterShorthand("decorator", "image, cell-width, glyph-x, glyph-y, glyph-width, glyph-height, height",
                          Rml::ShorthandType::FallThrough);
    }

    Rml::SharedPtr<Rml::Decorator>
    InstanceDecorator(const Rml::String&, const Rml::PropertyDictionary& properties,
                      const Rml::DecoratorInstancerInterface& instancer_interface) override
    {
        const Rml::Property* image = properties.GetProperty(m_imageId);
        if (image == nullptr)
            return nullptr;
        const Rml::Texture texture = instancer_interface.GetTexture(image->Get<Rml::String>());
        if (!texture)
            return nullptr;

        const auto number = [&properties](Rml::PropertyId id) { return properties.GetProperty(id)->Get<float>(); };
        const DigitStrip strip{number(m_cellWidthId), number(m_glyphXId), number(m_glyphYId), number(m_glyphWidthId),
                               number(m_glyphHeightId)};
        if (strip.glyphWidth <= 0.f || strip.glyphHeight <= 0.f)
            return nullptr;
        return Rml::MakeShared<DigitStripDecorator>(texture, strip,
                                                    properties.GetProperty(m_heightId)->GetNumericValue());
    }

private:
    Rml::PropertyId m_imageId{};
    Rml::PropertyId m_cellWidthId{};
    Rml::PropertyId m_glyphXId{};
    Rml::PropertyId m_glyphYId{};
    Rml::PropertyId m_glyphWidthId{};
    Rml::PropertyId m_glyphHeightId{};
    Rml::PropertyId m_heightId{};
};
} // namespace

void Render::RmlUi::RegisterDigitStripDecorator()
{
    static DigitStripDecoratorInstancer instancer;
    Rml::Factory::RegisterDecoratorInstancer("digit-strip", &instancer);
}
