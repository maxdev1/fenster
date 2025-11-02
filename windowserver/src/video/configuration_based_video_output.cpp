// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "configuration_based_video_output.hpp"
#include <fstream>
#include <iostream>
#include <libproperties/parser.hpp>
#include <sstream>

bool g_configuration_based_video_output::initialize()
{
    std::string path = "/system/graphics/configuration/resolution.cfg";
    std::ifstream in(path);
    if(!in.good())
    {
        std::cerr << "unable to read configuration file: '" << path << "'" << std::endl;
        return false;
    }

    g_properties_parser parser(in);
    auto properties = parser.getProperties();

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t bpp = 0;

    for(auto entry : properties)
    {
        std::stringstream s;
        s << entry.second;

        if(entry.first == "width")
        {
            s >> width;
        }
        else if(entry.first == "height")
        {
            s >> height;
        }
        else if(entry.first == "bpp")
        {
            s >> bpp;
        }
    }

    if(width == 0 || height == 0 || bpp == 0)
    {
        std::cerr << "invalid configuration. width: " << width << ", height: " << height << ", bpp: " << bpp << std::endl;
        return false;
    }

    return initializeWithSettings(width, height, bpp);
}
