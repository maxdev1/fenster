include ../platform.mk

LIB := libfenster.a
CXXFLAGS := -Iinc -I$(FREETYPE_INC) -I$(CAIRO_INC) -std=c++17 # -Wall -Wextra

SRC := $(shell find src -name '*.cpp')
OBJ := $(patsubst src/%.cpp,obj/%.o,$(SRC))

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: install-headers $(LIB)
	mkdir -p $(SYSROOT)/system/lib
	cp $(LIB) $(SYSROOT)/system/lib/

install-headers:
	mkdir -p $(SYSROOT)/system/include
	cp -r inc/* $(SYSROOT)/system/include/

clean:
	rm -rf obj $(LIB)

.PHONY: all install clean install-headers
