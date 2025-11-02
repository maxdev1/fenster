# ƒenster

A small, lightweight window server and UI toolkit designed for easy embedding and minimal dependencies.

This work-in-progress project aims to provide a clean, self-contained system for powering graphical environments — from hobby operating systems to embedded or experimental platforms.

It originated as part of the [Ghost OS](https://github.com/maxdev1/ghost) project, a hobby operating system.

## Architecture
The window server is designed to run as a standalone user-space application that other applications communicate with via messaging.

Applications build their graphical user interfaces through `libfenster`, which provides client-side abstractions for supported components and manages communication with the window server.

The server uses an event-driven architecture that allows clients to subscribe to events from any component.

The utility libraries `libjson` and `libproperties` are used for data serialization and configuration management.

Aside from this way, it is also possible to link the entire application into your embedded software and directly interface
with the component classes.

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
