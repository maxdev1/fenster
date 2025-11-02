// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libproperties/properties.hpp"
#include <fstream>
#include <sstream>

namespace properties
{
	const char* whitespace = " \t\n\r\f\v";

	void Properties::initialize(std::string _content)
	{
		current = -1;
		position = 0;
		content = _content;

		next();
	}

	Properties::Properties(std::ifstream& t)
	{

		std::stringstream buffer;
		buffer << t.rdbuf();
		initialize(buffer.str());
	}

	Properties::Properties(std::string _content)
	{
		initialize(_content);
	}

	std::map<std::string, std::string> Properties::getProperties()
	{

		std::map<std::string, std::string> properties;

		while(true)
		{
			std::string a;
			std::string b;
			if(propertyEntry(a, b))
			{
				properties.insert(std::make_pair(a, b));
			}
			else
			{
				break;
			}
		}

		return properties;
	}

	bool Properties::propertyEntry(std::string& k, std::string& v)
	{

		// Key
		if(!key(k))
		{
			return false;
		}

		// = or :
		next();

		// Value
		if(!value(v))
		{
			return false;
		}

		return true;
	}

	bool Properties::key(std::string& out)
	{
		out = "";
		while(current != -1 && current != '=' && current != ':')
		{
			out += current;
			next();
		}

		out.erase(out.find_last_not_of(whitespace) + 1);
		out.erase(0, out.find_first_not_of(whitespace));
		return !out.empty();
	}

	bool Properties::value(std::string& out)
	{
		out = "";
		while(current != -1 && current != '\n')
		{
			out += current;
			next();
		}

		out.erase(out.find_last_not_of(whitespace) + 1);
		out.erase(0, out.find_first_not_of(whitespace));

		if(out.length() > 1 && out[0] == '"' && out[out.length() - 1] == '"')
		{
			out = out.substr(1, out.length() - 2);
		}

		return !out.empty();
	}

	void Properties::next()
	{
		if(position >= content.length())
		{
			current = -1;
			return;
		}

		current = content[position++];
	}
}
