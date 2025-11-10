// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "server.hpp"
#include "component_registry.hpp"
#include "components/button.hpp"
#include "components/cursor.hpp"
#include "events/event.hpp"
#include "events/locatable.hpp"
#include "interface/registration_thread.hpp"
#include "components/window.hpp"
#include "components/text/text_field.hpp"
#include "test.hpp"
#include "platform/platform.hpp"

#include <cairo/cairo.h>
#include <iostream>
#include <cstdio>
#include <sstream>

namespace fensterserver
{
	static SYS_MUTEX_T dispatchLock = fenster::platformInitializeMutex(true);
	static int framesTotal = 0;
	static bool debugOn = false;
	int debugBorderCycle = 0;

	Server* Server::instance()
	{
		return server;
	}

	Server::Server()
	{
		eventProcessor = new EventProcessor();
	}

	void Server::startInputHandlers()
	{
		platformStartInput();

		std::string keyLayout = "de-DE";
		if(!platformInitializeKeyboardLayout(keyLayout))
		{
			fenster::platformLog(("failed to load keyboard layout '" + keyLayout + "', no keyboard input available").c_str());
		}

		platformLoadCursors();
		server->requestUpdateLater();
	}

	void Server::startOtherTasks()
	{
		fenster::platformRegisterTaskIdentifier("windowserver/launcher");
		// TODO not the windowservers job
		fenster::platformSpawn("/applications/desktop.bin", "", "");
	}

	void Server::startLazyUpdateLoop()
	{
		Server::instance()->updateDebounceLoop();
	}

	void Server::launch()
	{
		fenster::platformRegisterTaskIdentifier("windowserver");

		initializeVideo();

		fenster::platformCreateThread((void*) &startInputHandlers);

		fenster::Dimension resolution = videoOutput->getResolution();
		fenster::Rectangle screenBounds(0, 0, resolution.width, resolution.height);
		createVitalComponents(screenBounds);

		Test::createTestComponents();

		fenster::platformCreateThread((void*) &interfaceRegistrationThread);
		fenster::platformCreateThread((void*) &startOtherTasks);
		fenster::platformCreateThread((void*) &startLazyUpdateLoop);

		renderTask = fenster::platformGetTid();
		updateLoop(screenBounds);
	}

	void Server::createVitalComponents(fenster::Rectangle screenBounds)
	{
		screen = new Screen();
		screen->setBounds(screenBounds);

		stateLabel = new Label();
		stateLabel->setTitle("fps");
		stateLabel->setAlignment(fenster::TextAlignment::RIGHT);
		stateLabel->setVisible(debugOn);
		stateLabel->setBounds(fenster::Rectangle(10, screenBounds.height - 100, screenBounds.width - 20, 30));
		instance()->stateLabel->setColor(_RGB(255, 255, 255));
		screen->addChild(stateLabel);

#if defined(_WIN32) || defined(__APPLE__)
		Panel* backgroundPanel = new Panel();
		backgroundPanel->setBounds(screenBounds);
		backgroundPanel->setBackground(_RGB(0,0,0));
		screen->addChild(backgroundPanel);
#endif

		fenster::platformCreateThread((void*) &Server::fpsCounter);
	}

	void Server::initializeVideo()
	{
		videoOutput = platformCreateVideoOutput();

		if(!videoOutput->initialize())
		{
			fenster::platformLog("failed to initialize generic video output");
			fenster::platformExit(-1);
		}
	}

	void Server::updateLoop(const fenster::Rectangle& screenBounds)
	{
		fenster::platformRegisterTaskIdentifier("windowserver/updater");

		Graphics global;
		global.resize(screenBounds.width, screenBounds.height, false);

		Cursor::nextPosition = fenster::Point(screenBounds.width / 2, screenBounds.height / 2);

		uint64_t lastUpdate = 0;
		fenster::platformAcquireMutex(updateLock);
		while(true)
		{
			eventProcessor->process();

			screen->resolveRequirement(COMPONENT_REQUIREMENT_UPDATE, 0);
			screen->resolveRequirement(COMPONENT_REQUIREMENT_LAYOUT, 0);
			screen->resolveRequirement(COMPONENT_REQUIREMENT_PAINT, 0);

			screen->blit(&global, screenBounds, fenster::Point(0, 0));
			Cursor::paint(&global);

			output(&global);

			framesTotal++;
			fenster::platformAcquireMutexTimeout(updateLock, 1000);

			auto now = fenster::platformMillis();
			if(now - lastUpdate < 10)
				fenster::platformSleep(5);
			lastUpdate = now;
		}
	}

	void Server::requestUpdateImmediately() const
	{
		fenster::platformReleaseMutex(updateLock);
	}

	void Server::requestUpdateLater() const
	{
		fenster::platformReleaseMutex(lazyUpdateLock);
	}

