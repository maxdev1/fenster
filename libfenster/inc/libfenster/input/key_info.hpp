// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_INPUT_KEYINFO
#define LIBFENSTER_INPUT_KEYINFO

#include <string>

namespace fenster
{
	/**
	 * Reduced key info struct that is used for example in events.
	 */
	struct KeyInfoBasic
	{
		bool pressed : 1;
		bool ctrl : 1;
		bool alt : 1;
		bool shift : 1;
		uint16_t scancode;

		KeyInfoBasic() : pressed(false), ctrl(false), alt(false), shift(false), scancode(0)
		{
		}
	} __attribute__((packed));

	/**
	 * Key info struct that must be constructed by the platform based on user input
	 * so the window server can use it easily.
	 */
	class KeyInfo : public KeyInfoBasic
	{
	public:
		std::string key;

		KeyInfo() : key("KEY_NONE")
		{
		}
	};
}

#endif
