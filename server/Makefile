include ../platform.mk

ifeq ($(target),ghost)
	LDFLAGS := -linput -lvideo -lps2driver -lproperties -lfenster -lcairo -lfreetype -lpixman-1 -lpng -lz
	BIN := ../fenster.bin

else ifeq ($(target),windows)
	LDFLAGS := -lproperties -lfenster -lcairo -lfreetype -lpixman-1 -lpng -lz -L$(SYSROOT)/system/lib -lgdi32
	CXXFLAGS := $(CXXFLAGS) -I$(SYSROOT)/system/include
	BIN := ../fenster.exe

else ifeq ($(target),macos)
	LDFLAGS := -lproperties -lfenster -lcairo -lfreetype -lpixman-1 -lpng -lz -lsdl2 -L$(SYSROOT)/system/lib -L/opt/homebrew/lib
	CXXFLAGS := $(CXXFLAGS) -I$(SYSROOT)/system/include
	BIN := ../fenster

endif

CXXFLAGS := -Isrc -Wall -Wextra -std=c++17 $(CXXFLAGS) -Wno-unused-parameter

SRC := $(shell find src -name '*.cpp')
OBJDIR := obj/$(target)
OBJ := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC))
LIBFENSTER := $(SYSROOT)/system/lib/libfenster.a

all: $(BIN)

$(BIN): $(OBJ) $(LIBFENSTER)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: $(BIN)
	mkdir -p $(SYSROOT)/applications
	cp $(BIN) $(SYSROOT)/applications/

clean:
	rm -rf $(OBJDIR) $(BIN)

.PHONY: all install clean
