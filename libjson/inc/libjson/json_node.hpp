// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBJSON_JSONNODE
#define LIBJSON_JSONNODE

#include <string>
#include <variant>
#include <vector>
#include <map>

namespace json
{
	struct JsonNode
	{
		using object = std::map<std::string, JsonNode>;
		using array = std::vector<JsonNode>;
		using value = std::variant<std::nullptr_t, bool, double, std::string, array, object>;

		value v{};

		JsonNode() : v(nullptr)
		{
		}

		JsonNode(std::nullptr_t) : v(nullptr)
		{
		}

		JsonNode(bool b) : v(b)
		{
		}

		JsonNode(double d) : v(d)
		{
		}

		JsonNode(const char* s) : v(std::string(s))
		{
		}

		JsonNode(std::string s) : v(std::move(s))
		{
		}

		JsonNode(array a) : v(std::move(a))
		{
		}

		JsonNode(object o) : v(std::move(o))
		{
		}

		bool isObject() const { return std::holds_alternative<object>(v); }
		bool isArray() const { return std::holds_alternative<array>(v); }
		bool isString() const { return std::holds_alternative<std::string>(v); }
		bool isNumber() const { return std::holds_alternative<double>(v); }
		bool isBool() const { return std::holds_alternative<bool>(v); }
		bool isNull() const { return std::holds_alternative<std::nullptr_t>(v); }

		object& asObject() { return std::get<object>(v); }
		array& asArray() { return std::get<array>(v); }
		std::string& asString() { return std::get<std::string>(v); }
		double& asNumber() { return std::get<double>(v); }
		bool& asBool() { return std::get<bool>(v); }
	};
}

#endif
