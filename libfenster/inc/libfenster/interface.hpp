// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBFENSTER_INTERFACE
#define LIBFENSTER_INTERFACE

#include "properties.hpp"
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
	enum class ProtocolCommandId: uint8_t
	{
		Initialization = 0,
		CreateComponent = 1,
		AddComponent = 2,
		SetBounds = 3,
		Focus = 4,
		AddListener = 5,
		SetNumericProperty = 6,
		GetNumericProperty = 7,
		SetStringProperty = 8,
		GetStringProperty = 9,
		GetBounds = 10,
		CanvasBlit = 11,
		RegisterDesktopCanvas = 12,
		GetScreenDimension = 13,
		ScrollPaneSetContent = 18,
		ScrollPaneSetFixed = 19,
		SetPreferredSize = 20,
		DestroyComponent = 21,
		SetMinimumSize = 22,
		SetMaximumSize = 23,
	};

	/**
	 * Common status for requests
	 */
	enum class ProtocolStatus: uint8_t
	{
		Success = 0,
		Error = 1
	};

	/**
	 * Component types
	 */
	enum class ComponentType: uint16_t
	{
		Window = 0,
		Button = 1,
		Label = 2,
		TextField = 3,
		Canvas = 4,
		Selection = 5,
		Panel = 6,
		ScrollPane = 7,
		Image = 8,
		Checkbox = 9,
		RadioButton = 10,
		RadioGroup = 11,
		TextArea = 12,
	};

	/**
	 * Types of events that can be listened to
	 */
	enum class ComponentEventType: uint16_t
	{
		Action = 0,
		Bounds = 1,
		CanvasNewBuffer = 2,
		Key = 3,
		Focus = 4,
		Mouse = 5,
		Close = 6,
		Window = 7,
		Title = 8,
		Visible = 9,
		Checked = 10,
	};

	/**
	 * Different types of layouts
	 */
	enum class LayoutType: uint8_t
	{
		Grid = 0,
		Flow = 1,
		Flex = 2,
		Stack = 3,
	};

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
		ComponentProperty property;
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
		ComponentProperty property;
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
		ComponentProperty property;
		char value[];
	} __attribute__((packed)) CommandSetStringPropertyRequest;

	/**
	 * Request/response for getting a string property
	 */
	typedef struct
	{
		MessageHeader header;
		ComponentId id;
		ComponentProperty property;
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
	 * Mouse buttons
	 */
	enum class MouseButton: uint8_t
	{
		None = 0b0000,
		Button1 = 0b0001,
		Button2 = 0b0010,
		Button3 = 0b0100,
	};

	inline MouseButton& operator|=(MouseButton& lhs, MouseButton rhs)
	{
		lhs = (MouseButton) ((uint8_t) lhs | (uint8_t) rhs);
		return lhs;
	}

	inline MouseButton& operator&=(MouseButton& lhs, MouseButton rhs)
	{
		lhs = (MouseButton) ((uint8_t) lhs & (uint8_t) rhs);
		return lhs;
	}

	inline bool operator&(MouseButton& lhs, MouseButton rhs)
	{
		return ((uint8_t) lhs & (uint8_t) rhs);
	}

	inline MouseButton operator~(const MouseButton& lhs)
	{
		return (MouseButton) ~((uint8_t) lhs);
	}

	/**
	 * Types of mouse events
	 */
	enum class MouseEventType: uint8_t
	{
		None = 0,
		Move = 1,
		Press = 2,
		Release = 3,
		DragRelease = 4,
		Drag = 5,
		Enter = 6, // TODO: Components must react on Move events so they also receive Enter/Leave
		Leave = 7,
		Scroll = 8,
	};

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
