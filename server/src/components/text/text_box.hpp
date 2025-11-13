// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_TEXTBOX
#define FENSTER_SERVER_COMPONENTS_TEXTBOX

#include "components/text/text_component.hpp"
#include "components/titled_component.hpp"

#include <libfenster/font/font.hpp>
#include <libfenster/font/text_layouter.hpp>
#include <libfenster/metrics/insets.hpp>
#include <libfenster/color_argb.hpp>
#include <string>

namespace fensterserver
{
	enum class TextBoxVisualStatus : uint8_t
	{
		NORMAL,
		HOVERED
	};

	class TextBox : virtual public TextComponent, virtual public TitledComponent
	{
		std::string text;
		TextBoxVisualStatus visualStatus = TextBoxVisualStatus::NORMAL;
		bool secure = false;
		bool multiline = false;
		bool shiftDown = false;

		fenster::Font* font = nullptr;
		int fontSize = 14;

		fenster::ColorArgb textColor = _RGB(0, 0, 0);
		fenster::Insets insets = fenster::Insets(5, 5, 5, 5);

		int scrollX = 0;
		int scrollY = 0;
		int cursor = 0;
		int marker = 0;
		fenster::LayoutedText* viewModel;

		bool scrollToCursorBeforePaint = false;

		void scrollToCursor();

	public:
		TextBox();

		~TextBox() override = default;

		void update() override;

		void paint() override;

		void paintBackground(cairo_t* cr);

		void layoutText(cairo_t* cr);

		void paintMarking(cairo_t* cr);

		void paintText(cairo_t* cr);

		void paintCursor(cairo_t* cr);

		void limitScroll();

		Component* handleKeyEvent(KeyEvent& e) override;

		Component* handleMouseEvent(MouseEvent& e) override;

		bool getNumericProperty(fenster::ComponentProperty property, uint32_t* out) override;

		bool setNumericProperty(fenster::ComponentProperty property, uint32_t value) override;

		void setText(std::string text) override;

		std::string getText() override;

		bool isFocusableDefault() const override
		{
			return true;
		}

		void setFocusedInternal(bool focused) override;

		virtual void setSecure(bool secure);

		virtual bool isSecure()
		{
			return secure;
		}

		virtual void setMultiLine(bool multiline);

		virtual bool isMultiLine()
		{
			return multiline;
		}

		void setTitleInternal(std::string title) override
		{
			setText(title);
		}

		std::string getTitle() override
		{
			return getText();
		}

		void setCursor(int pos) override;

		int getCursor() override
		{
			return cursor;
		}

		void setMarker(int pos) override;

		int getMarker() override
		{
			return marker;
		}

		fenster::Range getSelectedRange() override;

		void backspace(fenster::KeyInfo& info);

		void insert(std::string text);

		int viewToPosition(fenster::Point p);

		fenster::Point positionToUnscrolledCursorPoint(int pos);

		fenster::Rectangle positionToCursorBounds(int pos);

		void setFont(fenster::Font* f);
	};
}

#endif
