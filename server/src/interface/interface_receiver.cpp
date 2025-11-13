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

using namespace fenster;

namespace fensterserver
{
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
				delete buf;
		}
	}

	void interfaceReceiverProcessCommand(void* requestMessage)
	{
		auto requestUiMessage = (MessageHeader*) SYS_MESSAGE_CONTENT(requestMessage);
		void* responseMessage = nullptr;
		int responseLength = 0;

		if(requestUiMessage->id == ProtocolCommandId::CreateComponent)
		{
			auto createRequest = (CommandCreateComponentRequest*) requestUiMessage;

			Component* component = nullptr;
			switch(createRequest->type)
			{
				case ComponentType::Window:
					component = new Window();
					Server::instance()->screen->addChild(component);
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
					component = new Canvas(SYS_MESSAGE_SENDER(requestMessage));
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
					platformLog("don't know how to create a component of type %i", createRequest->type);
					break;
			}

			ComponentId component_id = -1;
			if(component)
			{
				component_id = component->id;
				ComponentRegistry::bind(platformGetPidForTid(SYS_MESSAGE_SENDER(requestMessage)), component);
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
			if(parent == nullptr || child == nullptr)
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
		else if(requestUiMessage->id == ProtocolCommandId::DestroyComponent)
		{
			auto request = (CommandDestroyComponentRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			if(component)
			{
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
				component->addListener(request->event_type, request->target_thread, request->id);
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
				Server::instance()->switchFocus(component);
				response->status = ProtocolStatus::Success;
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
				component->setStringProperty(request->property, std::string(request->value));
				response->status = ProtocolStatus::Success;
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

			if(component)
			{
				auto canvas = dynamic_cast<Canvas*>(component);
				if(canvas)
				{
					canvas->requestBlit(request->area);
				}
			}
		}
		else if(requestUiMessage->id == ProtocolCommandId::RegisterDesktopCanvas)
		{
			auto request = (CommandRegisterDesktopCanvasRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->canvas_id);

			auto response = new CommandRegisterDesktopCanvasResponse;
			if(component == nullptr)
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
					screen->addListener(ComponentEventType::Window, request->target_thread, canvas->id);
					canvas->setBounds(screen->getBounds());
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
			if(scrollpane && content)
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

		Server::instance()->requestUpdateLater();

		if(responseMessage)
		{
			platformSendMessage(
					SYS_MESSAGE_SENDER(requestMessage), responseMessage, responseLength,
					SYS_MESSAGE_TRANSACTION(requestMessage));
			platformYieldTo(SYS_MESSAGE_SENDER(requestMessage));
		}
	}
}
