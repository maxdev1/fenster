// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "server.hpp"
#include "component_registry.hpp"
#include "components/button.hpp"
#include "components/cursor.hpp"
#include "components/menu.hpp"
#include "events/event.hpp"
#include "events/locatable.hpp"
#include "interface/registration_thread.hpp"
#include "components/window.hpp"
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
			fenster::platformLog(
					("failed to load keyboard layout '" + keyLayout + "', no keyboard input available").c_str());
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

		fenster::Dimension resolution = videoOutput->getResolution();
		fenster::Rectangle screenBounds(0, 0, resolution.width, resolution.height);
		createVitalComponents(screenBounds);
		fenster::platformAcquireMutex(Cursor::inputLock);
		Cursor::nextPosition = fenster::Point(screenBounds.width / 2, screenBounds.height / 2);
		fenster::platformReleaseMutex(Cursor::inputLock);
		cursorOverlay = new CursorOverlay(videoOutput, screenBounds);
#ifdef _GHOST_
		pointerTask = fenster::platformCreateThread((void*) &startPointerUpdateLoop);
#endif
		fenster::platformCreateThread((void*) &startInputHandlers);

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
		backgroundPanel->setBackground(_RGB(0, 0, 0));
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

		fenster::platformAcquireMutex(updateLock);
		while(true)
		{
			eventProcessor->process();

			screen->resolveRequirement(COMPONENT_REQUIREMENT_UPDATE, 0);
			screen->resolveRequirement(COMPONENT_REQUIREMENT_LAYOUT, 0);
			screen->resolveRequirement(COMPONENT_REQUIREMENT_PAINT, 0);

			// Invalidations arriving during composition belong to the next frame.
			const auto invalid = screen->grabInvalid().clip(screenBounds);
			auto damage = invalid;
			if(invalid.width > 0 && invalid.height > 0 && lastInvalid.width > 0 && lastInvalid.height > 0)
			{
				// Restore the previous debug border along with the changed content.
				damage.extend(lastInvalid.getStart());
				damage.extend(fenster::Point(lastInvalid.getRight() - 1, lastInvalid.getBottom() - 1));
			}
			compose(&global, damage);

			output(&global, invalid, damage);

			framesTotal++;
			fenster::platformAcquireMutexTimeout(updateLock, 1000);
		}
	}

	void Server::compose(Graphics* graphics, const fenster::Rectangle& damage)
	{
		if(damage.width <= 0 || damage.height <= 0)
			return;

		auto cr = graphics->acquireContext();
		if(!cr)
			return;

		// Rebuild damaged pixels from a clean background, including alpha surfaces.
		cairo_save(cr);
		cairo_rectangle(cr, damage.x, damage.y, damage.width, damage.height);
		cairo_clip(cr);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_paint(cr);
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

		screen->blit(graphics, damage, fenster::Point(0, 0));
		cairo_restore(cr);
		graphics->releaseContext();
	}

	void Server::requestUpdateImmediately() const
	{
		fenster::platformReleaseMutex(updateLock);
	}

	void Server::requestPointerUpdate() const
	{
		fenster::platformReleaseMutex(updateLock);
#ifdef _GHOST_
		fenster::platformReleaseMutex(pointerUpdateLock);
		if(pointerTask != SYS_TID_NONE)
			fenster::platformYieldTo(pointerTask);
#else
		if(cursorOverlay)
			cursorOverlay->present();
#endif
	}

	void Server::startPointerUpdateLoop()
	{
		Server::instance()->pointerUpdateLoop();
	}

	void Server::pointerUpdateLoop()
	{
		fenster::platformRegisterTaskIdentifier("windowserver/pointer");
		fenster::platformAcquireMutex(pointerUpdateLock);
		while(true)
		{
			cursorOverlay->present();
			fenster::platformAcquireMutex(pointerUpdateLock);
		}
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

	void Server::output(Graphics* graphics, const fenster::Rectangle& invalid, const fenster::Rectangle& damage)
	{
		if(damage.width <= 0 || damage.height <= 0)
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
				cairo_rectangle(cr, invalid.x, invalid.y, invalid.width, invalid.height);
				cairo_clip(cr);
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

		cursorOverlay->commit(graphics, damage);

		lastInvalid = debugOn ? invalid : fenster::Rectangle();
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

	Component* Server::switchFocus(Component* pressed)
	{
		auto source = ComponentRegistry::get(Cursor::focusedComponent);

		auto target = pressed->setFocused(true);
		if(!target)
			return nullptr;
		Cursor::focusedComponent = target->id;
		Window* targetWindow = target->getWindow();

		Window* sourceWindow = nullptr;
		if(source)
		{
			sourceWindow = source->getWindow();
			if(source != target)
			{
				source->setFocused(false);
			}
			if(sourceWindow && targetWindow != sourceWindow && sourceWindow != source)
			{
				sourceWindow->setFocused(false);
			}

			// Unfocusing a menu or any menu item hides all menus
			auto targetAsMenu = dynamic_cast<Menu*>(target) || dynamic_cast<Menu*>(target->getParent());
			if(!targetAsMenu)
				screen->hideMenus();
		}

		if(targetWindow)
		{
			if(sourceWindow != targetWindow)
				targetWindow->bringToFront();

			if(targetWindow != target)
				targetWindow->setFocused(true);
		}

		return target;
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
