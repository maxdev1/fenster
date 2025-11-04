// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_INTERFACE
#define LIBFENSTER_INTERFACE

#include "platform/platform.hpp"
#include "metrics/dimension.hpp"
#include "metrics/rectangle.hpp"
#include "input/key_info.hpp"

/**
 * This UI interface specification defines the messages
 * that the active window manager must understand.
 */
#define G_UI_REGISTRY_NAME "windowserver/registration"

/**
 * Size of the largest expected message
 */
#define G_UI_MAXIMUM_MESSAGE_SIZE 4096

#define G_UI_COMPONENT_TITLE_MAXIMUM 1024

namespace fenster
{
	/**
	 * Declared in the UI unit
	 */
	extern bool ApplicationInitialized;

	/**
	 * ID for a UI component
	 */
	typedef int32_t ComponentId;

	/**
	 * ID for a listener
	 */
	typedef int32_t ListenerId;

	/**
	 * A protocol message always starts with the header, the message id
	 */
	typedef uint8_t ProtocolCommandId;
#define FENSTER_PROTOCOL_INITIALIZATION				(1)
#define FENSTER_PROTOCOL_CREATE_COMPONENT			(2)
#define FENSTER_PROTOCOL_ADD_COMPONENT				(3)

#define FENSTER_PROTOCOL_SET_BOUNDS					(6)
#define FENSTER_PROTOCOL_FOCUS        				(7)
#define FENSTER_PROTOCOL_ADD_LISTENER				(8)
#define FENSTER_PROTOCOL_SET_NUMERIC_PROPERTY		(9)
#define FENSTER_PROTOCOL_GET_NUMERIC_PROPERTY		(10)
#define FENSTER_PROTOCOL_GET_BOUNDS					(11)
#define FENSTER_PROTOCOL_CANVAS_BLIT				(12)
#define FENSTER_PROTOCOL_REGISTER_DESKTOP_CANVAS	(13)
#define FENSTER_PROTOCOL_GET_SCREEN_DIMENSION		(14)
#define FENSTER_PROTOCOL_SCROLLPANE_SET_CONTENT		(18)
#define FENSTER_PROTOCOL_SCROLLPANE_SET_FIXED		(19)
#define FENSTER_PROTOCOL_SET_PREFERRED_SIZE			(20)
#define FENSTER_PROTOCOL_DESTROY_COMPONENT		    (21)
#define FENSTER_PROTOCOL_SET_MINIMUM_SIZE			(22)
#define FENSTER_PROTOCOL_SET_MAXIMUM_SIZE			(23)
#define FENSTER_PROTOCOL_SET_STRING_PROPERTY		(24)
#define FENSTER_PROTOCOL_GET_STRING_PROPERTY		(25)

	/**
	 * Common status for requests
	 */
	typedef uint8_t ProtocolStatus;
#define FENSTER_PROTOCOL_SUCCESS 0
#define FENSTER_PROTOCOL_FAIL 1

	/**
	 * Component types
	 */
	typedef uint32_t ComponentType;
#define FENSTER_COMPONENT_TYPE_WINDOW 0
#define FENSTER_COMPONENT_TYPE_BUTTON 1
#define FENSTER_COMPONENT_TYPE_LABEL 2
#define FENSTER_COMPONENT_TYPE_TEXTFIELD 3
#define FENSTER_COMPONENT_TYPE_CANVAS 4
#define FENSTER_COMPONENT_TYPE_SELECTION 5
#define FENSTER_COMPONENT_TYPE_PANEL 6
#define FENSTER_COMPONENT_TYPE_SCROLLPANE 7
#define FENSTER_COMPONENT_TYPE_IMAGE 8
#define FENSTER_COMPONENT_TYPE_CHECKBOX 9

