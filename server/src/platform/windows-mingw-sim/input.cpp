// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _WIN32

#include "windows_mingw_sim.hpp"
#include <libfenster/input/key_info.hpp>
#include <libfenster/platform/platform.hpp>
#include <windows.h>

#include <map>
#include <string>
#include <cstdint>

struct KeyMod
{
	std::string key;
	uint8_t mods;
};

#define MOD_SHIFT (1 << 0)
#define MOD_ALT (1 << 1)
#define MOD_CTRL (1 << 2)

static std::map<uint8_t, std::string> VK_MAP = {
		{VK_ESCAPE, "KEY_ESC"},
		{'1', "KEY_1"},
		{'2', "KEY_2"},
		{'3', "KEY_3"},
		{'4', "KEY_4"},
		{'5', "KEY_5"},
		{'6', "KEY_6"},
		{'7', "KEY_7"},
		{'8', "KEY_8"},
		{'9', "KEY_9"},
		{'0', "KEY_0"},
		{VK_OEM_7, "KEY_APOSTROPHE"},

		{'Q', "KEY_Q"},
		{'W', "KEY_W"},
		{'E', "KEY_E"},
		{'R', "KEY_R"},
		{'T', "KEY_T"},
		{'Z', "KEY_Z"},
		{'U', "KEY_U"},
		{'I', "KEY_I"},
		{'O', "KEY_O"},
		{'P', "KEY_P"},
		{VK_OEM_PLUS, "KEY_PLUS"},

		{'A', "KEY_A"},
		{'S', "KEY_S"},
		{'D', "KEY_D"},
		{'F', "KEY_F"},
		{'G', "KEY_G"},
		{'H', "KEY_H"},
		{'J', "KEY_J"},
		{'K', "KEY_K"},
		{'L', "KEY_L"},
		{VK_OEM_6, "KEY_CIRCUMFLEX"}, // ^ or ¨ depending on layout

		{VK_OEM_5, "KEY_HASH"},
		{'Y', "KEY_Y"},
		{'X', "KEY_X"},
		{'C', "KEY_C"},
		{'V', "KEY_V"},
		{'B', "KEY_B"},
		{'N', "KEY_N"},
		{'M', "KEY_M"},
		{VK_OEM_MINUS, "KEY_MINUS"},

		{VK_SPACE, "KEY_SPACE"},

		{VK_F1, "KEY_F1"},
		{VK_F2, "KEY_F2"},
		{VK_F3, "KEY_F3"},
		{VK_F4, "KEY_F4"},
		{VK_F5, "KEY_F5"},
		{VK_F6, "KEY_F6"},
		{VK_F7, "KEY_F7"},
		{VK_F8, "KEY_F8"},
		{VK_F9, "KEY_F9"},
		{VK_F10, "KEY_F10"},

		{VK_BACK, "KEY_BACKSPACE"},
		{VK_RETURN, "KEY_ENTER"},

		{VK_SHIFT, "KEY_SHIFT_L"},
		{VK_CONTROL, "KEY_CTRL_L"},
		{VK_LMENU, "KEY_ALT_L"},

		{VK_OEM_COMMA, "KEY_COMMA"},
		{VK_OEM_PERIOD, "KEY_DOT"},
		{VK_RSHIFT, "KEY_SHIFT_R"},
		{VK_OEM_4, "KEY_SHARP_S"},
		{VK_TAB, "KEY_TAB"},

		{VK_NUMPAD7, "KEY_PAD_7"},
		{VK_NUMPAD8, "KEY_PAD_8"},
		{VK_NUMPAD9, "KEY_PAD_9"},
		{VK_SUBTRACT, "KEY_PAD_MINUS"},
		{VK_NUMPAD4, "KEY_PAD_4"},
		{VK_NUMPAD5, "KEY_PAD_5"},
		{VK_NUMPAD6, "KEY_PAD_6"},
		{VK_ADD, "KEY_PAD_PLUS"},
		{VK_NUMPAD1, "KEY_PAD_1"},
		{VK_NUMPAD2, "KEY_PAD_2"},
		{VK_NUMPAD3, "KEY_PAD_3"},
		{VK_NUMPAD0, "KEY_PAD_0"},
		{VK_DECIMAL, "KEY_PAD_DOT"},

		{VK_OEM_102, "KEY_SMALLER"},
		{VK_F11, "KEY_F11"},
		{VK_F12, "KEY_F12"},
};


bool operator<(KeyMod const& a, KeyMod const& b)
{
	if(a.key < b.key)
		return true;
	if(a.key > b.key)
		return false;
	return a.mods < b.mods;
}

