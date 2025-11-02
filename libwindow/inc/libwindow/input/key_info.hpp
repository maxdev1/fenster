// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWINDOW_INPUT_KEYINFO
#define LIBWINDOW_INPUT_KEYINFO

#include <string>

/**
 * Reduced key info struct that is used for example in events.
 */
struct key_info_basic_t
{
	bool pressed : 1;
	bool ctrl : 1;
	bool alt : 1;
	bool shift : 1;
	uint16_t scancode;

	key_info_basic_t() : pressed(false), ctrl(false), alt(false), shift(false), scancode(0)
	{
	}
} __attribute__((packed));

/**
 * Key info struct that must be constructed by the platform based on user input
 * so the window server can use it easily.
 */
class key_info_t : public key_info_basic_t
{
public:
	std::string key;

	key_info_t() : key("KEY_NONE")
	{
	}
};

#endif
