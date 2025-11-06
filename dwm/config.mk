# Compiler and linker flags approximating upstream dwm defaults so we retain
# equivalent runtime dependencies when linking the multicall binary.

X11INC = /usr/include
X11LIB = /usr/lib

CFLAGS = -std=c99 -pedantic -Wall -O2
INCS   = -I$(X11INC) -I/usr/include/freetype2
LIBS   = -L$(X11LIB) -lX11 -lXft -lXrender -lfontconfig -lfreetype -lm
