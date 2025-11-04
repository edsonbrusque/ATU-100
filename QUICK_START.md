# Quick Start Guide

## Want to build firmware without installing anything?

### Option 1: GitHub Actions (Cloud Build)

**Zero installation required!**

1. Push your code to GitHub
2. Go to the "Actions" tab in your repository
3. Wait for the build to complete (~3-5 minutes)
4. Download the compiled `.hex` file from "Artifacts"

**That's it!** The firmware is ready to program to your PIC16F1938.

---

### Option 2: Docker (Local Build)

**No MPLAB X needed, just Docker**

```bash
# One-time setup
docker build -t atu100-builder .

# Build firmware
docker run --rm -v $(pwd):/work atu100-builder
```

Output: `ATU_100_EXT_board/FirmWare_PIC16F1938/dist/default/production/*.hex`

---

### Option 3: MPLAB X IDE

Traditional method - see [BUILD.md](BUILD.md) for details.

---

## After Building

1. Locate the `.hex` file:
   - GitHub Actions: Download from workflow artifacts
   - Docker/MPLAB X: `ATU_100_EXT_board/FirmWare_PIC16F1938/dist/default/production/FirmWare_PIC16F1938.production.hex`

2. Program your PIC16F1938:
   - Use PICkit 3/4 or compatible programmer
   - Load the `.hex` file
   - Program the device

3. Connect and test:
   - Connect UART at 9600 baud (8N1)
   - Send commands: `A` (auto), `B` (bypass), `R` (reset), `T` (tune)
   - Monitor status output

---

## Need More Details?

- **[BUILD.md](BUILD.md)** - Complete build instructions for all methods
- **[README.md](README.md)** - Project overview and usage
- **[.github/workflows/README.md](.github/workflows/README.md)** - GitHub Actions details

## Troubleshooting

**GitHub Actions build fails:**
- Check the workflow logs in the Actions tab
- Ensure all source files are committed

**Docker build fails:**
- Ensure Docker is running: `docker --version`
- Check disk space: `df -h`

**Need help?**
- Open an issue on GitHub
- Check the build logs for error messages
