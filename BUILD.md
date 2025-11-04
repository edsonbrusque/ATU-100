# Building ATU-100 Firmware

This document describes all available methods for building the ATU-100 firmware.

## Table of Contents

1. [Online Build (GitHub Actions)](#online-build-github-actions) - **No installation required!**
2. [Docker Build](#docker-build) - Local build without MPLAB X
3. [Traditional Build](#traditional-build) - Using MPLAB X IDE

---

## Online Build (GitHub Actions)

**✨ Recommended for most users - requires zero local installation!**

### How it works

When you push code to GitHub, it automatically:
1. Installs the XC8 compiler in the cloud
2. Builds your firmware
3. Saves the `.hex` file as a downloadable artifact

### Setup (one-time)

The workflow is already configured in `.github/workflows/build-firmware.yml`. Just push your code!

### Using the online build

1. **Push your code to GitHub:**
   ```bash
   git add .
   git commit -m "Update firmware"
   git push
   ```

2. **Check build status:**
   - Go to your repository on GitHub
   - Click the "Actions" tab
   - View the "Build ATU-100 Firmware" workflow

3. **Download compiled firmware:**
   - Open a successful workflow run
   - Scroll to "Artifacts" section
   - Download `ATU-100-firmware-hex.zip`
   - Extract the `.hex` file

### Manual trigger

You can also trigger builds manually:
1. Go to Actions → "Build ATU-100 Firmware"
2. Click "Run workflow"
3. Select branch and run

### Build badge

Add this to your README to show build status:

```markdown
![Build Status](https://github.com/YOUR_USERNAME/ATU-100/actions/workflows/build-firmware.yml/badge.svg)
```

---

## Docker Build

**For local builds without installing MPLAB X IDE**

### Prerequisites

- Docker installed on your system
- 4GB+ free disk space (for compiler)

### Method 1: Using Docker Compose (easiest)

```bash
# Build the firmware
docker-compose run builder

# Clean and rebuild
docker-compose run builder sh -c "cd ATU_100_EXT_board/FirmWare_PIC16F1938 && make clean && make build"
```

### Method 2: Using Docker directly

```bash
# Build the Docker image (first time only, ~5 minutes)
docker build -t atu100-builder .

# Build the firmware
docker run --rm -v $(pwd):/work atu100-builder

# On Windows PowerShell, use:
docker run --rm -v ${PWD}:/work atu100-builder
```

### Output location

The compiled firmware will be in:
```
ATU_100_EXT_board/FirmWare_PIC16F1938/dist/default/production/FirmWare_PIC16F1938.production.hex
```

### Advantages

✅ No MPLAB X installation needed
✅ Consistent build environment
✅ Works on Linux, macOS, and Windows
✅ Easy to reproduce builds

---

## Traditional Build

**Using MPLAB X IDE (the original method)**

### Prerequisites

1. **MPLAB X IDE**
   - Download from: https://www.microchip.com/mplab/mplab-x-ide
   - Latest version tested: v6.20

2. **XC8 Compiler**
   - Download from: https://www.microchip.com/mplab/compilers
   - Free version works fine
   - Tested version: v2.50

### Building with MPLAB X IDE

1. Open MPLAB X IDE
2. File → Open Project
3. Navigate to `ATU_100_EXT_board/FirmWare_PIC16F1938`
4. Click "Build Project" (hammer icon)
5. Output: `dist/default/production/FirmWare_PIC16F1938.production.hex`

### Building from command line

If you have MPLAB X and XC8 installed:

```bash
cd ATU_100_EXT_board/FirmWare_PIC16F1938
make clean
make build
```

**Note:** You may need to add XC8 to your PATH:
```bash
# Linux/macOS
export PATH="/opt/microchip/xc8/v2.50/bin:$PATH"

# Windows
set PATH=C:\Program Files\Microchip\xc8\v2.50\bin;%PATH%
```

---

## Troubleshooting

### GitHub Actions build fails

**XC8 download fails:**
- The download URL may have changed
- Edit `.github/workflows/build-firmware.yml`
- Update the XC8 download URL from Microchip's website

**Build succeeds but no artifact:**
- Check workflow logs for compilation errors
- Ensure all source files are committed

### Docker build fails

**"Cannot connect to Docker daemon":**
```bash
# Linux: Ensure Docker service is running
sudo systemctl start docker

# Ensure your user is in docker group
sudo usermod -aG docker $USER
# Then log out and back in
```

**Out of disk space:**
- Docker images can be large
- Clean up old images: `docker system prune -a`

### MPLAB X build fails

**Compiler not found:**
- Verify XC8 is installed
- Check compiler path in MPLAB X: Tools → Options → Embedded

**Project won't open:**
- Ensure you're opening the `nbproject` folder
- Try: File → Open Project → select `FirmWare_PIC16F1938` folder

---

## Comparison of Build Methods

| Method | Setup Time | Build Time | Pros | Cons |
|--------|-----------|------------|------|------|
| **GitHub Actions** | 0 min | ~3-5 min | Zero install, automatic, archived builds | Requires GitHub, needs internet |
| **Docker** | 5 min | ~2 min | Local, reproducible, no IDE | Large image (~2GB), needs Docker |
| **MPLAB X** | 15 min | ~1 min | Full IDE features, debugging | Large install (~2GB), platform-specific |

---

## Continuous Integration

The GitHub Actions workflow automatically runs on:
- Push to `main`, `master`, or `claude/**` branches
- Pull requests
- Manual workflow dispatch

Artifacts are retained for **90 days**.

---

## Build Configuration

### Compiler Settings

The build uses these XC8 flags:
- `-mcpu=16F1938` - Target PIC16F1938
- `-O2` - Optimization level 2
- `-std=c99` - C99 standard
- `-fno-short-double -fno-short-float` - Use full precision
- `-mstack=compiled:auto:auto` - Automatic stack management

### Source Files

All files in `ATU_100_EXT_board/FirmWare_PIC16F1938/`:
- `main.c` - Main tuning logic
- `pic_init.c` - Hardware initialization
- `cross_compiler.c` - Compiler abstraction
- `uart.c` - Serial communication
- `test_modes.c` - Test utilities

### Output Files

- **Production hex:** `dist/default/production/FirmWare_PIC16F1938.production.hex`
- **Debug elf:** `dist/default/debug/FirmWare_PIC16F1938.debug.elf`
- **Listing files:** `build/default/production/*.lst`

---

## Need Help?

- Check the [main README](README.md) for project documentation
- Review GitHub Actions logs for build errors
- Open an issue on GitHub for build-related problems