	/**
	 * Types of events that can be listened to
	 */
	typedef uint32_t ComponentEventType;
#define FENSTER_COMPONENT_EVENT_TYPE_ACTION 0
#define FENSTER_COMPONENT_EVENT_TYPE_BOUNDS 1
#define FENSTER_COMPONENT_EVENT_TYPE_CANVAS_NEW_BUFFER 2
#define FENSTER_COMPONENT_EVENT_TYPE_KEY 3
#define FENSTER_COMPONENT_EVENT_TYPE_FOCUS 4
#define FENSTER_COMPONENT_EVENT_TYPE_MOUSE 5
#define FENSTER_COMPONENT_EVENT_TYPE_CLOSE 6
#define FENSTER_COMPONENT_EVENT_TYPE_WINDOWS 7
#define FENSTER_COMPONENT_EVENT_TYPE_TITLE 8
#define FENSTER_COMPONENT_EVENT_TYPE_VISIBLE 9
#define FENSTER_COMPONENT_EVENT_TYPE_CHECKED 10

	/**
	 *
	 */
	typedef uint8_t LayoutManagerType;
#define FENSTER_LAYOUT_MANAGER_GRID (0)
#define FENSTER_LAYOUT_MANAGER_FLOW (1)
#define FENSTER_LAYOUT_MANAGER_FLEX (2)
#define FENSTER_LAYOUT_MANAGER_STACK (3)

	/**
	 *
	 */
	typedef struct
	{
		ProtocolCommandId id;
	} __attribute__((packed)) MessageHeader;

	/**
	 * Request to initialize interface communications. The window server creates a
	 * delegate thread that is responsible for further communications and responds
	 * with a <CommandApplicationInitializeResponse>.
	 */
	typedef struct
	{
		MessageHeader header;
		SYS_TID_T event_dispatcher;
	} __attribute__((packed)) CommandApplicationInitializeRequest;

