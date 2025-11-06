# guii: Combined iiwm (dwm) + iist (st)

This repo builds a single multicall binary `guii` that contains both the dwm window manager (as `iiwm`) and the st terminal (as `iist`). Installing creates symlinks so running `iiwm` or `iist` invokes the corresponding built-in tool.

## Build

- Debian/Ubuntu (shared link):
  - `sudo apt-get update`
  - `sudo apt-get install -y build-essential pkg-config \
    libx11-dev libxft-dev libxrender-dev libxinerama-dev \
    libharfbuzz-dev libfontconfig1-dev libfreetype6-dev`
  - `make STATIC=0`

- macOS: install XQuartz or MacPorts/X11 and build shared:
  - `make STATIC=0`

- Fully static linking is usually not possible unless static X11 libs are available. Use `STATIC=0`.

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

- The iiwm build includes the patches and config from `dwm/` in this repo; iist uses `st/` with its config.
- Man pages are generated from upstream `dwm.1` and `st.1` with program names adjusted.

