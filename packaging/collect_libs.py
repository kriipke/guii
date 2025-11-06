#!/usr/bin/env python3
"""Emit absolute shared-library paths required by a dynamic executable."""
from __future__ import annotations

import os
import subprocess
import sys

def parse_ldd_output(binary: str) -> list[str]:
    try:
        output = subprocess.check_output(["ldd", binary], text=True)
    except subprocess.CalledProcessError as exc:  # pragma: no cover
        raise SystemExit(exc.returncode)

    libs: set[str] = set()
    for raw_line in output.splitlines():
        line = raw_line.strip()
        if not line or "statically linked" in line:
            continue
        path = ""
        if "=>" in line:
            _, rhs = line.split("=>", 1)
            path = rhs.strip().split(" ", 1)[0]
        else:
            path = line.split(" ", 1)[0]
        path = path.strip()
        if not path or path == "not":
            continue
        if path.startswith("("):
            continue
        real = os.path.realpath(path)
        if os.path.isfile(real):
            libs.add(real)
    return sorted(libs)


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: collect_libs.py <binary>", file=sys.stderr)
        return 2
    binary = sys.argv[1]
    if not os.path.exists(binary):
        print(f"error: binary '{binary}' does not exist", file=sys.stderr)
        return 2
    for path in parse_ldd_output(binary):
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
