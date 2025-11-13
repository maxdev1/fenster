ifeq ($(target),ghost)
	SYSROOT := /ghost/source/sysroot
	CXX := /ghost/bin/x86_64-ghost-g++
	CXXFLAGS := -I$(SYSROOT)/system/include/freetype2
	AR := /ghost/bin/x86_64-ghost-ar

else ifeq ($(target),windows)
	SYSROOT := ../platform/windows-mingw-sim/sysroot
	CXX := x86_64-w64-mingw32-g++
	CXXFLAGS := -I/usr/x86_64-w64-mingw32/include/freetype2 -I/usr/x86_64-w64-mingw32/include
	AR := x86_64-w64-mingw32-ar

else ifeq ($(target),macos)
    SYSROOT := ../platform/macos-sdl-sim/sysroot
    CXX := g++-15
	CXXFLAGS := -I/opt/homebrew/include/freetype2 -I/opt/homebrew/include
    AR := gcc-ar-15

else ifeq ($(target),)
$(error Target must be specified (e.g. "make target=ghost"))

endif
