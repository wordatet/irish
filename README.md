# irish (IRIX Shell)

A strict, 64-bit clean port of the IRIX 6.5.17 Bourne Shell (`bsh`) to modern Linux.

## Overview

`irish` is a preservation project dedicated to porting the historical Silicon Graphics, Inc. (SGI) Bourne Shell to modern environments. This repository contains the original IRIX source code, modified for POSIX compliance and 64-bit stability, while maintaining the historical integrity and quirks of the original shell.

### Key Modifications
- **64-bit Safety:** Refactored memory alignment macros and pointer arithmetic to prevent truncation on modern x86_64 systems.
- **Silent Build:** Fixed over 100 compilation warnings related to implicit declarations and modern C standards.
- **POSIX Shims:** Implemented necessary compatibility layers for system calls that have evolved since IRIX 6.5.17.
- **Stability:** Includes a "Paranoid Mode" stress test suite to verify pipes, redirections, and signal handling.

## Build Requirements

- A modern C compiler (`gcc` or `clang`)
- `make`
- Standard Linux headers

## Installation

```sh
# Clone the repository
git clone git@github.com:wordatet/irish.git
cd irish

# Build the shell
make

# Run the shell
./sh
```

## Running Tests

To verify the stability of the port on your specific Linux distribution:

```sh
./sh stress.sh
```

## Repository Structure

- `irix-bsh-port.patch`: A unified diff containing all modifications made to the original IRIX source code.
- `stress.sh`: Stability test suite.
- `LICENSE`: Hybrid license covering the historical source and MIT-licensed porting patches.

## Legal & Licensing

The core source code remains the proprietary property of its respective copyright holders (UNIX System Laboratories / SGI). This project is intended for historical preservation, educational research, and interoperability study.

All porting modifications, patches, and build scripts are released under the 3-Clause BSD License. See the `LICENSE` file for full details.

---
*Maintained by Mario (@wordatet)*
