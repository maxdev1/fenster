// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_TITLEDCOMPONENT
#define LIBFENSTER_TITLEDCOMPONENT

#include <bits/std_function.h>

#include "libfenster/interface.hpp"
#include "component.hpp"
#include "libfenster/listener/title_listener.hpp"

namespace fenster
{
	class TitledComponent : virtual public Component
	{
	protected:
		explicit TitledComponent(ComponentId id) :
			Component(id)
		{
		}

	public:
		~TitledComponent() override = default;

		virtual bool setTitle(std::string title);
		virtual std::string getTitle();

		virtual void addTitleListener(TitleListenerFunc callback);
	};
}

#endif
