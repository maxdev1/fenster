// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later
// fenster — lightweight window server and UI toolkit
// macOS SDL scancode -> KEY_* mapping (German QWERTZ baseline)
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef __APPLE__

#include "macos_sdl_sim.hpp"
#include <libfenster/input/key_info.hpp>
#include <libfenster/platform/platform.hpp>

#include <SDL2/SDL.h>

#include <map>
#include <string>
#include <cstdint>

struct KeyMod
{
	std::string key;
	uint8_t mods;
};

#define MOD_SHIFT (1 << 0)
#define MOD_ALT   (1 << 1)
#define MOD_CTRL  (1 << 2)

bool operator<(KeyMod const& a, KeyMod const& b)
{
	if(a.key < b.key) return true;
	if(a.key > b.key) return false;
	return a.mods < b.mods;
}

// Char map (copied from your Windows implementation; German layout semantics)
std::map<KeyMod, std::string> KeyCharMap = {
		{{"KEY_A", 0}, "a"},    {{"KEY_A", MOD_SHIFT}, "A"},
		{{"KEY_B", 0}, "b"},    {{"KEY_B", MOD_SHIFT}, "B"},
		{{"KEY_C", 0}, "c"},    {{"KEY_C", MOD_SHIFT}, "C"},
		{{"KEY_D", 0}, "d"},    {{"KEY_D", MOD_SHIFT}, "D"},
		{{"KEY_E", 0}, "e"},    {{"KEY_E", MOD_SHIFT}, "E"},
		{{"KEY_E", MOD_CTRL | MOD_ALT}, "€"},
		{{"KEY_F", 0}, "f"},    {{"KEY_F", MOD_SHIFT}, "F"},
		{{"KEY_G", 0}, "g"},    {{"KEY_G", MOD_SHIFT}, "G"},
		{{"KEY_H", 0}, "h"},    {{"KEY_H", MOD_SHIFT}, "H"},
		{{"KEY_I", 0}, "i"},    {{"KEY_I", MOD_SHIFT}, "I"},
		{{"KEY_J", 0}, "j"},    {{"KEY_J", MOD_SHIFT}, "J"},
		{{"KEY_K", 0}, "k"},    {{"KEY_K", MOD_SHIFT}, "K"},
		{{"KEY_L", 0}, "l"},    {{"KEY_L", MOD_SHIFT}, "L"},
		{{"KEY_M", 0}, "m"},    {{"KEY_M", MOD_SHIFT}, "M"},
		{{"KEY_N", 0}, "n"},    {{"KEY_N", MOD_SHIFT}, "N"},
		{{"KEY_O", 0}, "o"},    {{"KEY_O", MOD_SHIFT}, "O"},
		{{"KEY_P", 0}, "p"},    {{"KEY_P", MOD_SHIFT}, "P"},
		{{"KEY_Q", 0}, "q"},    {{"KEY_Q", MOD_SHIFT}, "Q"},
		{{"KEY_R", 0}, "r"},    {{"KEY_R", MOD_SHIFT}, "R"},
		{{"KEY_S", 0}, "s"},    {{"KEY_S", MOD_SHIFT}, "S"},
		{{"KEY_T", 0}, "t"},    {{"KEY_T", MOD_SHIFT}, "T"},
		{{"KEY_U", 0}, "u"},    {{"KEY_U", MOD_SHIFT}, "U"},
		{{"KEY_V", 0}, "v"},    {{"KEY_V", MOD_SHIFT}, "V"},
		{{"KEY_W", 0}, "w"},    {{"KEY_W", MOD_SHIFT}, "W"},
		{{"KEY_X", 0}, "x"},    {{"KEY_X", MOD_SHIFT}, "X"},
		{{"KEY_Y", 0}, "y"},    {{"KEY_Y", MOD_SHIFT}, "Y"},
		{{"KEY_Z", 0}, "z"},    {{"KEY_Z", MOD_SHIFT}, "Z"},

		{{"KEY_SPACE", 0}, " "}, {{ "KEY_SPACE", MOD_SHIFT }, " "},

		{{"KEY_ENTER", 0}, "\n"},
		{{"KEY_TAB", 0}, "\t"},

		{{"KEY_1", 0}, "1"}, {{ "KEY_1", MOD_SHIFT }, "!"},
		{{"KEY_2", 0}, "2"}, {{ "KEY_2", MOD_SHIFT }, "\""},
		{{"KEY_3", 0}, "3"}, {{ "KEY_3", MOD_SHIFT }, "§"},
		{{"KEY_4", 0}, "4"}, {{ "KEY_4", MOD_SHIFT }, "$"},
		{{"KEY_5", 0}, "5"}, {{ "KEY_5", MOD_SHIFT }, "%"},
		{{"KEY_6", 0}, "6"}, {{ "KEY_6", MOD_SHIFT }, "&"},
		{{"KEY_7", 0}, "7"}, {{ "KEY_7", MOD_SHIFT }, "/"},
		{{"KEY_7", MOD_ALT | MOD_CTRL}, "{"},
		{{"KEY_8", 0}, "8"}, {{ "KEY_8", MOD_SHIFT }, "("},
		{{"KEY_8", MOD_ALT | MOD_CTRL}, "["},
		{{"KEY_9", 0}, "9"}, {{ "KEY_9", MOD_SHIFT }, ")"},
		{{"KEY_9", MOD_ALT | MOD_CTRL}, "]"},
		{{"KEY_0", 0}, "0"}, {{ "KEY_0", MOD_SHIFT }, "="},
		{{"KEY_0", MOD_ALT | MOD_CTRL}, "}"},
		{{"KEY_0", MOD_CTRL}, "\0"},

		{{"KEY_SHARP_S", 0}, "ß"}, {{ "KEY_SHARP_S", MOD_SHIFT }, "?"},
		{{ "KEY_SHARP_S", MOD_ALT | MOD_CTRL }, "\\92"},

		{{"KEY_SMALLER", 0}, "<"}, {{ "KEY_SMALLER", MOD_SHIFT }, ">"},
		{{ "KEY_SMALLER", MOD_ALT | MOD_CTRL }, "|"},

		{{"KEY_COMMA", 0}, ","}, {{ "KEY_COMMA", MOD_SHIFT }, ";"},
		{{"KEY_DOT", 0}, "."}, {{ "KEY_DOT", MOD_SHIFT }, ":"},
		{{"KEY_MINUS", 0}, "-"}, {{ "KEY_MINUS", MOD_SHIFT }, "_"},
		{{"KEY_PLUS", 0}, "+"}, {{ "KEY_PLUS", MOD_SHIFT }, "*"},
		{{ "KEY_PLUS", MOD_ALT | MOD_CTRL }, "~"},

		{{"KEY_APOSTROPHE", 0}, "´"}, {{ "KEY_APOSTROPHE", MOD_SHIFT }, "`"},
		{{"KEY_HASH", 0}, "#"}, {{ "KEY_HASH", MOD_SHIFT }, "'"},
		{{"KEY_CIRCUMFLEX", 0}, "^"}, {{ "KEY_CIRCUMFLEX", MOD_SHIFT }, "°"},
};

