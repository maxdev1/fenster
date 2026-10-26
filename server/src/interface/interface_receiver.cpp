// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <libfenster/interface.hpp>
#include <stdio.h>
#include <string.h>
#include <server.hpp>

#include "components/button.hpp"
#include "components/canvas.hpp"
#include "components/window.hpp"
#include "components/desktop/selection.hpp"
#include "component_registry.hpp"
#include "components/scrollpane.hpp"
#include "components/image.hpp"
#include "components/checkbox.hpp"
#include "components/text/text_box.hpp"
#include "interface/interface_receiver.hpp"
#include "interface/component_updates.hpp"
#include "process_registry.hpp"
#include "input/mouse_grab.hpp"
#include <vector>

using namespace fenster;

namespace fensterserver
{
	static Component* createComponent(ComponentType type, SYS_TID_T sender, SYS_TID_T pid, ComponentId id = 0);
	// The packet validator checks record sizes and string termination before any
	// mutation runs. The outer receiver holds the registry access guard and has
	// already resolved/validated the sender process for the complete packet.
	static bool applyComponentMutation(const uint8_t* bytes, SYS_TID_T pid, SYS_TID_T sender)
	{
		auto command = reinterpret_cast<const MessageHeader*>(bytes)->id;
		if(command == ProtocolCommandId::CreateReservedComponent)
		{
			auto request = reinterpret_cast<const CommandCreateReservedComponentRequest*>(bytes);
			return request->id > 0 && createComponent(request->type, sender, pid, request->id) != nullptr;
		}
		if(command == ProtocolCommandId::AddComponent)
		{
			auto request = reinterpret_cast<const CommandAddChildRequest*>(bytes);
			auto parent = ComponentRegistry::get(request->parent);
			auto child = ComponentRegistry::get(request->child);
			if(!parent || !child || !ComponentRegistry::owns(pid, parent->id) ||
			   !ComponentRegistry::owns(pid, child->id) || parent == child || parent->isChildOf(child))
				return false;
			parent->addChild(child);
			return true;
		}
		// The remaining mutation records all begin with a component ID.
		ComponentId id;
		memcpy(&id, bytes + sizeof(MessageHeader), sizeof(id));
		auto component = ComponentRegistry::get(id);
		if(!component) return false;
		switch(command)
		{
			case ProtocolCommandId::SetLayoutPadding:
				return component->setLayoutPadding(reinterpret_cast<const CommandSetLayoutPaddingRequest*>(bytes)->padding);
			case ProtocolCommandId::SetFlexComponentInfo:
			{
				auto request = reinterpret_cast<const CommandSetFlexComponentInfoRequest*>(bytes);
				return component->setFlexComponentInfo(request->child, request->grow, request->shrink, request->basis);
			}
			case ProtocolCommandId::SetBounds:
				component->setBounds(reinterpret_cast<const CommandSetBoundsRequest*>(bytes)->bounds);
				return true;
			case ProtocolCommandId::SetNumericProperty:
			{
				auto request = reinterpret_cast<const CommandSetNumericPropertyRequest*>(bytes);
				return component->setNumericProperty(request->property, request->value);
			}
			case ProtocolCommandId::SetStringProperty:
			{
				auto request = reinterpret_cast<const CommandSetStringPropertyRequest*>(bytes);
				return component->setStringProperty(request->property, request->value);
			}
			case ProtocolCommandId::AddListener:
			{
				auto request = reinterpret_cast<const CommandAddListenerRequest*>(bytes);
				component->addListener(request->event_type, request->target_thread, request->id, pid);
				return true;
			}
			case ProtocolCommandId::SetMinimumSize:
				component->setMinimumSize(reinterpret_cast<const CommandSetSizeRequest*>(bytes)->size);
				return true;
			case ProtocolCommandId::SetMaximumSize:
				component->setMaximumSize(reinterpret_cast<const CommandSetSizeRequest*>(bytes)->size);
				return true;
			case ProtocolCommandId::SetPreferredSize:
				component->setPreferredSize(reinterpret_cast<const CommandSetSizeRequest*>(bytes)->size);
				return true;
			default: return false;
		}
	}