	void Server::updateDebounceLoop() const
	{
		fenster::platformAcquireMutex(lazyUpdateLock);
		while(true)
		{
			fenster::platformSleep(10);
			fenster::platformAcquireMutexTimeout(lazyUpdateLock, 1000);
			fenster::platformReleaseMutex(updateLock);
		}
	}

	void Server::output(Graphics* graphics)
	{
		fenster::Rectangle invalid = screen->grabInvalid();
		if(invalid.width == 0 && invalid.height == 0)
			return;

		// Draw the green debug border around invalid area
		if(debugOn)
		{
			auto cr = graphics->acquireContext();
			if(cr)
			{
				debugBorderCycle++;
				if(debugBorderCycle > 100)
					debugBorderCycle = 0;

				cairo_save(cr);
				cairo_set_line_width(cr, 2);
				cairo_rectangle(cr, invalid.x, invalid.y, invalid.width, invalid.height);
				cairo_set_source_rgba(cr,
									  debugBorderCycle % 3 == 0 ? ((double) (debugBorderCycle + 100)) / 255 : 0,
									  debugBorderCycle % 2 == 0 ? ((double) (debugBorderCycle + 100)) / 255 : 0,
									  debugBorderCycle % 2 == 1 ? ((double) (debugBorderCycle + 100)) / 255 : 0,
									  1);
				cairo_stroke(cr);
				cairo_restore(cr);
				graphics->releaseContext();
			}
		}

		fenster::Dimension resolution = videoOutput->getResolution();
		fenster::Rectangle screenBounds(0, 0, resolution.width, resolution.height);

		auto screenSurface = graphics->getSurface();
		cairo_surface_flush(screenSurface);
		auto buffer = (fenster::ColorArgb*) cairo_image_surface_get_data(screenSurface);

		fenster::Rectangle totalInvalid = invalid;
		totalInvalid.extend(lastInvalid.getStart());
		totalInvalid.extend(lastInvalid.getEnd());
		videoOutput->blit(totalInvalid.clip(screenBounds), screenBounds, buffer);

		lastInvalid = invalid;
	}

	Component* Server::dispatchUpwards(Component* component, Event& event)
	{
		// store when dispatching to parents
		fenster::Point initialPosition;
		Locatable* locatable = dynamic_cast<Locatable*>(&event);
		if(locatable)
			initialPosition = locatable->position;

		// check upwards until someone accepts the event
		Component* acceptor = component;
		while(dispatch(acceptor, event) == nullptr)
		{
			acceptor = acceptor->getParent();
			if(acceptor == nullptr)
				break;

			// restore position on locatable events
			if(locatable)
				locatable->position = initialPosition;
		}
		return acceptor;
	}

	Component* Server::dispatch(Component* component, Event& event)
	{
		Component* handledBy = nullptr;

		if(component->canHandleEvents())
		{
			Locatable* locatable = dynamic_cast<Locatable*>(&event);
			if(locatable != 0)
			{
				fenster::Point locationOnScreen = component->getLocationOnScreen();
				locatable->position.x -= locationOnScreen.x;
				locatable->position.y -= locationOnScreen.y;
			}

			fenster::platformAcquireMutex(dispatchLock);
			handledBy = event.visit(component);
			fenster::platformReleaseMutex(dispatchLock);
		}

		return handledBy;
	}

	Component* Server::switchFocus(Component* to)
	{
		auto from = ComponentRegistry::get(Cursor::focusedComponent);

		auto actualTo = to->setFocused(true);
		if(actualTo)
		{
			Cursor::focusedComponent = actualTo->id;

			Window* fromWindow = nullptr;
			if(from && from != actualTo)
			{
				from->setFocused(false);
				fromWindow = from->getWindow();
			}

			Window* toWindow = actualTo->getWindow();
			if(fromWindow && toWindow != fromWindow && fromWindow != from)
			{
				fromWindow->setFocused(false);
			}

			if(toWindow)
			{
				toWindow->bringToFront();
				if(toWindow != actualTo)
				{
					toWindow->setFocused(true);
				}
			}
			return actualTo;
		}
		return nullptr;
	}


	void Server::fpsCounter()
	{
		fenster::platformRegisterTaskIdentifier("windowserver/fps-counter");

		int seconds = 0;
		for(;;)
		{
			if(!debugOn)
			{
				fenster::platformSleep(5000);
				continue;
			}

			fenster::platformSleep(1000);
			seconds++;
			std::stringstream s;
			s << "FPS: " << framesTotal << ", Time: " << seconds;
			instance()->stateLabel->setTitle(s.str());
			instance()->requestUpdateLater();
			framesTotal = 0;
		}
	}

	void Server::setDebug(bool cond)
	{
		debugOn = cond;
		server->stateLabel->setVisible(cond);
	}

	bool Server::isDebug()
	{
		return debugOn;
	}
}
