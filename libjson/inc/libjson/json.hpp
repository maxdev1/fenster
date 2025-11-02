// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBJSON_JSON
#define LIBJSON_JSON

#include "./json_node.hpp"
#include <string>

class g_json
{
    const char* start = nullptr;
    const char* source = nullptr;

    void skipWhitespace();
    bool consume(char c);

    std::string parseString();
    double parseNumber();
    g_json_node parseValue();
    g_json_node parseArray();
    g_json_node parseObject();

public:
    g_json_node parse(const std::string& s);
    std::string stringify(g_json_node& node);
};

#endif
