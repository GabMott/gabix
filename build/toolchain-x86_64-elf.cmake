# build/toolchain-x86_64-elf.cmake
#
# Cross-compilation toolchain for x86_64-elf targets.
# Used inside the Docker build container where the x86_64-elf-* tools are
# installed under /usr/local/bin.
#
# Usage:
#   cmake -S kernel -B build/cmake-build -DCMAKE_TOOLCHAIN_FILE=build/toolchain-x86_64-elf.cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  x86_64)

# Prevent CMake from trying to link a hosted test binary when probing tools.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER    x86_64-elf-gcc)
set(CMAKE_CXX_COMPILER  x86_64-elf-g++)
set(CMAKE_ASM_COMPILER  x86_64-elf-gcc)
set(CMAKE_LINKER        x86_64-elf-ld)
set(CMAKE_AR            x86_64-elf-ar)
set(CMAKE_RANLIB        x86_64-elf-ranlib)
set(CMAKE_OBJCOPY       x86_64-elf-objcopy)
set(CMAKE_STRIP         x86_64-elf-strip)

# Do not search the host sysroot for libraries or headers.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
