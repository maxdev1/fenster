include ../platform.mk

LIB := libfenster.a
CXXFLAGS := $(CXXFLAGS) -Iinc -std=c++17

SRC := $(shell find src -name '*.cpp')
OBJTGT := obj/$(target)
OBJ := $(patsubst src/%.cpp,$(OBJTGT)/%.o,$(SRC))

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

$(OBJTGT)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: install-headers $(LIB)
	mkdir -p $(SYSROOT)/system/lib
	cp $(LIB) $(SYSROOT)/system/lib/

install-headers:
	mkdir -p $(SYSROOT)/system/include
	cp -r inc/* $(SYSROOT)/system/include/

clean:
	rm -rf $(OBJTGT) $(LIB) $(TEST_BIN)

.PHONY: all install clean install-headers