// --- scancode -> KEY_* map (German/QWERTZ baseline, mirrors Windows VK_MAP) ---
static std::map<SDL_Scancode, std::string> SDL_SCANCODE_MAP = {
	// top row numbers / punctuation
	{SDL_SCANCODE_ESCAPE, "KEY_ESC"},
	{SDL_SCANCODE_1, "KEY_1"},
	{SDL_SCANCODE_2, "KEY_2"},
	{SDL_SCANCODE_3, "KEY_3"},
	{SDL_SCANCODE_4, "KEY_4"},
	{SDL_SCANCODE_5, "KEY_5"},
	{SDL_SCANCODE_6, "KEY_6"},
	{SDL_SCANCODE_7, "KEY_7"},
	{SDL_SCANCODE_8, "KEY_8"},
	{SDL_SCANCODE_9, "KEY_9"},
	{SDL_SCANCODE_0, "KEY_0"},
	{SDL_SCANCODE_APOSTROPHE, "KEY_APOSTROPHE"}, // OEM_7
	{SDL_SCANCODE_EQUALS, "KEY_PLUS"},          // OEM_PLUS

	// letters (QWERTZ order referenced, but symbolic names are letter-based)
	{SDL_SCANCODE_Q, "KEY_Q"},
	{SDL_SCANCODE_W, "KEY_W"},
	{SDL_SCANCODE_E, "KEY_E"},
	{SDL_SCANCODE_R, "KEY_R"},
	{SDL_SCANCODE_T, "KEY_T"},
	{SDL_SCANCODE_Y, "KEY_Y"},
	{SDL_SCANCODE_U, "KEY_U"},
	{SDL_SCANCODE_I, "KEY_I"},
	{SDL_SCANCODE_O, "KEY_O"},
	{SDL_SCANCODE_P, "KEY_P"},
	{SDL_SCANCODE_A, "KEY_A"},
	{SDL_SCANCODE_S, "KEY_S"},
	{SDL_SCANCODE_D, "KEY_D"},
	{SDL_SCANCODE_F, "KEY_F"},
	{SDL_SCANCODE_G, "KEY_G"},
	{SDL_SCANCODE_H, "KEY_H"},
	{SDL_SCANCODE_J, "KEY_J"},
	{SDL_SCANCODE_K, "KEY_K"},
	{SDL_SCANCODE_L, "KEY_L"},
	{SDL_SCANCODE_Z, "KEY_Z"}, // QWERTZ: Z and Y positions differ physically, but symbolic name remains KEY_Z
	{SDL_SCANCODE_X, "KEY_X"},
	{SDL_SCANCODE_C, "KEY_C"},
	{SDL_SCANCODE_V, "KEY_V"},
	{SDL_SCANCODE_B, "KEY_B"},
	{SDL_SCANCODE_N, "KEY_N"},
	{SDL_SCANCODE_M, "KEY_M"},

	// punctuation / OEMs
	{SDL_SCANCODE_RIGHTBRACKET, "KEY_CIRCUMFLEX"}, // OEM_6 (']' in US)
	{SDL_SCANCODE_NONUSBACKSLASH, "KEY_HASH"},     // OEM_5 / OEM_102 equivalents (smaller/<> key)
	{SDL_SCANCODE_COMMA, "KEY_COMMA"},
	{SDL_SCANCODE_PERIOD, "KEY_DOT"},
	{SDL_SCANCODE_MINUS, "KEY_MINUS"},
	{SDL_SCANCODE_GRAVE, "KEY_SHARP_S"}, // approximate (depends on layout)

	// whitespace / control
	{SDL_SCANCODE_SPACE, "KEY_SPACE"},
	{SDL_SCANCODE_TAB, "KEY_TAB"},
	{SDL_SCANCODE_BACKSPACE, "KEY_BACKSPACE"},
	{SDL_SCANCODE_RETURN, "KEY_ENTER"},

	// function keys
	{SDL_SCANCODE_F1, "KEY_F1"},
	{SDL_SCANCODE_F2, "KEY_F2"},
	{SDL_SCANCODE_F3, "KEY_F3"},
	{SDL_SCANCODE_F4, "KEY_F4"},
	{SDL_SCANCODE_F5, "KEY_F5"},
	{SDL_SCANCODE_F6, "KEY_F6"},
	{SDL_SCANCODE_F7, "KEY_F7"},
	{SDL_SCANCODE_F8, "KEY_F8"},
	{SDL_SCANCODE_F9, "KEY_F9"},
	{SDL_SCANCODE_F10, "KEY_F10"},
	{SDL_SCANCODE_F11, "KEY_F11"},
	{SDL_SCANCODE_F12, "KEY_F12"},

	// numpad
	{SDL_SCANCODE_KP_7, "KEY_PAD_7"},
	{SDL_SCANCODE_KP_8, "KEY_PAD_8"},
	{SDL_SCANCODE_KP_9, "KEY_PAD_9"},
	{SDL_SCANCODE_KP_MINUS, "KEY_PAD_MINUS"},
	{SDL_SCANCODE_KP_4, "KEY_PAD_4"},
	{SDL_SCANCODE_KP_5, "KEY_PAD_5"},
	{SDL_SCANCODE_KP_6, "KEY_PAD_6"},
	{SDL_SCANCODE_KP_PLUS, "KEY_PAD_PLUS"},
	{SDL_SCANCODE_KP_1, "KEY_PAD_1"},
	{SDL_SCANCODE_KP_2, "KEY_PAD_2"},
	{SDL_SCANCODE_KP_3, "KEY_PAD_3"},
	{SDL_SCANCODE_KP_0, "KEY_PAD_0"},
	{SDL_SCANCODE_KP_DECIMAL, "KEY_PAD_DOT"},

	// arrows (use Windows naming)
	{SDL_SCANCODE_UP, "KEY_ARROW_UP"},
	{SDL_SCANCODE_DOWN, "KEY_ARROW_DOWN"},
	{SDL_SCANCODE_LEFT, "KEY_ARROW_LEFT"},
	{SDL_SCANCODE_RIGHT, "KEY_ARROW_RIGHT"},

	// modifiers
	{SDL_SCANCODE_LSHIFT, "KEY_SHIFT_L"},
	{SDL_SCANCODE_RSHIFT, "KEY_SHIFT_R"},
	{SDL_SCANCODE_LCTRL,  "KEY_CTRL_L"},
	{SDL_SCANCODE_RCTRL,  "KEY_CTRL_R"},
	{SDL_SCANCODE_LALT,   "KEY_ALT_L"},
	{SDL_SCANCODE_RALT,   "KEY_ALT_R"},
	// you asked to treat GUI/Command as ALT: map them to KEY_ALT_L / KEY_ALT_R
	{SDL_SCANCODE_LGUI,   "KEY_ALT_L"},
	{SDL_SCANCODE_RGUI,   "KEY_ALT_R"},
};

