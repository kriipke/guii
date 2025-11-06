#define _POSIX_C_SOURCE 200809L
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

/*
 * Force the linker to keep the same shared library dependencies that dwm/st
 * normally require by taking the address of representative symbols.  Nothing
 * here executes at runtime; the variables are only marked used to defeat dead
 * code elimination.
 */

typedef void (*voidfn)(void);

typedef FT_Error (*ft_init_fn)(FT_Library *);
typedef FT_Error (*ft_done_fn)(FT_Library);

__attribute__((used)) static voidfn dep_xclose = (voidfn)XCloseDisplay;
__attribute__((used)) static voidfn dep_xsync = (voidfn)XSynchronize;
__attribute__((used)) static voidfn dep_xset_error = (voidfn)XSetErrorHandler;
__attribute__((used)) static voidfn dep_xftdrawcreate = (voidfn)XftDrawCreate;
__attribute__((used)) static voidfn dep_xftdrawdestroy = (voidfn)XftDrawDestroy;
__attribute__((used)) static voidfn dep_xftcoloralloc = (voidfn)XftColorAllocName;
__attribute__((used)) static voidfn dep_xftcolorfree = (voidfn)XftColorFree;
__attribute__((used)) static voidfn dep_xfttext = (voidfn)XftDrawStringUtf8;
__attribute__((used)) static voidfn dep_fcinit = (voidfn)FcInit;
__attribute__((used)) static voidfn dep_fcfini = (voidfn)FcFini;
__attribute__((used)) static ft_init_fn dep_ft_init = FT_Init_FreeType;
__attribute__((used)) static ft_done_fn dep_ft_done = FT_Done_FreeType;
