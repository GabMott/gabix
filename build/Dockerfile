# Dockerfile — x86_64-elf cross-compiler build environment
# Each tool is its own named stage so Docker BuildKit's progress output shows
# descriptive labels (e.g. [gcc-build 2/2]) instead of generic [build N/N].
# Layer caching is preserved: stages are only re-run when their inputs change.

ARG BINUTILS_VERSION=2.46.0
ARG GCC_VERSION=16.1.0

# ── Stage: base-packages ──────────────────────────────────────────────────────
FROM debian:latest AS base-packages
RUN set -x \
    && apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y --no-install-recommends \
    ca-certificates wget gcc libgmp3-dev libmpfr-dev libisl-dev libmpc-dev \
    texinfo bison flex make bzip2 patch build-essential bear cmake nasm \
    mtools fdisk

# ── Stage: binutils-download ─────────────────────────────────────────────────
FROM base-packages AS binutils-download
ARG BINUTILS_VERSION
RUN set -x \
    && mkdir -p /usr/local/src \
    && wget -q -O /usr/local/src/binutils.tar.gz \
    https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz \
    && tar -xzf /usr/local/src/binutils.tar.gz -C /usr/local/src \
    && rm /usr/local/src/binutils.tar.gz \
    && chown -R root:root /usr/local/src/binutils-${BINUTILS_VERSION} \
    && chmod -R o-w,g+w /usr/local/src/binutils-${BINUTILS_VERSION}

# ── Stage: binutils-build ────────────────────────────────────────────────────
FROM binutils-download AS binutils-build
ARG BINUTILS_VERSION
COPY build/scripts/build-binutils.sh /usr/local/src/
RUN set -x \
    && cd /usr/local/src \
    && bash build-binutils.sh ${BINUTILS_VERSION}

# ── Stage: gcc-download ──────────────────────────────────────────────────────
FROM binutils-build AS gcc-download
ARG GCC_VERSION
RUN set -x \
    && wget -q -O /usr/local/src/gcc.tar.gz \
    https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz \
    && tar -xzf /usr/local/src/gcc.tar.gz -C /usr/local/src \
    && rm /usr/local/src/gcc.tar.gz \
    && chown -R root:root /usr/local/src/gcc-${GCC_VERSION} \
    && chmod -R o-w,g+w /usr/local/src/gcc-${GCC_VERSION}

# ── Stage: gcc-build ─────────────────────────────────────────────────────────
FROM gcc-download AS gcc-build
ARG GCC_VERSION
COPY build/scripts/build-gcc.sh /usr/local/src/
RUN set -x \
    && cd /usr/local/src \
    && bash build-gcc.sh ${GCC_VERSION}

# ── Stage: limine ────────────────────────────────────────────────────────────
FROM gcc-build AS limine
RUN set -x \
    && mkdir -p /opt/limine \
    && wget -qO- https://github.com/limine-bootloader/limine/releases/latest/download/limine-binary.tar.gz \
    | tar -xz --strip-components=1 -C /opt/limine \
    && make -C /opt/limine

VOLUME /workspace
WORKDIR /workspace

# ── Stage: devenv ─────────────────────────────────────────────────────────────
# Extends the build image with clangd and git for VS Code Dev Container.
# clangd provides IntelliSense; git enables VS Code's built-in source control
# and any Git extensions (GitLens, Git Graph, etc.).
# All cross-compiler tooling is inherited from the limine stage.
FROM limine AS devenv
RUN apt-get update \
    && apt-get install -y --no-install-recommends clangd git \
    && rm -rf /var/lib/apt/lists/*
