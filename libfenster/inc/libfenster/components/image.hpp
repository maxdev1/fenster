// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_IMAGE
#define LIBFENSTER_IMAGE

#include "component.hpp"

namespace fenster
{
	class Image : virtual public Component
	{
	public:
		explicit Image(ComponentId id) :
			Component(id)
		{
		}

		static Image* create();

		bool loadImage(std::string path);
	};
}

#endif
