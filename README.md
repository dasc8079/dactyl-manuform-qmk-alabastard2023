# Dactyl-Manuform 4x6 PMW3360 Trackball Keyboard

Custom QMK firmware for a Dactyl-Manuform 4x6 split keyboard with integrated PMW3360 trackball sensor.

## Quick Start

### Flash Firmware
```bash
cd /Users/darrenschaeffer/Documents/QMK_Keyboards

# Put keyboard in bootloader mode (press reset button)

dfu-programmer atmega32u4 erase --force && \
dfu-programmer atmega32u4 flash --force dactyl_manuform_4x6_dasc8079.hex && \
dfu-programmer atmega32u4 reset
```

### Recompile (from full QMK repo)
```bash
cd /Users/darrenschaeffer/Desktop/qmk_2023
qmk compile -kb handwired/dactyl_manuform/4x6 -km dasc8079
```

## Repository Structure

```
QMK_Keyboards/
├── dactyl_manuform_4x6_dasc8079.hex    # Compiled baseline firmware (ready to flash)
├── dactyl_manuform_4x6/                # Keyboard configuration
│   ├── config.h                        # Hardware configuration
│   ├── rules.mk                        # Build configuration
│   └── keymaps/
│       └── dasc8079/                   # Active tuned keymap
│           ├── keymap.c                # Main keymap with acceleration curves
│           ├── config.h                # Keymap-specific config
│           └── rules.mk                # Keymap build rules
├── CLAUDE.md                           # Complete project documentation
└── PORTING_ANALYSIS.md                 # Technical analysis & compatibility notes
```

## Current Configuration

**Firmware:**
- QMK Version: 0.18.17-611-g4eda1fea51 (Alabastard fork)
- Branch: `feature_pointing_device_scroll_framework`
- Size: 28,486 / 28,672 bytes (186 bytes free)

**Acceleration Curves:**
- Pointer/Scroll: x² × 5.0 + x × base_sens × 0.6 (X), x² × 4.8 + x × base_sens × 0.6 (Y)
- Caret Mode: x² × 0.25 + x × 1.3 (X), x² × 0.25 + x × 1.0 (Y)
- Base sensitivity: 0.1 (adjustable via K+O/K+U)

**Key Features:**
- Mouse button timeout: 750ms
- Axis snapping: Symmetric 1.3× threshold
- Auto J/L/H mouse button switching
- Multiple modes: pointer, scroll, caret, volume

## Documentation

- **[CLAUDE.md](CLAUDE.md)** - Complete project history, features, and implementation plans
- **[PORTING_ANALYSIS.md](PORTING_ANALYSIS.md)** - Why modern QMK won't work (PMW3360 SROM requirement)

## Important Notes

⚠️ **Do NOT update to modern QMK** - PMW3360 sensor requires SROM firmware that was removed in newer versions

⚠️ **Full QMK repo required for compilation** - This repo contains only keyboard-specific files. Use `/Users/darrenschaeffer/Desktop/qmk_2023/` for compilation.

## Next Steps

See `CLAUDE.md` for planned features:
- Custom WebHID parameter control GUI
- Tap-hold timing improvements
- On-screen keymap display
- VIA-style keymap editor (if space allows)

---

**Last Updated:** October 2, 2025
**Status:** ✅ Baseline firmware working and preserved
