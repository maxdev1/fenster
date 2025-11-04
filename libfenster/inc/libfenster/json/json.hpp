// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBJSON_JSON
#define LIBJSON_JSON

#include "json_node.hpp"

#include <string>

namespace fenster
{
	/**
	 * A simple JSON parser and serializer
	 */
	class Json
	{
		const char* start = nullptr;
		const char* source = nullptr;

		void skipWhitespace();
		bool consume(char c);

		std::string parseString();
		double parseNumber();
		JsonNode parseValue();
		JsonNode parseArray();
		JsonNode parseObject();

		JsonNode doParse(const std::string& s);
		std::string doStringify(JsonNode& node);

	public:
		static JsonNode parse(const std::string& s);
		static std::string stringify(JsonNode& node);
	};
}

#endif
