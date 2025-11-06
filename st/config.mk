# Compiler/linker flags keeping parity with upstream st defaults.

X11INC = /usr/include
X11LIB = /usr/lib
PKG_CONFIG = pkg-config

STCPPFLAGS =
STCFLAGS   = -I$(X11INC) -I/usr/include/freetype2
STLDFLAGS  = -L$(X11LIB) -lX11 -lXft -lXrender -lfontconfig -lfreetype -lm