	static Component* makeComponent(ComponentType type, SYS_TID_T sender)
	{
		Component* component = nullptr;
		switch(type)
		{
			case ComponentType::Window:
				component = new Window();
				break;

			case ComponentType::Label:
				component = new Label();
				break;

			case ComponentType::Button:
				component = new Button();
				break;

			case ComponentType::TextBox:
				component = new TextBox();
				break;

			case ComponentType::Canvas:
				component = new Canvas(sender);
				break;

			case ComponentType::Selection:
				component = new Selection();
				break;

			case ComponentType::Panel:
				component = new Panel();
				break;

			case ComponentType::ScrollPane:
				component = new ScrollPane();
				break;

			case ComponentType::Image:
				component = new Image();
				break;

			case ComponentType::Checkbox:
				component = new Checkbox();
				break;

			default:
				platformLog("don't know how to create a component of type %i", type);
				break;
		}

		return component;
	}

	static Component* createComponent(ComponentType type, SYS_TID_T sender, SYS_TID_T pid, ComponentId id)
	{
		auto create = [=] { return makeComponent(type, sender); };
		auto component = id ? ComponentRegistry::createReserved(pid, id, create) : create();
		if(component)
		{
			if(!id) ComponentRegistry::bind(pid, component);
			if(type == ComponentType::Window) Server::instance()->screen->addChild(component);
		}
		return component;
	}

	void interfaceReceiverThread()
	{
		while(true)
		{
			size_t buflen = SYS_MESSAGE_HEADER_SIZE + G_UI_MAXIMUM_MESSAGE_SIZE;
			auto buf = new uint8_t[buflen];
			bool deferred = false;

			auto stat = platformReceiveMessage(buf, buflen, SYS_TX_NONE);
			if(stat == SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				interfaceReceiverProcessCommand(buf);
			}
			else if(stat == SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER)
			{
				platformLog("could not receive an incoming request, message exceeded buffer size");
			}
			else
			{
				platformLog("an unknown error occurred when trying to receive a UI request (code: %i)", stat);
			}

			if(!deferred)
				delete[] buf;
		}
	}

