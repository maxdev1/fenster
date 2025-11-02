// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef __LIBPROPERTIES_PARSER__
#define __LIBPROPERTIES_PARSER__

#include <map>
#include <string>
#include <vector>

class g_properties_parser
{
  private:
    char current;
    uint32_t position;
    std::string content;

    void initialize(std::string content);

    bool propertyEntry(std::string& key, std::string& value);
    bool key(std::string& out);
    bool value(std::string& out);

    void next();

  public:
    g_properties_parser(std::ifstream& t);
    g_properties_parser(std::string content);
    std::map<std::string, std::string> getProperties();
};

#endif
