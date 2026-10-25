#pragma once

namespace Render::RmlUi
{
// Registers `decorator: digit-strip(<image> <cell-width> <glyph-x> <glyph-y> <glyph-width>
// <glyph-height> <height>)`: draws the element's text, a run of digits, from a bitmap digit strip
// instead of the font, the way the original RenderNumber() drew newui_number1/Font97. Digit d is the
// <glyph-width> x <glyph-height> texel window at (d * <cell-width> + <glyph-x>, <glyph-y>); each is
// drawn <height> tall, three quarters of that wide, a fifth of its width overlapping the next. The
// run is tinted by image-color and opacity and sits at the top of the content box, placed by
// text-align (centred the way RenderNumber() centred it). A character that is not a digit keeps its
// place empty. The theme hides the text itself (color: transparent). Call once after
// Rml::Initialise().
void RegisterDigitStripDecorator();
} // namespace Render::RmlUi