	bool interfaceReceiverProcessCommand(void* requestMessage)
	{
		auto requestUiMessage = (MessageHeader*) SYS_MESSAGE_CONTENT(requestMessage);
		void* responseMessage = nullptr;
		int responseLength = 0;
		CommandSimpleResponse canvasBlitResponse{};
		ComponentRegistry::Access components;
		auto pid = platformGetPidForTid(SYS_MESSAGE_SENDER(requestMessage));
		if(pid == SYS_TID_NONE || ProcessRegistry::get(pid) == SYS_TID_NONE)
			return false; // Discard queued requests after the client's exit cleanup.

		if(requestUiMessage->id == ProtocolCommandId::UpdateComponents)
		{
			auto response = new CommandSimpleResponse{{ProtocolCommandId::UpdateComponents}, ProtocolStatus::Error};
			const auto length = SYS_MESSAGE_LENGTH(requestMessage);
			if(length >= sizeof(CommandUpdateComponentsRequest))
			{
				auto request = reinterpret_cast<CommandUpdateComponentsRequest*>(requestUiMessage);
				const bool success = applyComponentUpdates(request->updates, length - sizeof(*request),
					[&](const uint8_t* bytes, size_t size) {
						return applyComponentMutation(bytes, pid, SYS_MESSAGE_SENDER(requestMessage));
					});
				if(success)
					response->status = ProtocolStatus::Success;
			}
			responseMessage = response;
			responseLength = sizeof(*response);
		}
		else if(requestUiMessage->id == ProtocolCommandId::PrepareComponent)
		{
			bool ready = false;
			if(SYS_MESSAGE_LENGTH(requestMessage) == sizeof(CommandPrepareComponentRequest))
			{
				auto request = reinterpret_cast<CommandPrepareComponentRequest*>(requestUiMessage);
				if(ComponentRegistry::owns(pid, request->id))
					if(auto component = ComponentRegistry::get(request->id)) ready = component->prepareForDisplay();
			}
			responseMessage = new CommandSimpleResponse{{requestUiMessage->id}, ready ? ProtocolStatus::Success : ProtocolStatus::Error};
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetLayoutPadding ||
				requestUiMessage->id == ProtocolCommandId::SetFlexComponentInfo)
		{
			auto expected = requestUiMessage->id == ProtocolCommandId::SetLayoutPadding ?
				sizeof(CommandSetLayoutPaddingRequest) : sizeof(CommandSetFlexComponentInfoRequest);
			auto success = SYS_MESSAGE_LENGTH(requestMessage) == expected &&
				applyComponentMutation(reinterpret_cast<const uint8_t*>(requestUiMessage), pid, SYS_MESSAGE_SENDER(requestMessage));
			responseMessage = new CommandSimpleResponse{{requestUiMessage->id}, success ? ProtocolStatus::Success : ProtocolStatus::Error};
			responseLength = sizeof(CommandSimpleResponse);
		}

		else if(requestUiMessage->id == ProtocolCommandId::ReserveComponentIds)
		{
			auto response = new CommandReserveComponentIdsResponse{};
			response->header.id = ProtocolCommandId::ReserveComponentIds;
			if(SYS_MESSAGE_LENGTH(requestMessage) == sizeof(MessageHeader))
				response->component_ids = ComponentRegistry::reserveIds(pid);
			response->status = response->component_ids.count ? ProtocolStatus::Success : ProtocolStatus::Error;
			responseMessage = response;
			responseLength = sizeof(*response);
		}
		else if(requestUiMessage->id == ProtocolCommandId::CreateComponents)
		{
			auto request = reinterpret_cast<CommandCreateComponentsRequest*>(requestUiMessage);
			uint16_t count = 0;
			if(SYS_MESSAGE_LENGTH(requestMessage) == sizeof(*request) &&
			   request->count > 0 && request->count <= G_UI_CREATE_COMPONENTS_MAXIMUM &&
			   (request->type == ComponentType::Panel || request->type == ComponentType::Image ||
				request->type == ComponentType::Label))
				count = request->count;
			responseLength = sizeof(CommandCreateComponentsResponse) + count * sizeof(ComponentId);
			auto response = static_cast<CommandCreateComponentsResponse*>(operator new(responseLength));
			response->header.id = ProtocolCommandId::CreateComponents;
			response->status = count ? ProtocolStatus::Success : ProtocolStatus::Error;
			response->count = count;
			for(size_t i = 0; i < count; ++i)
				response->ids[i] = createComponent(request->type, SYS_MESSAGE_SENDER(requestMessage), pid)->id;
			responseMessage = response;
		}
		else if(requestUiMessage->id == ProtocolCommandId::CreateComponent)
		{
			auto createRequest = (CommandCreateComponentRequest*) requestUiMessage;

			Component* component = createComponent(createRequest->type, SYS_MESSAGE_SENDER(requestMessage), pid);
			ComponentId component_id = -1;
			if(component)
			{
				component_id = component->id;
			}

			// create response message
			auto response = new CommandCreateComponentResponse;
			response->header.id = ProtocolCommandId::CreateComponent;
			response->id = component_id;
			response->status = (component != nullptr
				                    ? ProtocolStatus::Success
				                    : ProtocolStatus::Error);

			responseMessage = response;
			responseLength = sizeof(CommandCreateComponentResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::AddComponent)
		{
			auto request = (CommandAddChildRequest*) requestUiMessage;
			Component* parent = ComponentRegistry::get(request->parent);
			Component* child = ComponentRegistry::get(request->child);

			// create response message
			auto response = new CommandAddChildResponse;
			if(parent == nullptr || child == nullptr || !ComponentRegistry::owns(pid, parent->id) ||
			   !ComponentRegistry::owns(pid, child->id) || parent == child || parent->isChildOf(child))
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				parent->addChild(child);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandAddChildResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetMouseGrab)
		{
			auto response = new CommandSimpleResponse{{ProtocolCommandId::SetMouseGrab}, ProtocolStatus::Error};
			if(SYS_MESSAGE_LENGTH(requestMessage) == sizeof(CommandSetMouseGrabRequest))
			{
				auto request = reinterpret_cast<CommandSetMouseGrabRequest*>(requestUiMessage);
				if(MouseGrab::set(request->id, pid, request->grabbed))
					response->status = ProtocolStatus::Success;
			}
			responseMessage = response;
			responseLength = sizeof(*response);
		}
		else if(requestUiMessage->id == ProtocolCommandId::DestroyComponent)
		{
			auto request = (CommandDestroyComponentRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			if(component)
			{
				MouseGrab::releaseFor(component);
				auto parent = component->getParent();
				if(parent)
					parent->removeChild(component);
				// TODO deferred component deletion
			}
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetBounds)
		{
			auto request = (CommandSetBoundsRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandSimpleResponse;
			if(component == nullptr)
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				component->setBounds(request->bounds);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::GetBounds)
		{
			auto request = (CommandGetBoundsRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandGetBoundsResponse;
			if(component == nullptr)
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				response->bounds = component->getBounds();
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandGetBoundsResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::AddListener)
		{
			auto request = (CommandAddListenerRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandAddListenerResponse;
			if(component == nullptr)
			{
				platformLog("failed to attach listener since component doesn't exist");
				response->status = ProtocolStatus::Error;
			}
			else
			{
				component->addListener(request->event_type, request->target_thread, request->id, pid);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandAddListenerResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetNumericProperty)
		{
			auto request = (CommandSetNumericPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandSetStringPropertyResponse;
			if(component == nullptr)
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				if(component->setNumericProperty(request->property, request->value))
				{
					response->status = ProtocolStatus::Success;
				}
				else
				{
					response->status = ProtocolStatus::Error;
				}
			}

			responseMessage = response;
			responseLength = sizeof(CommandSetStringPropertyResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::GetNumericProperty)
		{
			auto request = (CommandGetNumericPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandGetNumericPropertyResponse;
			if(component == nullptr)
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				uint32_t value;
				if(component->getNumericProperty(request->property, &value))
				{
					response->value = value;
					response->status = ProtocolStatus::Success;
				}
				else
				{
					response->status = ProtocolStatus::Error;
				}
			}

			responseMessage = response;
			responseLength = sizeof(CommandGetNumericPropertyResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::Focus)
		{
			auto request = (CommandFocusRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandFocusResponse;
			if(component == nullptr)
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				response->status = Server::instance()->switchFocus(component) ? ProtocolStatus::Success : ProtocolStatus::Error;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSetStringPropertyResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetStringProperty)
		{
			auto request = (CommandSetStringPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandSimpleResponse;
			if(component == nullptr)
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				response->status = component->setStringProperty(request->property, std::string(request->value))
					? ProtocolStatus::Success : ProtocolStatus::Error;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::GetStringProperty)
		{
			auto request = (CommandGetStringPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			CommandGetStringPropertyResponse* response;
			if(component == nullptr)
			{
				response = new CommandGetStringPropertyResponse;
				response->status = ProtocolStatus::Error;
				responseLength = sizeof(CommandGetStringPropertyResponse);
			}
			else
			{
				std::string value;
				if(component->getStringProperty(request->property, value))
				{
					responseLength = sizeof(CommandGetStringPropertyResponse) + value.length() + 1;
					response = static_cast<CommandGetStringPropertyResponse*>(
						operator new(responseLength)
					);
					strcpy(response->value, value.c_str());
					response->status = ProtocolStatus::Success;
				}
				else
				{
					response = new CommandGetStringPropertyResponse;
					response->status = ProtocolStatus::Error;
					responseLength = sizeof(CommandGetStringPropertyResponse);
				}
			}

			responseMessage = response;
		}
		else if(requestUiMessage->id == ProtocolCommandId::CanvasBlit)
		{
			auto request = (CommandCanvasBlitRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			auto response = &canvasBlitResponse;
			response->header.id = ProtocolCommandId::CanvasBlit;
			response->status = ProtocolStatus::Error;

			if(component)
			{
				auto canvas = dynamic_cast<Canvas*>(component);
				if(canvas)
				{
					if(canvas->requestBlit(request->area, request->bufferAddress))
						response->status = ProtocolStatus::Success;
				}
			}
			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::RegisterDesktopCanvas)
		{
			auto request = (CommandRegisterDesktopCanvasRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->canvas_id);

			auto response = new CommandRegisterDesktopCanvasResponse;
			if(component == nullptr || !ComponentRegistry::owns(pid, component->id))
			{
				response->status = ProtocolStatus::Error;
			}
			else
			{
				response->status = ProtocolStatus::Success;

				auto canvas = dynamic_cast<Canvas*>(component);
				if(canvas)
				{
					canvas->setZIndex(1);

					Screen* screen = Server::instance()->screen;
					screen->addChild(canvas);
					screen->setDesktopCanvas(canvas);
					screen->addListener(ComponentEventType::Window, request->target_thread, canvas->id, pid);
					canvas->setBounds(screen->getBounds());
					for(auto child : screen->acquireChildren())
						if(auto window = dynamic_cast<Window*>(child.component))
							screen->sendWindowEvent(canvas->id, window, request->target_thread, true);
					screen->releaseChildren();
				}
			}

			responseMessage = response;
			responseLength = sizeof(CommandRegisterDesktopCanvasResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::GetScreenDimension)
		{
			auto response = new CommandGetScreenDimensionResponse;
			response->size = Server::instance()->screen->getBounds().getSize();

			responseMessage = response;
			responseLength = sizeof(CommandGetScreenDimensionResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::ScrollPaneSetContent)
		{
			auto response = new CommandSimpleResponse;
			response->status = ProtocolStatus::Error;

			auto request = (CommandScrollPaneSetContent*) requestUiMessage;
			ScrollPane* scrollpane = dynamic_cast<ScrollPane*>(ComponentRegistry::get(request->scrollpane));
			Component* content = ComponentRegistry::get(request->content);
			if(scrollpane && content && ComponentRegistry::owns(pid, scrollpane->id) &&
			   ComponentRegistry::owns(pid, content->id) && scrollpane != content && !scrollpane->isChildOf(content))
			{
				scrollpane->setContent(content);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::ScrollPaneSetFixed)
		{
			auto response = new CommandSimpleResponse;
			response->status = ProtocolStatus::Error;

			auto request = (CommandScrollPaneSetFixed*) requestUiMessage;
			ScrollPane* scrollpane = dynamic_cast<ScrollPane*>(ComponentRegistry::get(request->scrollpane));
			if(scrollpane)
			{
				scrollpane->setFixedHeight(request->height);
				scrollpane->setFixedWidth(request->width);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetPreferredSize)
		{
			auto response = new CommandSimpleResponse;
			response->status = ProtocolStatus::Error;

			auto request = (CommandSetSizeRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			if(component)
			{
				component->setPreferredSize(request->size);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetMinimumSize)
		{
			auto response = new CommandSimpleResponse;
			response->status = ProtocolStatus::Error;

			auto request = (CommandSetSizeRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			if(component)
			{
				component->setMinimumSize(request->size);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == ProtocolCommandId::SetMaximumSize)
		{
			auto response = new CommandSimpleResponse;
			response->status = ProtocolStatus::Error;

			auto request = (CommandSetSizeRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			if(component)
			{
				component->setMaximumSize(request->size);
				response->status = ProtocolStatus::Success;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}

		components.release(); // Transport can block; no component pointer escapes this operation.
		Server::instance()->requestUpdateLater();
		if(responseMessage)
		{
			platformSendMessage(SYS_MESSAGE_SENDER(requestMessage), responseMessage, responseLength,
							 SYS_MESSAGE_TRANSACTION(requestMessage));
			platformYieldTo(SYS_MESSAGE_SENDER(requestMessage));
		}
		if(responseMessage && responseMessage != &canvasBlitResponse)
			operator delete(responseMessage);
		return true;
	}
}
