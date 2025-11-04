# ƒenster

A small, lightweight window server and UI toolkit designed for easy embedding and minimal dependencies.

This work-in-progress project aims to provide a clean, self-contained system for powering graphical environments — from hobby operating systems to embedded or experimental platforms.

It originated as part of the [Ghost OS](https://github.com/maxdev1/ghost) project, a hobby operating system.

## Architecture
The window server is designed to run as a standalone user-space application that other applications communicate with via messaging.

Applications build their graphical user interfaces through `libfenster`, which provides client-side abstractions for supported components and manages communication with the window server.

The server uses an event-driven architecture that allows clients to subscribe to events from any component.

The utility library `libproperties` is used for configuration management.

Aside from this way, it is also possible to link the entire application into your embedded software and directly interface
with the component classes.

## Client Library

`libfenster` offers a set of component classes and layouts that can be combined to create a UI for your application.
Starting interaction with the server works by initializing the application. This operation returns a status code:

    if(fenster::Application::open() != FENSTER_APPLICATION_STATUS_SUCCESSFUL)
        return -1;

For creating components, there is usually always a static factory method that will create an instance:

    auto window = fenster::Window::create();

This client-side handle represents a component on the server and is used to modify the component, retrieve information
about it or attach listeners.

    window->setTitle("Window title");
    window->setBounds(fenster::Rectangle(30, 30, 400, 200));
    window->setVisible(true);

## Event Handling

There is a system for event processing that allows handling different kinds of events like keyboard or mouse input and
also actions (clicking on a button). On client-side, it is easily possible to subscribe events directly on the component:

    auto button = fenster::Button::create();
    button->setTitle("Click me!");
    button->setActionListener([] () {
        printf("Button was clicked\n");
    });


## Embedding & Platform Specifics

The project intends to keep dependencies for the target platform minimal. The major dependencies of the project are:

* **C++ compiler**
* **zlib**, **pixman** and **libpng** as dependencies to below
* **freetype** for font rendering
* **cairo** for accelerated 2D drawing

With these libraries available on the target platform, porting the project requires implementing a set of platform-specific type definitions and functions.
The headers for these definitions are found in `libfenster/inc/platform/platform.hpp` and `server/src/platform/platform.hpp`.
Each header includes documentation explaining the required functions and their expected behavior.

Currently, two platform implementations exist:
* **ghost**, the actual implementation for Ghost OS;
* **windows-mingw-sim** a WinAPI-based simulation layer for debugging and testing on Windows

These implementations serve as examples for creating new platform integrations.
