# Dockerfile for AthenaEnv (PS2 Homebrew)
# Based on official PS2 Homebrew toolchain container
FROM ghcr.io/ps2homebrew/ps2homebrew:main

WORKDIR /src

# Set default environment variables for PS2SDK
ENV PS2DEV=/usr/local/ps2dev
ENV PS2SDK=$PS2DEV/ps2sdk
ENV PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin

# Default command compiles the project
CMD ["make", "clean", "all"]
