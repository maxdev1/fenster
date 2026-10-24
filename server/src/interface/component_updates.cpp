// fenster — lightweight window server and UI toolkit
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "interface/component_updates.hpp"
#include <cstring>

namespace fensterserver
{
	static bool validUpdate(const uint8_t* bytes, size_t length)
	{
		using namespace fenster;
		if(length < sizeof(MessageHeader))
			return false;
		switch(reinterpret_cast<const MessageHeader*>(bytes)->id)
		{
			case ProtocolCommandId::SetBounds:
				return length == sizeof(CommandSetBoundsRequest);
			case ProtocolCommandId::SetNumericProperty:
				return length == sizeof(CommandSetNumericPropertyRequest);
			case ProtocolCommandId::SetStringProperty:
				return length > sizeof(CommandSetStringPropertyRequest) &&
				       memchr(bytes + sizeof(CommandSetStringPropertyRequest), 0,
				              length - sizeof(CommandSetStringPropertyRequest)) != nullptr;
			case ProtocolCommandId::SetMinimumSize:
			case ProtocolCommandId::SetMaximumSize:
			case ProtocolCommandId::SetPreferredSize:
				return length == sizeof(CommandSetSizeRequest);
			case ProtocolCommandId::AddComponent:
				return length == sizeof(CommandAddChildRequest);
			case ProtocolCommandId::AddListener:
				return length == sizeof(CommandAddListenerRequest);
			default:
				return false;
		}
	}

	bool applyComponentUpdates(const uint8_t* bytes, size_t length,
	                           const std::function<bool(const uint8_t*, size_t)>& apply)
	{
		if(length > fenster::G_UI_MAXIMUM_UPDATE_SIZE - sizeof(fenster::CommandUpdateComponentsRequest))
			return false;
		// Validate the complete packet first, including truncated or nested records.
		for(size_t position = 0; position < length;)
		{
			if(length - position < sizeof(fenster::ComponentUpdateHeader))
				return false;
			fenster::ComponentUpdateHeader header;
			memcpy(&header, bytes + position, sizeof(header));
			position += sizeof(header);
			if(header.length > length - position || !validUpdate(bytes + position, header.length))
				return false;
			position += header.length;
		}
		bool successful = true;
		for(size_t position = 0; position < length;)
		{
			fenster::ComponentUpdateHeader header;
			memcpy(&header, bytes + position, sizeof(header));
			position += sizeof(header);
			if(!apply(bytes + position, header.length))
				successful = false;
			position += header.length;
		}
		return successful;
	}
}
