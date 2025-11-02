// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_LISTENER_TITLELISTENER
#define LIBFENSTER_LISTENER_TITLELISTENER

#include "libfenster/listener/listener.hpp"
#include <functional>
#include <utility>
#include <bits/std_function.h>

namespace fenster
{
	typedef std::function<void(std::string)> TitleListenerFunc;

	class TitleListener : public Listener
	{
	public:
		~TitleListener() override = default;

		void process(ComponentEventHeader* header) override
		{
			handleTitleEvent((ComponentTitleEvent*) header);
		}

		virtual void handleTitleEvent(ComponentTitleEvent* e) = 0;
	};

	class TitleListenerDispatcher : public TitleListener
	{
		TitleListenerFunc func;

	public:
		explicit TitleListenerDispatcher(TitleListenerFunc func) :
			func(std::move(func))
		{
		};

		void handleTitleEvent(ComponentTitleEvent* e) override
		{
			func(std::string(e->title));
		}
	};
}

#endif