std::map<KeyMod, std::string> KeyCharMap = {

		{{"KEY_A", 0}, "a"},
		{{"KEY_A", MOD_SHIFT}, "A"},
		{{"KEY_B", 0}, "b"},
		{{"KEY_B", MOD_SHIFT}, "B"},
		{{"KEY_C", 0}, "c"},
		{{"KEY_C", MOD_SHIFT}, "C"},
		{{"KEY_D", 0}, "d"},
		{{"KEY_D", MOD_SHIFT}, "D"},
		{{"KEY_E", 0}, "e"},
		{{"KEY_E", MOD_SHIFT}, "E"},
		{{"KEY_E", MOD_CTRL | MOD_ALT}, "€"},
		{{"KEY_F", 0}, "f"},
		{{"KEY_F", MOD_SHIFT}, "F"},
		{{"KEY_G", 0}, "g"},
		{{"KEY_G", MOD_SHIFT}, "G"},
		{{"KEY_H", 0}, "h"},
		{{"KEY_H", MOD_SHIFT}, "H"},
		{{"KEY_I", 0}, "i"},
		{{"KEY_I", MOD_SHIFT}, "I"},
		{{"KEY_J", 0}, "j"},
		{{"KEY_J", MOD_SHIFT}, "J"},
		{{"KEY_K", 0}, "k"},
		{{"KEY_K", MOD_SHIFT}, "K"},
		{{"KEY_L", 0}, "l"},
		{{"KEY_L", MOD_SHIFT}, "L"},
		{{"KEY_M", 0}, "m"},
		{{"KEY_M", MOD_SHIFT}, "M"},
		{{"KEY_N", 0}, "n"},
		{{"KEY_N", MOD_SHIFT}, "N"},
		{{"KEY_O", 0}, "o"},
		{{"KEY_O", MOD_SHIFT}, "O"},
		{{"KEY_P", 0}, "p"},
		{{"KEY_P", MOD_SHIFT}, "P"},
		{{"KEY_Q", 0}, "q"},
		{{"KEY_Q", MOD_SHIFT}, "Q"},
		{{"KEY_R", 0}, "r"},
		{{"KEY_R", MOD_SHIFT}, "R"},
		{{"KEY_S", 0}, "s"},
		{{"KEY_S", MOD_SHIFT}, "S"},
		{{"KEY_T", 0}, "t"},
		{{"KEY_T", MOD_SHIFT}, "T"},
		{{"KEY_U", 0}, "u"},
		{{"KEY_U", MOD_SHIFT}, "U"},
		{{"KEY_V", 0}, "v"},
		{{"KEY_V", MOD_SHIFT}, "V"},
		{{"KEY_W", 0}, "w"},
		{{"KEY_W", MOD_SHIFT}, "W"},
		{{"KEY_X", 0}, "x"},
		{{"KEY_X", MOD_SHIFT}, "X"},
		{{"KEY_Y", 0}, "y"},
		{{"KEY_Y", MOD_SHIFT}, "Y"},
		{{"KEY_Z", 0}, "z"},
		{{"KEY_Z", MOD_SHIFT}, "Z"},

		{{"KEY_SPACE", 0}, " "},
		{{"KEY_SPACE", MOD_SHIFT}, " "},

		{{"KEY_1", 0}, "1"},
		{{"KEY_1", MOD_SHIFT}, "!"},
		{{"KEY_2", 0}, "2"},
		{{"KEY_2", MOD_SHIFT}, "\""},
		{{"KEY_3", 0}, "3"},
		{{"KEY_3", MOD_SHIFT}, "§"},
		{{"KEY_4", 0}, "4"},
		{{"KEY_4", MOD_SHIFT}, "$"},
		{{"KEY_5", 0}, "5"},
		{{"KEY_5", MOD_SHIFT}, "%"},
		{{"KEY_6", 0}, "6"},
		{{"KEY_6", MOD_SHIFT}, "&"},
		{{"KEY_7", 0}, "7"},
		{{"KEY_7", MOD_SHIFT}, "/"},
		{{"KEY_7", MOD_ALT | MOD_CTRL}, "{"},
		{{"KEY_8", 0}, "8"},
		{{"KEY_8", MOD_SHIFT}, "("},
		{{"KEY_8", MOD_ALT | MOD_CTRL}, "["},
		{{"KEY_9", 0}, "9"},
		{{"KEY_9", MOD_SHIFT}, ")"},
		{{"KEY_9", MOD_ALT | MOD_CTRL}, "]"},
		{{"KEY_0", 0}, "0"},
		{{"KEY_0", MOD_SHIFT}, "="},
		{{"KEY_0", MOD_ALT | MOD_CTRL}, "}"},
		{{"KEY_0", MOD_CTRL}, "\0"},

		{{"KEY_SHARP_S", 0}, "ß"},
		{{"KEY_SHARP_S", MOD_SHIFT}, "?"},
		{{"KEY_SHARP_S", MOD_ALT | MOD_CTRL}, "\\92"},

		{{"KEY_SMALLER", 0}, "<"},
		{{"KEY_SMALLER", MOD_SHIFT}, ">"},
		{{"KEY_SMALLER", MOD_ALT | MOD_CTRL}, "|"},

		{{"KEY_COMMA", 0}, ","},
		{{"KEY_COMMA", MOD_SHIFT}, ";"},
		{{"KEY_DOT", 0}, "."},
		{{"KEY_DOT", MOD_SHIFT}, ":"},
		{{"KEY_MINUS", 0}, "-"},
		{{"KEY_MINUS", MOD_SHIFT}, "_"},

		{{"KEY_PLUS", 0}, "+"},
		{{"KEY_PLUS", MOD_SHIFT}, "*"},
		{{"KEY_PLUS", MOD_ALT | MOD_CTRL}, "~"},

		{{"KEY_APOSTROPHE", 0}, "´"},
		{{"KEY_APOSTROPHE", MOD_SHIFT}, "`"},

		{{"KEY_HASH", 0}, "#"},
		{{"KEY_HASH", MOD_SHIFT}, "'"},

		{{"KEY_CIRCUMFLEX", 0}, "^"},
		{{"KEY_CIRCUMFLEX", MOD_SHIFT}, "°"},
};

namespace fensterserver
{
	std::string platformKeyForVKey(uint8_t scancode)
	{
		return VK_MAP[scancode];
	}

	char platformCharForKey(fenster::KeyInfo key)
	{
		uint8_t mods = 0;
		if(key.shift)
			mods |= MOD_SHIFT;
		if(key.alt)
			mods |= MOD_ALT;
		if(key.ctrl)
			mods |= MOD_CTRL;

		auto it = KeyCharMap.find({key.key, mods});
		if(it != KeyCharMap.end())
			return it->second[0];

		return -1;
	}
}

#endif
