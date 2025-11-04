// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <libfenster/interface.hpp>
#include <stdio.h>
#include <string.h>
#include <server.hpp>

#include "components/button.hpp"
#include "components/canvas.hpp"
#include "components/text/text_field.hpp"
#include "components/window.hpp"
#include "components/desktop/selection.hpp"
#include "component_registry.hpp"
#include "components/scrollpane.hpp"
#include "components/image.hpp"
#include "interface/interface_receiver.hpp"

#include <components/checkbox.hpp>

#include "layout/grid_layout_manager.hpp"
#include "layout/flex_layout_manager.hpp"

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

			auto stat = fenster::platformReceiveMessage(buf, buflen, SYS_TX_NONE);
			if(stat == SYS_MESSAGE_RECEIVE_SUCCESS)
			{
				interfaceReceiverProcessCommand(buf);
			}
			else if(stat == SYS_MESSAGE_RECEIVE_ERROR_EXCEEDS_BUFFER)
			{
				fenster::platformLog("could not receive an incoming request, message exceeded buffer size");
			}
			else
			{
				fenster::platformLog("an unknown error occurred when trying to receive a UI request (code: %i)", stat);
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

		if(requestUiMessage->id == FENSTER_PROTOCOL_CREATE_COMPONENT)
		{
			auto createRequest = (CommandCreateComponentRequest*) requestUiMessage;

			Component* component = nullptr;
			switch(createRequest->type)
			{
				case FENSTER_COMPONENT_TYPE_WINDOW:
					component = new Window();
				Server::instance()->screen->addChild(component);
				break;

				case FENSTER_COMPONENT_TYPE_LABEL:
					component = new Label();
				break;

				case FENSTER_COMPONENT_TYPE_BUTTON:
					component = new Button();
				break;

				case FENSTER_COMPONENT_TYPE_TEXTFIELD:
					component = new TextField();
				break;

				case FENSTER_COMPONENT_TYPE_CANVAS:
					component = new Canvas(SYS_MESSAGE_SENDER(requestMessage));
				break;

				case FENSTER_COMPONENT_TYPE_SELECTION:
					component = new Selection();
				break;

				case FENSTER_COMPONENT_TYPE_PANEL:
					component = new Panel();
				break;

				case FENSTER_COMPONENT_TYPE_SCROLLPANE:
					component = new ScrollPane();
				break;

				case FENSTER_COMPONENT_TYPE_IMAGE:
					component = new Image();
				break;

				case FENSTER_COMPONENT_TYPE_CHECKBOX:
					component = new Checkbox();
				break;

				default:
					fenster::platformLog("don't know how to create a component of type %i", createRequest->type);
				break;
			}

			ComponentId component_id = -1;
			if(component)
			{
				component_id = component->id;
				ComponentRegistry::bind(fenster::platformGetPidForTid(SYS_MESSAGE_SENDER(requestMessage)), component);
			}

			// create response message
			auto response = new CommandCreateComponentResponse;
			response->header.id = FENSTER_PROTOCOL_CREATE_COMPONENT;
			response->id = component_id;
			response->status = (component != nullptr ? FENSTER_PROTOCOL_SUCCESS : FENSTER_PROTOCOL_FAIL);

			responseMessage = response;
			responseLength = sizeof(CommandCreateComponentResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_ADD_COMPONENT)
		{
			auto request = (CommandAddChildRequest*) requestUiMessage;
			Component* parent = ComponentRegistry::get(request->parent);
			Component* child = ComponentRegistry::get(request->child);

			// create response message
			auto response = new CommandAddChildResponse;
			if(parent == nullptr || child == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				parent->addChild(child);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandAddChildResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_DESTROY_COMPONENT)
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
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SET_BOUNDS)
		{
			auto request = (CommandSetBoundsRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandSimpleResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				component->setBounds(request->bounds);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_GET_BOUNDS)
		{
			auto request = (CommandGetBoundsRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandGetBoundsResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				response->bounds = component->getBounds();
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandGetBoundsResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_ADD_LISTENER)
		{
			auto request = (CommandAddListenerRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandAddListenerResponse;
			if(component == nullptr)
			{
				fenster::platformLog("failed to attach listener since component doesn't exist");
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				component->addListener(request->event_type, request->target_thread, request->id);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandAddListenerResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SET_NUMERIC_PROPERTY)
		{
			auto request = (CommandSetNumericPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandSetStringPropertyResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				if(component->setNumericProperty(request->property, request->value))
				{
					response->status = FENSTER_PROTOCOL_SUCCESS;
				}
				else
				{
					response->status = FENSTER_PROTOCOL_FAIL;
				}
			}

			responseMessage = response;
			responseLength = sizeof(CommandSetStringPropertyResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_GET_NUMERIC_PROPERTY)
		{
			auto request = (CommandGetNumericPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandGetNumericPropertyResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				uint32_t value;
				if(component->getNumericProperty(request->property, &value))
				{
					response->value = value;
					response->status = FENSTER_PROTOCOL_SUCCESS;
				}
				else
				{
					response->status = FENSTER_PROTOCOL_FAIL;
				}
			}

			responseMessage = response;
			responseLength = sizeof(CommandGetNumericPropertyResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_FOCUS)
		{
			auto request = (CommandFocusRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandFocusResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				Server::instance()->switchFocus(component);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSetStringPropertyResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SET_STRING_PROPERTY)
		{
			auto request = (CommandSetStringPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			auto response = new CommandSimpleResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				component->setStringProperty(request->property, std::string(request->value));
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_GET_STRING_PROPERTY)
		{
			auto request = (CommandGetStringPropertyRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);

			CommandGetStringPropertyResponse* response;
			if(component == nullptr)
			{
				response = new CommandGetStringPropertyResponse;
				response->status = FENSTER_PROTOCOL_FAIL;
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
					response->status = FENSTER_PROTOCOL_SUCCESS;
				}
				else
				{
					response = new CommandGetStringPropertyResponse;
					response->status = FENSTER_PROTOCOL_FAIL;
					responseLength = sizeof(CommandGetStringPropertyResponse);
				}
			}

			responseMessage = response;
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_CANVAS_BLIT)
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
		else if(requestUiMessage->id == FENSTER_PROTOCOL_REGISTER_DESKTOP_CANVAS)
		{
			auto request = (CommandRegisterDesktopCanvasRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->canvas_id);

			auto response = new CommandRegisterDesktopCanvasResponse;
			if(component == nullptr)
			{
				response->status = FENSTER_PROTOCOL_FAIL;
			}
			else
			{
				response->status = FENSTER_PROTOCOL_SUCCESS;

				auto canvas = dynamic_cast<Canvas*>(component);
				if(canvas)
				{
					canvas->setZIndex(1);

					Screen* screen = Server::instance()->screen;
					screen->addChild(canvas);
					screen->addListener(FENSTER_COMPONENT_EVENT_TYPE_WINDOWS, request->target_thread, canvas->id);
					canvas->setBounds(screen->getBounds());
				}
			}

			responseMessage = response;
			responseLength = sizeof(CommandRegisterDesktopCanvasResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_GET_SCREEN_DIMENSION)
		{
			auto response = new CommandGetScreenDimensionResponse;
			response->size = Server::instance()->screen->getBounds().getSize();

			responseMessage = response;
			responseLength = sizeof(CommandGetScreenDimensionResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SCROLLPANE_SET_CONTENT)
		{
			auto response = new CommandSimpleResponse;
			response->status = FENSTER_PROTOCOL_FAIL;

			auto request = (CommandScrollPaneSetContent*) requestUiMessage;
			ScrollPane* scrollpane = dynamic_cast<ScrollPane*>(ComponentRegistry::get(request->scrollpane));
			Component* content = ComponentRegistry::get(request->content);
			if(scrollpane && content)
			{
				scrollpane->setContent(content);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SCROLLPANE_SET_FIXED)
		{
			auto response = new CommandSimpleResponse;
			response->status = FENSTER_PROTOCOL_FAIL;

			auto request = (CommandScrollPaneSetFixed*) requestUiMessage;
			ScrollPane* scrollpane = dynamic_cast<ScrollPane*>(ComponentRegistry::get(request->scrollpane));
			if(scrollpane)
			{
				scrollpane->setFixedHeight(request->height);
				scrollpane->setFixedWidth(request->width);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SET_PREFERRED_SIZE)
		{
			auto response = new CommandSimpleResponse;
			response->status = FENSTER_PROTOCOL_FAIL;

			auto request = (CommandSetSizeRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			if(component)
			{
				component->setPreferredSize(request->size);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SET_MINIMUM_SIZE)
		{
			auto response = new CommandSimpleResponse;
			response->status = FENSTER_PROTOCOL_FAIL;

			auto request = (CommandSetSizeRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			if(component)
			{
				component->setMinimumSize(request->size);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}
		else if(requestUiMessage->id == FENSTER_PROTOCOL_SET_MAXIMUM_SIZE)
		{
			auto response = new CommandSimpleResponse;
			response->status = FENSTER_PROTOCOL_FAIL;

			auto request = (CommandSetSizeRequest*) requestUiMessage;
			Component* component = ComponentRegistry::get(request->id);
			if(component)
			{
				component->setMaximumSize(request->size);
				response->status = FENSTER_PROTOCOL_SUCCESS;
			}

			responseMessage = response;
			responseLength = sizeof(CommandSimpleResponse);
		}

		Server::instance()->requestUpdateLater();

		if(responseMessage)
		{
			fenster::platformSendMessage(SYS_MESSAGE_SENDER(requestMessage), responseMessage, responseLength, SYS_MESSAGE_TRANSACTION(requestMessage));
			fenster::platformYieldTo(SYS_MESSAGE_SENDER(requestMessage));
		}
	}
}
