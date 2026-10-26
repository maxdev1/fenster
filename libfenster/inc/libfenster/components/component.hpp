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
#include "libfenster/component_registry.hpp"
#include "libfenster/interface.hpp"
#include "libfenster/listener/mouse_listener.hpp"
#include "libfenster/listener/visible_listener.hpp"
#include "libfenster/listener/key_listener.hpp"
#include "libfenster/listener/relative_mouse_listener.hpp"
#include "libfenster/metrics/rectangle.hpp"
#include "libfenster/application.hpp"
#include "libfenster/color_argb.hpp"
#include "libfenster/layout/layout.hpp"
#include "libfenster/properties.hpp"
#include "libfenster/platform/platform.hpp"

namespace fenster
{
	/**
	 * Template class for all components. Implements the basic component functionalities such as creation and bounds handling.
	 */
	class Component : public BoundingComponent
	{
		friend class ComponentUpdateBatch;
		friend class FlexLayout;
		friend class StackLayout;
		friend class GridLayout;
		friend class FlowLayout;
		friend class FillLayout;

		bool setLayout(Layout* layout);
		static ComponentId createComponentId(ComponentType type);
		static std::vector<ComponentId> createComponentIds(ComponentType type, size_t count);

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

			auto id = createComponentId(COMPONENT_CONSTANT);
			return id ? attachComponent<COMPONENT_TYPE>(id) : nullptr;
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

		template <typename COMPONENT_TYPE, ComponentType COMPONENT_CONSTANT>
		static std::vector<COMPONENT_TYPE*> createComponents(size_t count)
		{
			std::vector<COMPONENT_TYPE*> result;
			for(auto componentId : createComponentIds(COMPONENT_CONSTANT, count))
				result.push_back(attachComponent<COMPONENT_TYPE>(componentId));
			return result;
		}

		ComponentId getId() const
		{
			return id;
		}

		void destroy();
		/**
		 * Deletes an attached local proxy without destroying its remote object.
		 * The caller must stop local readers/callbacks first.
		 */
		void disposeLocal();
		bool addChild(Component* c);

		bool setBounds(const Rectangle& rect);
		Rectangle getBounds();


		bool isVisible();
		bool setVisible(bool visible);

		/**
		 * Synchronously lay out and paint this hidden component's visible descendants.
		 */
		bool prepareForDisplay();
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

		bool setMouseGrabbed(bool grabbed);
		bool addRelativeMouseListener(RelativeMouseListenerFunc listener);
		bool addMouseGrabListener(MouseGrabListenerFunc listener);

		void handle(ComponentEventHeader* header);
	};
}

#endif
