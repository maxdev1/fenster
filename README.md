# ƒenster

A small, lightweight window server and UI toolkit designed for easy embedding and minimal dependencies.

It features standard components, a sophisticated event handling mechanism, different container
layouts, canvas for client-side drawing and more.

This work-in-progress project aims to provide a clean, self-contained system for GUIs - from hobby operating systems to
embedded or experimental platforms.

It originated as part of the [Ghost OS](https://github.com/maxdev1/ghost) project, a hobby operating system.

## Building

To build the project, run make specifying the target platform:

    make target=ghost clean all

The makefiles require a target to be specified since platform-specific code must be compiled.

## Architecture

The window server is designed to run as a standalone user-space application that other applications communicate with via
messaging.

Applications build their graphical user interfaces through `libfenster`, which provides client-side abstractions for
supported components and manages communication with the window server.

The server uses an event-driven architecture that allows clients to subscribe to events from any component.

The utility library `libproperties` is used for configuration management.

Aside from this way, it is also possible to link the entire application into your embedded software and directly
interface
with the component classes.

## Embedding & platform-specifics

The project intends to keep dependencies for the target platform minimal. The major dependencies of the project are:

* **C++ compiler**
* **zlib**, **pixman** and **libpng** as dependencies to below
* **freetype** for font rendering
* **cairo** for accelerated 2D drawing

With these libraries available on the target platform, porting the project requires implementing a set of
platform-specific type definitions and functions.
The headers for these definitions are found in `libfenster/inc/platform/platform.hpp` and
`server/src/platform/platform.hpp`.
Each header includes documentation explaining the required functions and their expected behavior.

Currently, two platform implementations exist:

* **ghost**, the actual implementation for Ghost OS;
* **windows-mingw-sim** a WinAPI-based simulation layer for debugging and testing on Windows

These implementations serve as examples for creating new platform integrations.

## Client library

`libfenster` offers a set of component classes and layouts that can be combined to create a UI for your application.
Starting interaction with the server works by initializing the application. This operation returns a status code:

    if(fenster::Application::open() != fenster::ApplicationOpenStatus::Success)
        return -1;

For creating components, there is usually always a static factory method that will create an instance:

    auto window = fenster::Window::create();

This client-side handle represents a component on the server and is used to modify the component, retrieve information
about it or attach listeners.

    window->setTitle("Window title");
    window->setBounds(fenster::Rectangle(30, 30, 400, 200));
    window->setVisible(true);

## Components & Updates

Components are organized in a tree structure, where any component can have child components.
The root component is the `Screen`, which is a special component that also tracks dirty (invalidated) areas and emits
window events.

The window server repeatedly executes the following cycle:

### 1. Processing input events

Input events such as mouse and keyboard interactions are captured and converted into specific
event types like `MouseEvent` or `KeyEvent`.
These events are then dispatched to the affected components.

### 2. Resolving component requirements

Each component can go through up to three phases, each serving a specific purpose:

* **Update** – Updates the component's model and determines its preferred size (bottom-up).
* **Layout** – Positions and sizes child components (top-down).
* **Paint** – Renders visual content (bottom-up).

A component maintains a _requirement state_ that indicates which phases need to be executed.
It also tracks the requirement state of its children to optimize updates.

Typical scenarios:

* When a component's own model changes, the update requirement should be set.
  → The framework will call `update()`.

* When child sizes or layout positions are affected, the layout requirement should be set.
  → The framework will call `layout()` and cascade layouting to children.

* When only visual changes occur (e.g., a checkbox being checked/unchecked), the paint requirement should be set.
  → Only the affected component will be repainted, and parent components will be informed that a repaint is needed.

### 3. Blitting to output buffer

Each component has an internal buffer where it draws its contents during `paint()`.
Once all updates and paints are resolved, visible components are composited (blitted) from top to bottom into a screen
buffer.
Finally, only the dirty regions of the screen are copied to the actual output (for example, a framebuffer).

## Event handling

There is a system for event processing that handles different types of input — such as keyboard and mouse — as well as
higher-level actions like clicking a button.

On the client side, it’s easy to subscribe to events directly on components:

    auto button = fenster::Button::create();
    button->setTitle("Click me!");
    button->setActionListener([] () {
        printf("Button was clicked\n");
    });

### Flow overview

Events are processed centrally by the `EventProcessor`, which collects raw input
data (from the platform layer), converts it into structured events (like `MouseEvent`
or `KeyEvent`), and dispatches them to the appropriate components through the Server.

Events are dispatched to components starting from the root and traversing down the
tree until a component handles the event. If an event is not handled, it can be bubbled
upwards (to parent components).

Some events, like scroll, drag or mouse-enter/leave events are dispatched upwards, starting
at the component lowest in the tree.
