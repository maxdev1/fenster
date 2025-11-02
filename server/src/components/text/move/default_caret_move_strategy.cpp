// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/text/move/default_caret_move_strategy.hpp"
#include "components/text/text_component.hpp"

namespace fensterserver
{
	static DefaultCaretMoveStrategy* INSTANCE = nullptr;

	DefaultCaretMoveStrategy* DefaultCaretMoveStrategy::getInstance()
	{
		if(INSTANCE == 0)
		{
			INSTANCE = new DefaultCaretMoveStrategy();
		}
		return INSTANCE;
	}

	void DefaultCaretMoveStrategy::moveCaret(TextComponent* component, CaretDirection direction, fenster::KeyInfo& info)
	{

		int cursor = component->getCursor();
		int newCursorPosition = cursor;
		int selectedLength = component->getSelectedRange().getLength();

		if(direction == CaretDirection::RIGHT)
		{
			if(info.ctrl)
			{
				newCursorPosition = component->getText().length();
			}
			else
			{
				newCursorPosition = info.alt ? calculateSkip(component->getText(), cursor, CaretDirection::RIGHT) : (cursor + 1);
			}
		}
		else if(direction == CaretDirection::LEFT)
		{
			if(info.ctrl)
			{
				newCursorPosition = 0;
			}
			else
			{
				newCursorPosition = info.alt ? calculateSkip(component->getText(), cursor, CaretDirection::LEFT) : (cursor - 1);
			}
		}

		component->setCursor(newCursorPosition);
		if(!info.shift)
		{
			component->setMarker(newCursorPosition);
		}
	}

	int DefaultCaretMoveStrategy::calculateSkip(std::string text, int position, CaretDirection direction)
	{

		bool l = (direction == CaretDirection::LEFT);

		if(l ? (position > 0) : (position < text.length()))
		{
			bool inFirst = true;
			if(!l)
			{
				char c = text[position];
				if(!(c == ' ' || c == ',' || c == '.'))
				{
					inFirst = false;
				}
			}

			l ? --position : ++position;

			while(l ? position > 0 : position < text.length())
			{
				char c = text[position];
				char p = text[l ? position - 1 : position];

				if(inFirst && (c == ' ' || c == ',' || c == '.'))
				{
					l ? --position : ++position;
				}
				else if((!l || !(p == ' ' || p == ',' || p == '.')) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
				{
					l ? --position : ++position;
					inFirst = false;
				}
				else
				{
					break;
				}
			}
		}

		return position;
	}
}
