#!/usr/bin/env sh
# build-image.sh — compile the kernel and assemble a bootable disk image.
# Runs inside the x86_64-build Docker container with the project root mounted
# at /workspace.

set -e

ENVDIR=/workspace

# ── Kernel compile ────────────────────────────────────────────────────────────
cmake -S "$ENVDIR/kernel" -B "$ENVDIR/build/cmake-build" \
    -DCMAKE_TOOLCHAIN_FILE="$ENVDIR/build/toolchain-x86_64-elf.cmake"
cmake --build "$ENVDIR/build/cmake-build" --parallel "$(nproc)"

# ── Limine bootloader ────────────────────────────────────────────────────────
# Binaries and host tool are baked into the image at /opt/limine.
LIMINE_DIR=/opt/limine

# ── Disk image assembly ───────────────────────────────────────────────────────
# Produces a raw MBR disk image — no xorriso required.
mkdir -p "$ENVDIR/build/out"
IMAGE="$ENVDIR/build/out/image.img"

# 64 MiB blank image
dd if=/dev/zero of="$IMAGE" bs=1M count=64 2>/dev/null

# MBR partition table: one bootable FAT32 partition starting at sector 2048
echo '2048,,b,*' | sfdisk --label dos "$IMAGE" >/dev/null

# Format the FAT32 partition and populate it via mtools.
# The partition starts at byte offset 2048 × 512 = 1 048 576.
MTOOLSRC_FILE="$(mktemp)"
printf 'drive i: file="%s" offset=1048576\n' "$IMAGE" > "$MTOOLSRC_FILE"
MTOOLSRC="$MTOOLSRC_FILE" mformat -F -v GABIX -H 2048 i:
MTOOLSRC="$MTOOLSRC_FILE" mmd i:/boot i:/boot/limine i:/EFI i:/EFI/BOOT
MTOOLSRC="$MTOOLSRC_FILE" mcopy "$ENVDIR/kernel/bin/gabix"    "i:/boot/"
MTOOLSRC="$MTOOLSRC_FILE" mcopy "$ENVDIR/build/limine.conf"   "i:/boot/limine/"
MTOOLSRC="$MTOOLSRC_FILE" mcopy "$LIMINE_DIR/limine-bios.sys" "i:/boot/limine/"
MTOOLSRC="$MTOOLSRC_FILE" mcopy "$LIMINE_DIR/BOOTX64.EFI"     "i:/EFI/BOOT/"
MTOOLSRC="$MTOOLSRC_FILE" mcopy "$LIMINE_DIR/BOOTIA32.EFI"    "i:/EFI/BOOT/"
rm -f "$MTOOLSRC_FILE"

# Install Limine BIOS bootcode into the MBR and VBR.
# Capture output so the informational "Reminder:" banner is filtered out;
# the $() exit code still propagates so set -e fires on real failures.
_limine_out="$("$LIMINE_DIR/limine" bios-install "$IMAGE" 2>&1)"
printf '%s\n' "$_limine_out" | sed '/^Reminder:/,/boot will fail!/d'

# Fix ownership so the host user (who owns /workspace) can access all outputs.
HOST_OWNER="$(stat -c '%u:%g' /workspace)"
chown -R "$HOST_OWNER" "$ENVDIR/build/out" "$ENVDIR/build/cmake-build" "$ENVDIR/kernel/bin"
