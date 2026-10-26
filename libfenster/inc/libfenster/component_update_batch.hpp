// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_COMPONENT_UPDATE_BATCH
#define LIBFENSTER_COMPONENT_UPDATE_BATCH

#include "components/component.hpp"

namespace fenster
{
	// Mutations are sent in order, with one acknowledgement per bounded packet.
	// Call commit() to send the last packet and check the combined result.
	// This batches communication; it does not provide rollback or frame atomicity.
	class ComponentUpdateBatch
	{
		friend bool sendUiMutation(void* bytes, size_t length);
		std::vector<uint8_t> buffer;
		bool successful = true;
		bool append(const void* request, size_t length);
		bool setSize(ProtocolCommandId command, Component* component, const Dimension& size);
		bool addListener(Component* component, ComponentEventType type, Listener* listener);
		static thread_local ComponentUpdateBatch* activeBatch;

	public:
		// Opt in to batching ordinary setters/layout helpers/listener registrations on
		// this thread. Creation uses reserved IDs; queries/other RPCs flush first.
		// Setter success means queued; check commit() for transport/server failures.
		class Scope
		{
			ComponentUpdateBatch& batch;
			ComponentUpdateBatch* previous;
		public:
			explicit Scope(ComponentUpdateBatch& batch);
			~Scope();
			Scope(const Scope&) = delete;
			Scope& operator=(const Scope&) = delete;
		};

		ComponentUpdateBatch();
		ComponentUpdateBatch(const ComponentUpdateBatch&) = delete;
		ComponentUpdateBatch& operator=(const ComponentUpdateBatch&) = delete;

		bool setNumericProperty(Component* component, ComponentProperty property, uint32_t value);
		bool setStringProperty(Component* component, ComponentProperty property, const std::string& value);
		bool setBounds(Component* component, const Rectangle& bounds);
		bool setMinimumSize(Component* component, const Dimension& size);
		bool setMaximumSize(Component* component, const Dimension& size);
		bool setPreferredSize(Component* component, const Dimension& size);
		bool addChild(Component* parent, Component* child);
		bool registerListener(ComponentId component, ComponentEventType type);
		bool addMouseListener(Component* component, MouseListenerFunc listener);
		bool addKeyListener(Component* component, KeyListenerFunc listener);
		ComponentId createComponent(ComponentType type);
		static bool addIdRange(ComponentIdRange range);
		bool commit();
		static ComponentUpdateBatch* current() { return activeBatch; }
	};
}

#endif
