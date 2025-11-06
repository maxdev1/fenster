// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_COMPONENT
#define LIBFENSTER_COMPONENT

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>

#include "bounding_component.hpp"
#include "component_registry.hpp"
#include "interface.hpp"
#include "listener/mouse_listener.hpp"
#include "listener/visible_listener.hpp"
#include "listener/key_listener.hpp"
#include "metrics/rectangle.hpp"
#include "application.hpp"
#include "color_argb.hpp"
#include "layout/layout.hpp"
#include "properties.hpp"
#include "platform/platform.hpp"

namespace fenster
{
	/**
	 * Template class for all components. Implements the basic component functionalities such as creation and bounds handling.
	 */
	class Component : public BoundingComponent
	{
		friend class FlexLayout;
		friend class StackLayout;
		friend class GridLayout;
		friend class FlowLayout;

		bool setLayout(Layout* layout);

	protected:
		ComponentId id;
		bool destroyed = false;

		SYS_MUTEX_T listenersLock = platformInitializeMutex(true);
		std::unordered_map<ComponentEventType, std::vector<Listener*>> listeners;

		~Component() override;

		template <typename COMPONENT_TYPE, ComponentType COMPONENT_CONSTANT>
		static COMPONENT_TYPE* createComponent()
		{
			if (!ApplicationInitialized)
				return nullptr;

			SYS_TX_T tx = platformCreateMessageTransaction();
			CommandCreateComponentRequest request;
			request.header.id = FENSTER_PROTOCOL_CREATE_COMPONENT;
			request.type = COMPONENT_CONSTANT;
			platformSendMessage(DelegateTaskId, &request, sizeof(CommandCreateComponentRequest), tx);
			// g_yield_t(DelegateTaskId);

			size_t bufferSize = SYS_MESSAGE_HEADER_SIZE + sizeof(CommandCreateComponentResponse);
			uint8_t buffer[bufferSize];

			COMPONENT_TYPE* component = nullptr;
			if (platformReceiveMessage(buffer, bufferSize, tx) == SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				auto response = (CommandCreateComponentResponse*) SYS_MESSAGE_CONTENT(buffer);

				if (response->status == FENSTER_PROTOCOL_SUCCESS)
				{
					component = attachComponent<COMPONENT_TYPE>(response->id);
				}
			}
			return component;
		}

		template <typename COMPONENT_TYPE>
		static COMPONENT_TYPE* attachComponent(ComponentId id)
		{
			if (!ApplicationInitialized)
				return nullptr;

			auto component = new COMPONENT_TYPE(id);
			ComponentRegistry::add(component);
			return component;
		}

		bool setSize(ProtocolCommandId command, const Dimension& size);

	public:
		explicit Component(ComponentId id) : id(id), BoundingComponent(this)
		{
		}

		ComponentId getId() const
		{
			return id;
		}

		void destroy();
		bool addChild(Component* c);

		bool setBounds(const Rectangle& rect);
		Rectangle getBounds();


		bool isVisible();
		bool setVisible(bool visible);
		bool setBackground(ColorArgb argb);

		bool setFocusable(bool focusable);
		bool isFocusable();
		bool setDispatchesFocus(bool d);
		bool isDispatchesFocus();

		bool setPreferredSize(const Dimension& size);
		bool setMinimumSize(const Dimension& size);
		bool setMaximumSize(const Dimension& size);

		bool setNumericProperty(ComponentProperty property, uint32_t value);
		bool getNumericProperty(ComponentProperty property, uint32_t* out);

		bool setStringProperty(ComponentProperty property, std::string value);
		bool getStringProperty(ComponentProperty property, std::string& out);

		bool addListener(ComponentEventType eventType, Listener* listener);
		bool addMouseListener(MouseListener* listener);
		bool addMouseListener(MouseListenerFunc listener);
		bool addVisibleListener(VisibleListener* listener);
		bool addVisibleListener(VisibleListenerFunc listener);
		bool addKeyListener(KeyListener* listener);
		bool addKeyListener(KeyListenerFunc listener);

		void handle(ComponentEventHeader* header);
	};
}

#endif