	/**
	 * Response for initializing interface communications.
	 *
	 * @field status
	 * 		whether the initialization was successful
	 * @field window_server_delegate
	 * 		id of the task that is responsible for further window server communication
	 */
	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
		SYS_TID_T window_server_delegate;
	} __attribute__((packed)) CommandApplicationInitializeResponse;

	/**
	 * Generic response
	 */
	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
	} __attribute__((packed)) CommandSimpleResponse;

	/**
	 * Request sent to create a component.
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentType type;
	} __attribute__((packed)) CommandCreateComponentRequest;

	/**
	 * Response when creating a component.
	 */
	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
		ComponentId id;
	} __attribute__((packed)) CommandCreateComponentResponse;

	/**
	 * Request sent to destroy a component
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
	} __attribute__((packed)) CommandDestroyComponentRequest;

	/**
	 * Request/response to focus
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
	} __attribute__((packed)) CommandFocusRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
	} __attribute__((packed)) CommandFocusResponse;

	/**
	 * Request/response for adding a child
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId parent;
		ComponentId child;
	} __attribute__((packed)) CommandAddChildRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
	} __attribute__((packed)) CommandAddChildResponse;

	/**
	 * Setting bounds
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		Rectangle bounds;
	} __attribute__((packed)) CommandSetBoundsRequest;

	/**
	 * Setting size (preferred/minimum/maximum)
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		Dimension size;
	} __attribute__((packed)) CommandSetSizeRequest;

	/**
	 * Request/response for getting bounds
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
	} __attribute__((packed)) CommandGetBoundsRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
		Rectangle bounds;
	} __attribute__((packed)) CommandGetBoundsResponse;

	/**
	 * Request/response for getting a numeric property
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		int property;
	} __attribute__((packed)) CommandGetNumericPropertyRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
		uint32_t value;
	} __attribute__((packed)) CommandGetNumericPropertyResponse;

	/**
	 * Request/response for setting a numeric property
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		int property;
		uint32_t value;
	} __attribute__((packed)) CommandSetNumericPropertyRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
	} __attribute__((packed)) CommandSetStringPropertyResponse;

	/**
	 * Request for setting a string property
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		int property;
		char value[];
	} __attribute__((packed)) CommandSetStringPropertyRequest;

	/**
	 * Request/response for getting a string property
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		int property;
	} __attribute__((packed)) CommandGetStringPropertyRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
		char value[];
	} __attribute__((packed)) CommandGetStringPropertyResponse;

	/**
	 * Request to blit a canvas
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		Rectangle area;
	} __attribute__((packed)) CommandCanvasBlitRequest;

	/**
	 * Request to register the desktop canvas
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId canvas_id;
		SYS_TID_T target_thread; // For global events (like window event)
	} __attribute__((packed)) CommandRegisterDesktopCanvasRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
	} __attribute__((packed)) CommandRegisterDesktopCanvasResponse;

	/**
	 * Event handler registration functions
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		ComponentEventType event_type;
		SYS_TID_T target_thread;
	} __attribute__((packed)) CommandAddListenerRequest;

	typedef struct
	{
		MessageHeader header;
		ProtocolStatus status;
	} __attribute__((packed)) CommandAddListenerResponse;

	/**
	 * Retrieve screen size
	 */
	typedef struct
	{
		MessageHeader header;
	} __attribute__((packed)) CommandGetScreenDimensionRequest;

	typedef struct
	{
		MessageHeader header;
		Dimension size;
	} __attribute__((packed)) CommandGetScreenDimensionResponse;

	/**
	 * ScrollPane content
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId scrollpane;
		ComponentId content;
	} __attribute__((packed)) CommandScrollPaneSetContent;

	/**
	 * ScrollPane fixed sizes
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId scrollpane;
		bool width;
		bool height;
	} __attribute__((packed)) CommandScrollPaneSetFixed;

	/**
	 * Event structures
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentEventType type;
		ComponentId component_id;
	} __attribute__((packed)) ComponentEventHeader;

	typedef struct
	{
		ComponentEventHeader header;
	} __attribute__((packed)) ComponentActionEvent;

	typedef struct
	{
		ComponentEventHeader header;
		Rectangle bounds;
	} __attribute__((packed)) ComponentBoundsEvent;

	typedef struct
	{
		ComponentEventHeader header;
		bool checked;
	} __attribute__((packed)) ComponentCheckedEvent;

	typedef struct
	{
		ComponentEventHeader header;
		size_t newBufferAddress;
		uint16_t width;
		uint16_t height;
	} __attribute__((packed)) ComponentCanvasWaitForAcknowledgeEvent;

	typedef struct
	{
		ComponentEventHeader header;
		KeyInfoBasic key_info;
	} __attribute__((packed)) ComponentKeyEvent;

	typedef struct
	{
		ComponentEventHeader header;
		uint8_t now_focused;
	} __attribute__((packed)) ComponentFocusEvent;

	typedef struct
	{
		ComponentEventHeader header;
	} __attribute__((packed)) ComponentCloseEvent;

	typedef struct
	{
		ComponentEventHeader header;
		ProtocolStatus status;
		ComponentId window_id;
		bool present;
	} __attribute__((packed)) WindowsEvent;

	typedef struct
	{
		ComponentEventHeader header;
		char title[G_UI_COMPONENT_TITLE_MAXIMUM];
	} __attribute__((packed)) ComponentTitleEvent;

	typedef struct
	{
		ComponentEventHeader header;
		bool visible;
	} __attribute__((packed)) ComponentVisibleEvent;

	/**
	 * Mouse events
	 */
	typedef uint8_t MouseButton;
#define FENSTER_MOUSE_BUTTON_NONE ( 0x0)
#define FENSTER_MOUSE_BUTTON_1 ( 0x1)
#define FENSTER_MOUSE_BUTTON_2 ( 0x2)
#define FENSTER_MOUSE_BUTTON_3 ( 0x4)

	typedef uint8_t MouseEventType;
#define FENSTER_MOUSE_EVENT_NONE ( 0)
#define FENSTER_MOUSE_EVENT_MOVE ( 1)
#define FENSTER_MOUSE_EVENT_PRESS ( 2)
#define FENSTER_MOUSE_EVENT_RELEASE ( 3)
#define FENSTER_MOUSE_EVENT_DRAG_RELEASE ( 4)
#define FENSTER_MOUSE_EVENT_DRAG ( 5)
#define FENSTER_MOUSE_EVENT_ENTER ( 6)
#define FENSTER_MOUSE_EVENT_LEAVE ( 7)
#define FENSTER_MOUSE_EVENT_SCROLL ( 8)

	typedef struct
	{
		ComponentEventHeader header;
		Point position;
		MouseEventType type;
		MouseButton buttons;
		int clickCount;
		int scroll;
	} __attribute__((packed)) ComponentMouseEvent;
}

#endif
