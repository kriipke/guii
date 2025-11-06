# guii: Combined iiwm (dwm) + iist (st)

This repo builds a single multicall binary `guii` that contains both an `iiwm` window-manager stub and an `iist` terminal stub. Installing creates symlinks so running `iiwm` or `iist` invokes the corresponding built-in tool.

## Build

The stub implementations only depend on a C toolchain. Building the
multicall binary therefore requires `make` and a C compiler:

```
make STATIC=0
```

The `STATIC` flag is still accepted for compatibility, but statically
linking the bundled programs rarely succeeds on systems without static
libc support.

## Install

- `sudo make install PREFIX=/usr`
- Installs:
  - `/usr/bin/guii`
  - `/usr/bin/iiwm -> guii`
  - `/usr/bin/iist -> guii`
  - Man pages: `iiwm.1` and `iist.1` in `/usr/share/man/man1/`

## Debian Package

- Build: `make deb STATIC=0 PKGVER=1.0.0 ARCH=amd64`
- Output: `dist/guii_1.0.0_amd64.deb`
- Install: `sudo dpkg -i dist/guii_1.0.0_amd64.deb`

## Usage

- Run window manager: `iiwm` (typically via your X session)
- Run terminal: `iist [arguments]`
- Multicall entrypoint: `guii` defaults to `iiwm`, or choose explicitly:
  - `guii --iiwm`
  - `guii --iist`

## Notes

- The repository now vendors minimal C implementations under `dwm/` and `st/`. They provide deterministic behaviour for packaging and testing but do not implement the full X11 feature set of upstream dwm/st.
- Developers who want to experiment with the genuine upstream projects can replace the contents of `dwm/` and `st/` with their preferred versions and rebuild. The top-level `Makefile` only expects a `config.mk`, `config.def.h`, the corresponding source files, and man pages in each directory.
- Man pages are generated from the vendored `dwm/dwm.1` and `st/st.1` files with program names adjusted during installation.

