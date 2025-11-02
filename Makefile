SUBPROJECTS := libjson libproperties libwindow libfont windowserver

.PHONY: all clean install-headers

all: build install

build:
	@$(MAKE) -C libjson
	@$(MAKE) -C libjson install
	@$(MAKE) -C libproperties
	@$(MAKE) -C libproperties install
	@$(MAKE) -C libwindow
	@$(MAKE) -C libwindow install
	@$(MAKE) -C windowserver

install:
	@$(MAKE) -C windowserver install

install-headers:
	@$(MAKE) -C libjson install-headers
	@$(MAKE) -C libproperties install-headers
	@$(MAKE) -C libwindow install-headers

clean:
	@for dir in $(SUBPROJECTS); do \
		$(MAKE) -C $$dir clean; \
	done
