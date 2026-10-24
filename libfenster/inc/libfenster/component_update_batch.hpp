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
		std::vector<uint8_t> buffer;
		bool successful = true;
		bool append(const void* request, size_t length);

	public:
		ComponentUpdateBatch();
		ComponentUpdateBatch(const ComponentUpdateBatch&) = delete;
		ComponentUpdateBatch& operator=(const ComponentUpdateBatch&) = delete;

		bool setNumericProperty(Component* component, ComponentProperty property, uint32_t value);
		bool setStringProperty(Component* component, ComponentProperty property, const std::string& value);
		bool setBounds(Component* component, const Rectangle& bounds);
		bool setMinimumSize(Component* component, const Dimension& size);
		bool addChild(Component* parent, Component* child);
		bool addMouseListener(Component* component, MouseListenerFunc listener);
		bool commit();
	};
}

#endif
