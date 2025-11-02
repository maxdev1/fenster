// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libjson/json.hpp"

#include <cctype>
#include <string.h>
#include <stdlib.h>

void g_json::skipWhitespace()
{
	while(std::isspace(*source))
		++source;
}


bool g_json::consume(char c)
{
	skipWhitespace();
	if(*source == c)
	{
		++source;
		return true;
	}
	return false;
}

std::string g_json::parseString()
{
	if(*source != '"')
	{
		printf("JSON ERROR; expected string at %i", source - start);
		return "";
	}

	++source;
	std::string s;
	while(*source && *source != '"')
	{
		if(*source == '\\')
		{
			++source;
			if(*source == '"')
				s += '"';
			else if(*source == 'n')
				s += '\n';
			else if(*source == 't')
				s += '\t';
			else
				s += *source;
		}
		else
			s += *source;
		++source;
	}

	if(*source != '"')
	{
		printf("JSON ERROR; unterminated string at %i", source - start);
	}

	++source;
	return s;
}

double g_json::parseNumber()
{
	char* end;
	double d = strtod(source, &end);
	source = end;
	return d;
}

g_json_node g_json::parseValue()
{
	skipWhitespace();

	if(*source == '{')
		return parseObject();

	if(*source == '[')
		return parseArray();

	if(*source == '"')
		return g_json_node{parseString()};

	if(std::isdigit(*source) || *source == '-')
		return g_json_node{parseNumber()};

	if(!strncmp(source, "true", 4))
	{
		source += 4;
		return g_json_node{true};
	}

	if(!strncmp(source, "false", 5))
	{
		source += 5;
		return g_json_node{false};
	}

	if(!strncmp(source, "null", 4))
	{
		source += 4;
		return g_json_node{nullptr};
	}

	printf("JSON ERROR; invalid value at %i", source - start);
	return g_json_node{nullptr};
}

g_json_node g_json::parseArray()
{
	consume('[');
	g_json_node::array arr;
	skipWhitespace();
	if(consume(']'))
		return g_json_node{arr};
	for(;;)
	{
		arr.push_back(parseValue());
		skipWhitespace();

		if(consume(']'))
			break;

		if(!consume(','))
		{
			printf("JSON ERROR; expected comma at %i", source - start);
			break;
		}
	}
	return g_json_node{arr};
}

g_json_node g_json::parseObject()
{
	consume('{');
	g_json_node::object obj;
	skipWhitespace();
	if(consume('}'))
		return g_json_node{obj};
	for(;;)
	{
		std::string key = parseString();
		if(!consume(':'))
		{
			printf("JSON ERROR; expected colon at %i", source - start);
			break;
		}
		obj[key] = parseValue();
		skipWhitespace();
		if(consume('}'))
			break;
		if(!consume(','))
		{
			printf("JSON ERROR; expected comma at %i", source - start);
			break;
		}
		skipWhitespace();
	}
	return g_json_node{obj};
}

g_json_node g_json::parse(const std::string& s)
{
	start = s.c_str();
	source = start;
	return parseValue();
}

std::string g_json::stringify(g_json_node& node)
{
	if(node.isNull())
	{
		return "null";
	}

	if(node.isBool())
	{
		return node.asBool() ? "true" : "false";
	}

	if(node.isNumber())
	{
		char buf[64];
		snprintf(buf, sizeof(buf), "%.17g", node.asNumber());
		return buf;
	}

	if(node.isString())
	{
		auto& s = node.asString();

		std::string out = "\"";
		for(char c: s)
		{
			switch(c)
			{
				case '"':
					out += "\\\"";
					break;
				case '\\':
					out += "\\\\";
					break;
				case '\n':
					out += "\\n";
					break;
				case '\t':
					out += "\\t";
					break;
				default:
					out += c;
					break;
			}
		}
		out += "\"";

		return out;
	}

	if(node.isArray())
	{
		auto& a = node.asArray();

		std::string out = "[";
		for(size_t i = 0; i < a.size(); ++i)
		{
			if(i)
				out += ",";
			out += stringify(a[i]);
		}
		out += "]";

		return out;
	}

	if(node.isObject())
	{
		auto& o = node.asObject();

		std::string out = "{";
		bool first = true;
		for(auto& [k, val]: o)
		{
			if(!first)
				out += ",";
			first = false;
			out += "\"" + k + "\":" + stringify(val);
		}
		out += "}";

		return out;
	}

	return "null";
}