// helper: platformKeyForSDLKey (keeps your SDL-based event path happy)
namespace fensterserver
{
	std::string platformKeyForSDLKey(const SDL_Keysym &ks)
	{
		SDL_Scancode sc = ks.scancode;
		auto it = SDL_SCANCODE_MAP.find(sc);
		if(it != SDL_SCANCODE_MAP.end())
			return it->second;
		// fallback to readable name for debugging
		return std::string(SDL_GetScancodeName(sc));
	}

	// compatibility: some code calls platformKeyForVKey(uint8_t) (Windows style).
	// We'll accept a uint8_t scancode (SDL_Scancode fits in an int but may be cast).
	std::string platformKeyForVKey(uint8_t scancode)
	{
		SDL_Scancode sc = static_cast<SDL_Scancode>(scancode);
		auto it = SDL_SCANCODE_MAP.find(sc);
		if(it != SDL_SCANCODE_MAP.end())
			return it->second;
		return std::string(SDL_GetScancodeName(sc));
	}

	// same platformCharForKey implementation as Windows version
	char platformCharForKey(fenster::KeyInfo key)
	{
		uint8_t mods = 0;
		if(key.shift) mods |= MOD_SHIFT;
		if(key.alt)   mods |= MOD_ALT;
		if(key.ctrl)  mods |= MOD_CTRL;

		auto it = KeyCharMap.find({key.key, mods});
		if(it != KeyCharMap.end())
			return it->second[0];

		return -1;
	}
}

#endif // __APPLE__
