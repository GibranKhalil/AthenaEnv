# syntax=docker/dockerfile:1
# Pinned to immutable SHA256 digest for guaranteed reproducible builds
ARG BASE_IMAGE=ghcr.io/ps2homebrew/ps2homebrew@sha256:207523ad98f17fd406afa7dd0c6cfe10730627545cbe45247f0bd73c8a66c376
FROM ${BASE_IMAGE}

# OpenContainers image metadata
LABEL org.opencontainers.image.title="AthenaEnv Build Container" \
      org.opencontainers.image.description="Minimal reproducible build environment for AthenaEnv PS2 Homebrew with QuickJS" \
      org.opencontainers.image.source="https://github.com/GibranKhalil/AthenaEnv" \
      org.opencontainers.image.licenses="MIT" \
      org.opencontainers.image.vendor="AthenaEnv"

# Set environment variables for PS2 toolchains
ENV PS2DEV=/usr/local/ps2dev
ENV PS2SDK=$PS2DEV/ps2sdk
ENV PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin

# Build arguments for non-root user (default to 1000:1000)
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=1000

# Create non-root user and ensure permissions on workdir
RUN groupadd --gid ${USER_GID} ${USERNAME} 2>/dev/null || true && \
    useradd --uid ${USER_UID} --gid ${USER_GID} -m -s /bin/bash ${USERNAME} 2>/dev/null || true && \
    mkdir -p /src && \
    chown -R ${USER_UID}:${USER_GID} /src

WORKDIR /src

# Switch to non-root user
USER ${USERNAME}

# Default command
CMD ["make", "clean", "all"]
