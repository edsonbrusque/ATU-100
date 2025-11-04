# Dockerfile for building ATU-100 firmware without MPLAB X IDE
# This allows you to build the firmware using only Docker

FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    wget \
    make \
    git \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Download and install Microchip XC8 Compiler v2.50
RUN wget -q https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareLibraries/Firmware/xc8-v2.50-full-install-linux-x64-installer.run \
    -O /tmp/xc8-installer.run && \
    chmod +x /tmp/xc8-installer.run && \
    /tmp/xc8-installer.run --mode unattended --unattendedmodeui none \
        --netservername localhost --LicenseType FreeMode \
        --prefix /opt/microchip/xc8 && \
    rm /tmp/xc8-installer.run

# Add XC8 to PATH
ENV PATH="/opt/microchip/xc8/v2.50/bin:${PATH}"

# Set working directory
WORKDIR /work

# Default command: build the firmware
CMD ["sh", "-c", "cd ATU_100_EXT_board/FirmWare_PIC16F1938 && cp nbproject/Makefile-local-default.mk.linux nbproject/Makefile-local-default.mk && make clean && make build && echo '\n=== Build Complete ===' && ls -lh dist/default/production/*.hex"]
