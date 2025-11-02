// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBPROPERTIES_PROPERTIES
#define LIBPROPERTIES_PROPERTIES

#include <map>
#include <string>

namespace properties
{
	class Properties
	{
		char current;
		uint32_t position;
		std::string content;

		void initialize(std::string content);

		bool propertyEntry(std::string& key, std::string& value);
		bool key(std::string& out);
		bool value(std::string& out);

		void next();

	public:
		Properties(std::ifstream& t);
		Properties(std::string content);
		std::map<std::string, std::string> getProperties();
	};
}

#endif
