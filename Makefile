# Top-level build to produce a single static multicall binary (guii)
#+ containing both dwm and st, and a deb package that installs it.

PREFIX ?= /usr
MANPREFIX ?= $(PREFIX)/share/man
DESTDIR ?=

MKFILEDIR := $(dir $(mkfile_path))


CC ?= cc
AR ?= ar
RANLIB ?= ranlib

# Package identity
PKGVER ?= 1.0.0
PKGNAME ?= guii
ARCH ?= amd64

# Control static linking: set STATIC=1 to link with -static
STATIC ?= 1

# Include subproject config for flags
include $(MKFILEDIR)/dwm/config.mk
include $(MKFILEDIR)/st/config.mk

# Rename conflicting symbols and mains per project
DWM_DEFS = -Dmain=dwm_main -Ddie=dwm_die
ST_DEFS  = -Dmain=st_main  -Ddie=st_die

# Sources per project
DWM_SRC = \
  dwm/dwm.c \
  dwm/drw.c \
  dwm/util.c \
  dwm/transient.c

ST_SRC = \
  st/st.c \
  st/x.c \
  st/hb.c

WRAP_SRC = combined/main.c

# Objects
DWM_OBJ := $(DWM_SRC:.c=.o)
ST_OBJ  := $(ST_SRC:.c=.o)
WRAP_OBJ := $(WRAP_SRC:.c=.o)

# Flags: merge both projects' include/link flags
# DWM uses INCS/LIBS from dwm/config.mk. ST uses STCFLAGS/STLDFLAGS from st/config.mk
COMMON_CFLAGS ?= -std=c99 -Wall -Wno-deprecated-declarations -Os
COMMON_INCS   = $(INCS) -Ist -Idwm
COMMON_LIBS   = $(LIBS)

# ST exposes STCFLAGS/STLDFLAGS which already contain INCS/LIBS

# Platform tweaks
UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
ifeq ($(UNAME_S),Darwin)
    # macOS: no librt or /usr/X11R6; prefer XQuartz/MacPorts
    FILTER_LIBS := -lrt -lutil -L/usr/X11R6/lib
    FILTER_INCS := -I/usr/X11R6/include
    DARWIN_INCS := -I/opt/X11/include -I/opt/local/include
    DARWIN_LIBS := -L/opt/X11/lib -L/opt/local/lib
else
    FILTER_LIBS :=
    FILTER_INCS :=
    DARWIN_INCS :=
    DARWIN_LIBS :=
endif

# Cleaned flags
LIBS_CLEANED := $(filter-out $(FILTER_LIBS),$(LIBS)) $(DARWIN_LIBS)
INCS_CLEANED := $(filter-out $(FILTER_INCS),$(INCS)) $(DARWIN_INCS)
STLDFLAGS_CLEANED := $(filter-out $(FILTER_LIBS),$(STLDFLAGS)) $(DARWIN_LIBS)

LDFLAGS_EXTRA =
ifeq ($(STATIC),1)
LDFLAGS_EXTRA += -static
endif

# Link libs: combine both
COMBINED_LDLIBS = $(STLDFLAGS_CLEANED) $(LDFLAGS) $(LDFLAGS_EXTRA)

.PHONY: all clean install uninstall print-flags deb

all: $(PKGNAME)

print-flags:
	@echo "CC = $(CC)"
	@echo "COMMON_CFLAGS = $(COMMON_CFLAGS)"
	@echo "COMMON_INCS = $(COMMON_INCS)"
	@echo "DWM CFLAGS = $(CFLAGS) $(DWM_DEFS)"
	@echo "ST CFLAGS = $(STCFLAGS) $(ST_DEFS)"
	@echo "COMBINED_LDLIBS = $(COMBINED_LDLIBS)"

$(PKGNAME): $(DWM_OBJ) $(ST_OBJ) $(WRAP_OBJ)
	$(CC) -o $@ $^ $(COMBINED_LDLIBS)

# Compile rules
dwm/config.h: dwm/config.def.h
	cp -f $< $@

dwm/%.o: dwm/%.c dwm/config.h
	$(CC) $(COMMON_CFLAGS) $(INCS_CLEANED) $(CFLAGS) $(DWM_DEFS) -c -o $@ $<

st/%.o: st/%.c
	$(CC) $(COMMON_CFLAGS) $(STCFLAGS) $(ST_DEFS) -c -o $@ $<

combined/%.o: combined/%.c
	$(CC) $(COMMON_CFLAGS) $(COMMON_INCS) -c -o $@ $<

clean:
	rm -f $(DWM_OBJ) $(ST_OBJ) $(WRAP_OBJ) $(PKGNAME)
	rm -rf build dist

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PKGNAME) $(DESTDIR)$(PREFIX)/bin/$(PKGNAME)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(PKGNAME)
	ln -sf $(PKGNAME) $(DESTDIR)$(PREFIX)/bin/iiwm
	ln -sf $(PKGNAME) $(DESTDIR)$(PREFIX)/bin/iist
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed -e "s/VERSION/$(PKGVER)/g" -e 's/\\bdwm\\b/iiwm/g' < dwm/dwm.1 > $(DESTDIR)$(MANPREFIX)/man1/iiwm.1
	sed -e 's/\\bst\\b/iist/g' < st/st.1 > $(DESTDIR)$(MANPREFIX)/man1/iist.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/iiwm.1 $(DESTDIR)$(MANPREFIX)/man1/iist.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PKGNAME)
	rm -f $(DESTDIR)$(PREFIX)/bin/iiwm
	rm -f $(DESTDIR)$(PREFIX)/bin/iist
	rm -f $(DESTDIR)$(MANPREFIX)/man1/iiwm.1 $(DESTDIR)$(MANPREFIX)/man1/iist.1

# Simple .deb packaging without debhelper, via dpkg-deb
deb: all
	rm -rf build dist && mkdir -p build/$(PKGNAME)/DEBIAN build/$(PKGNAME)/usr/bin build/$(PKGNAME)$(MANPREFIX)/man1
	cp -f $(PKGNAME) build/$(PKGNAME)/usr/bin/$(PKGNAME)
	chmod 755 build/$(PKGNAME)/usr/bin/$(PKGNAME)
	ln -s $(PKGNAME) build/$(PKGNAME)/usr/bin/iiwm
	ln -s $(PKGNAME) build/$(PKGNAME)/usr/bin/iist
	# Man pages inside package
	sed -e "s/VERSION/$(PKGVER)/g" -e 's/\\bdwm\\b/iiwm/g' < dwm/dwm.1 > build/$(PKGNAME)$(MANPREFIX)/man1/iiwm.1
	sed -e 's/\\bst\\b/iist/g' < st/st.1 > build/$(PKGNAME)$(MANPREFIX)/man1/iist.1
	chmod 644 build/$(PKGNAME)$(MANPREFIX)/man1/iiwm.1 build/$(PKGNAME)$(MANPREFIX)/man1/iist.1
	# Control file
	mkdir -p build/$(PKGNAME)/DEBIAN
	printf "Package: $(PKGNAME)\nVersion: $(PKGVER)\nSection: x11\nPriority: optional\nArchitecture: $(ARCH)\nMaintainer: Unknown <unknown@example.com>\nDescription: Combined iiwm+iist multicall binary (static)\n" > build/$(PKGNAME)/DEBIAN/control
	# Build .deb
	mkdir -p dist
	cd build && dpkg-deb --build $(PKGNAME) ../dist/$(PKGNAME)_$(PKGVER)_$(ARCH).deb
	@echo "Built dist/$(PKGNAME)_$(PKGVER)_$(ARCH).deb"
