cmake_minimum_required(VERSION 3.25)

# Must precede project() so CMake doesn't try to link a hosted test executable
# when probing the compiler — a freestanding kernel can't satisfy that link.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(gabix
    VERSION 0.1.0
    LANGUAGES C ASM
)

# ── Optional NASM support ──────────────────────────────────────────────────────
# Enable only when nasm is present; .asm sources are compiled automatically
# once the language is active.
include(CheckLanguage)
check_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER)
    enable_language(ASM_NASM)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
    set(CMAKE_ASM_NASM_FLAGS "-g -F dwarf -Wall")
endif()

# ── Sources ────────────────────────────────────────────────────────────────────
file(GLOB_RECURSE C_SOURCES     CONFIGURE_DEPENDS "src/*.c")
file(GLOB_RECURSE ASM_S_SOURCES CONFIGURE_DEPENDS "src/*.S")
file(GLOB_RECURSE NASM_SOURCES  CONFIGURE_DEPENDS "src/*.asm")

# ── Kernel executable ──────────────────────────────────────────────────────────
add_executable(gabix
    ${C_SOURCES}
    ${ASM_S_SOURCES}
    $<$<BOOL:${CMAKE_ASM_NASM_COMPILER}>:${NASM_SOURCES}>
)

# Emit the binary to bin/ at the project root so build.sh can find it at the
# same path it always has (bin/gabix).
set_target_properties(gabix PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# ── Include directories ────────────────────────────────────────────────────────
target_include_directories(gabix PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
)

# ── Compile flags ──────────────────────────────────────────────────────────────
target_compile_options(gabix PRIVATE
    # C source files
    $<$<COMPILE_LANGUAGE:C>:
        -Wall
        -Wextra
        -g
        -O2
        -pipe
        -std=gnu11
        -ffreestanding
        -fno-stack-protector
        -fno-stack-check
        -fno-lto
        -fno-PIC
        -ffunction-sections
        -fdata-sections
        -m64
        -march=x86-64
        -mabi=sysv
        -mno-80387
        -mno-mmx
        -mno-sse
        -mno-sse2
        -mno-red-zone
        -mcmodel=kernel
    >
    # GAS assembly (.S) files — subset of C flags that apply to the assembler
    $<$<COMPILE_LANGUAGE:ASM>:
        -g
        -O2
        -pipe
        -ffreestanding
        -fno-stack-protector
        -fno-stack-check
        -fno-lto
        -fno-PIC
        -m64
        -march=x86-64
        -mno-red-zone
        -mcmodel=kernel
    >
)

# ── Linker flags ───────────────────────────────────────────────────────────────
# The compiler drives the link step; ld-specific flags are passed via -Wl,.
target_link_options(gabix PRIVATE
    -nostdlib
    -static
    -m64
    -Wl,-z,max-page-size=0x1000
    -Wl,--gc-sections
    -T "${CMAKE_SOURCE_DIR}/config/linker.lds"
)

# Relink automatically when the linker script changes.
set_target_properties(gabix PROPERTIES
    LINK_DEPENDS "${CMAKE_SOURCE_DIR}/config/linker.lds"
)
