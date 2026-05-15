# gabix

Bare metal x86_64 kernel written in C as a fun little project

## Overview

This is a hobby kernel intended to help facilitate my own learning. Features will be added when I think they would be fun to tinker with or if they are needed for another feature. Currently it just boots, modifies a frame buffer to print a simple message, and then halts.

The build process happens entirely within a docker container to allow any host with Docker and QEMU installed to build and test the kernel. A container is also configured to run so that intellisense functions correctly regardless of the host.

## Requirements

- Docker
- QEMU (`qemu-system-x86_64`)

## Building

```sh
./gabix-build
```

To build and immediately launch in QEMU:

```sh
./gabix-build --run
```

## Project Structure

```
gabix-build                     # Top-level build/run script
build/
  Dockerfile                    # Cross-compiler image (x86_64-elf gcc, mtools, limine)
  compose.yml                   # Docker Compose service definition
  scripts/
    build-image.sh              # Runs inside the container; compiles kernel and assembles disk image
    build-binutils.sh
    build-gcc.sh
  toolchain-x86_64-elf.cmake    # CMake cross-compilation toolchain file
  limine.conf                   # Limine bootloader configuration
kernel/
  src/
    core/                       # Kernel entry point
    drivers/                    # Hardware drivers (display/framebuffer)
    lib/                        # Freestanding library routines (memcpy, memset, …)
  include/
    vendor/                     # Third-party headers with no corresponding source (limine, fonts)
  config/
    linker.lds                  # Linker script
```

## License

MIT License — see [LICENSE](LICENSE) for full text.
