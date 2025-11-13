SUBPROJECTS := libproperties libfenster server

.PHONY: all clean install-headers test

all: build install

build:
	@$(MAKE) -C libproperties
	@$(MAKE) -C libproperties install
	@$(MAKE) -C libfenster
	@$(MAKE) -C libfenster install
	@$(MAKE) -C server

install:
	@$(MAKE) -C server install

install-headers:
	@$(MAKE) -C libproperties install-headers
	@$(MAKE) -C libfenster install-headers

clean:
	@for dir in $(SUBPROJECTS); do \
		$(MAKE) -C $$dir clean; \
	done
