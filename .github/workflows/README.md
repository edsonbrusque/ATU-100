# GitHub Actions Workflows

This directory contains automated CI/CD workflows for building the ATU-100 firmware.

## Build Firmware Workflow

**File:** `build-firmware.yml`

### What it does

Automatically compiles the PIC16F1938 firmware using the Microchip XC8 compiler whenever you:
- Push code to `main`, `master`, or any `claude/**` branch
- Create a pull request
- Manually trigger the workflow

### Features

✅ **No local installation needed** - Builds entirely in the cloud
✅ **Automatic XC8 installation** - Downloads and installs the compiler automatically
✅ **Caching** - Caches the compiler between runs for faster builds
✅ **Artifact storage** - Saves the compiled `.hex` file for 90 days
✅ **Build summary** - Shows build status and file sizes in the workflow run

### How to use

1. **View build status:**
   - Go to the "Actions" tab in your GitHub repository
   - Click on the latest "Build ATU-100 Firmware" workflow run

2. **Download compiled firmware:**
   - Open a completed workflow run
   - Scroll to the "Artifacts" section at the bottom
   - Download `ATU-100-firmware-hex.zip`
   - Extract to get the `.hex` file ready for programming

3. **Manual build:**
   - Go to Actions → "Build ATU-100 Firmware"
   - Click "Run workflow" button
   - Select the branch and click "Run workflow"

### Adding a build badge to your README

Add this to the top of your main README.md:

```markdown
![Build Status](https://github.com/YOUR_USERNAME/ATU-100/actions/workflows/build-firmware.yml/badge.svg)
```

Replace `YOUR_USERNAME` with your GitHub username.

### Requirements

- GitHub repository must be public OR you need GitHub Actions minutes on private repos
- No local setup required - everything runs in the cloud!

### Troubleshooting

**Build fails during XC8 installation:**
- The XC8 download URL may have changed. Check [Microchip's website](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers) for the latest version.

**Build succeeds but no hex file:**
- Check the workflow logs for compilation errors
- Ensure all source files are committed to the repository

**Want to use a different XC8 version:**
- Edit the download URL in the workflow file
- Update the cache key to match the new version
- Update the PATH to match the new version number

## Alternative: Local Build with Docker

If you prefer to build locally without installing MPLAB X, you can use Docker:

```bash
# Create a Dockerfile in the project root (see Docker example below)
docker build -t atu100-builder .
docker run --rm -v $(pwd):/work atu100-builder
```

The compiled firmware will appear in `ATU_100_EXT_board/FirmWare_PIC16F1938/dist/default/production/`.
