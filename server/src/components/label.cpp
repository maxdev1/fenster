// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/label.hpp"
#include <libfenster/font/font_loader.hpp>
#include <libfenster/properties.hpp>

#include <cairo/cairo.h>
#include <sstream>

namespace fensterserver
{
	Label::Label()
	{
		Label::setFont(fenster::FontLoader::getDefault());
		graphics.resize(1, 1);
		viewModel = fenster::TextLayouter::initializeLayout();
	}

	void Label::setFont(fenster::Font* newFont)
	{
		font = newFont;
	}

	void Label::update()
	{
		auto cr = graphics.acquireContext();
		if (!cr)
			return;

		if (font)
		{
			fenster::TextLayouter::layout(cr, text.c_str(), font, fontSize,
			                              fenster::Rectangle(0, 0, bounds.width, bounds.height),
			                              alignment,
			                              viewModel, false);

			this->setPreferredSize(fenster::Dimension(viewModel->textBounds.width, viewModel->textBounds.height));
		}

		graphics.releaseContext();
	}

	void Label::paint()
	{
		clearSurface();

		auto bounds = getBounds();

		auto cr = graphics.acquireContext();
		if (!cr)
			return;

		if (font)
		{
			fenster::TextLayouter::layout(cr, text.c_str(), font, fontSize,
			                                             fenster::Rectangle(0, 0, bounds.width, bounds.height),
			                                             alignment,
			                                             viewModel, false);

			int yOffset = bounds.height / 2 - viewModel->textBounds.height / 2;

			int pos = 0;
			for (pos = 0; pos < viewModel->positions.size(); pos++)
			{
				auto g = viewModel->positions[pos];
				// cairo_save(cr);
				// cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(ARGB(100, 255, 255, 0)));
				// cairo_rectangle(cr, g.bounds.x, g.bounds.y, g.bounds.width, g.bounds.height);
				// cairo_stroke(cr);
				// cairo_restore(cr);

				cairo_save(cr);
				cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(color));
				fenster::TextLayouter::paintChar(cr, g, fenster::Point(0, yOffset));
				cairo_restore(cr);
			}
		}

		// cairo_set_source_rgba(cr, ARGB_TO_FPARAMS(ARGB(200, 0, 100, 50)));
		// cairo_rectangle(cr, 0, 0, bounds.width, bounds.height);
		// cairo_stroke(cr);

		graphics.releaseContext();
	}

	void Label::setTitleInternal(std::string newText)
	{
		text = newText;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	std::string Label::getTitle()
	{
		return text;
	}

	void Label::setAlignment(fenster::TextAlignment newAlignment)
	{
		alignment = newAlignment;
		markFor(COMPONENT_REQUIREMENT_UPDATE);
	}

	fenster::TextAlignment Label::getAlignment()
	{
		return alignment;
	}

	fenster::ColorArgb Label::getColor()
	{
		return color;
	}

	void Label::setColor(fenster::ColorArgb color)
	{
		this->color = color;
		markFor(COMPONENT_REQUIREMENT_PAINT);
	}

	bool Label::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if (property == fenster::ComponentProperty::Color)
		{
			this->setColor(value);
			return true;
		}

		if (property == fenster::ComponentProperty::Alignment)
		{
			this->setAlignment((fenster::TextAlignment) value);
			return true;
		}

		return Component::setNumericProperty(property, value);
	}
}
