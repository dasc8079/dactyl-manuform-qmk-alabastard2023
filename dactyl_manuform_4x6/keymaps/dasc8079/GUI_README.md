# WebHID GUI for Dactyl-Manuform 4x6 Trackball

Browser-based acceleration curve editor for real-time firmware parameter adjustment.

## Requirements

- **Browser**: Chrome or Edge 89+ (WebHID support required)
- **Firmware**: Must have `RAW_ENABLE = yes` compiled in
- **OS**: Any (WebHID is cross-platform)

## Quick Start

1. Flash firmware with Raw HID enabled:
   ```bash
   make handwired/dactyl_manuform/4x6:dasc8079:flash
   ```

2. Open `qmk_gui.html` in Chrome/Edge

3. Click "Connect to Keyboard" and select your device

4. Adjust parameters in real-time:
   - Pointer Mode acceleration (X/Y quadratic coefficients)
   - Caret Mode acceleration (X/Y quadratic coefficients)
   - Base sensitivity (linear multiplier)
   - Mouse button timeout (ms)

5. Click "Send to Keyboard" to apply changes immediately

6. Click "Save to EEPROM" to persist settings across reboots

## Features

### Real-Time Parameter Control
- **Pointer Mode Acceleration**: Adjust X/Y quadratic coefficients (default: 5.0, 4.8)
- **Caret Mode Acceleration**: Adjust X/Y quadratic coefficients (default: 0.25, 0.25)
- **Base Sensitivity**: Runtime sensitivity multiplier (default: 0.1)
- **Mouse Timeout**: Auto-deactivate timeout in ms (default: 750ms)

### Visual Curve Editor
- Real-time acceleration curve preview
- Separate graphs for Pointer and Caret modes
- Visual feedback for parameter changes

### Profile Management
- **Export Profile**: Save current settings as JSON
- **Import Profile**: Load previously saved settings
- **Reset Defaults**: Restore factory settings

### EEPROM Persistence
- All settings stored in 16 bytes of EEPROM
- Auto-load on keyboard boot
- Magic byte validation (0xAC)

## HID Protocol Commands

| Command | Code | Data | Description |
|---------|------|------|-------------|
| GET_CONFIG | 0x01 | - | Read all current parameters |
| SET_POINTER_X | 0x02 | int16 | Set pointer X quadratic coefficient |
| SET_POINTER_Y | 0x03 | int16 | Set pointer Y quadratic coefficient |
| SET_CARET_X | 0x04 | int16 | Set caret X quadratic coefficient |
| SET_CARET_Y | 0x05 | int16 | Set caret Y quadratic coefficient |
| SET_TIMEOUT | 0x06 | uint16 | Set mouse button timeout (ms) |
| SET_BASE_SENS | 0x07 | int16 | Set base sensitivity |
| SAVE_EEPROM | 0x08 | - | Save current config to EEPROM |
| RESET_DEFAULTS | 0x09 | - | Restore factory defaults |

All coefficients use 8.8 fixed-point format (value * 256).

## Troubleshooting

**"Not connected" error:**
- Ensure Chrome/Edge 89+ is being used
- Check that RAW_ENABLE = yes in firmware
- Try different USB port
- Restart browser

**Settings not persisting:**
- Click "Save to EEPROM" after adjusting
- Verify EEPROM write succeeded (check feedback)
- Reset keyboard to test auto-load

**Acceleration feels wrong:**
- Export current profile as backup
- Click "Reset to Defaults"
- Gradually adjust from factory settings
- Use curve preview to visualize changes

## Technical Details

### Fixed-Point Math (8.8 Format)
- **Integer bits**: 8 (values 0-255)
- **Fractional bits**: 8 (precision to 1/256)
- **Range**: 0.00390625 to 255.996
- **Conversion**: `float_value * 256 = fixed_point_value`

### Acceleration Formula
```c
// Pointer/Scroll mode:
output = (input² × accel_quad) + (input × base_sens × 0.6)

// Caret mode:
output = (input² × accel_quad) + (input × linear_mult)
```

### EEPROM Structure (16 bytes)
```c
struct {
    uint8_t  magic;           // 0xAC validation
    int16_t  pointer_accel_x; // Pointer X coefficient
    int16_t  pointer_accel_y; // Pointer Y coefficient
    int16_t  caret_accel_x;   // Caret X coefficient
    int16_t  caret_accel_y;   // Caret Y coefficient
    uint16_t mouse_timeout;   // Timeout in ms
    int16_t  base_sensitivity;// Base sensitivity
    uint8_t  reserved[3];     // Future use
}
```

## Development

The GUI is a single self-contained HTML file with embedded CSS and JavaScript. No build process required.

### File Structure
- `qmk_gui.html` - Complete WebHID interface
- `keymap.c` - Firmware with Raw HID protocol
- `rules.mk` - RAW_ENABLE = yes

### Extending the Protocol
1. Add new command to HID_CMD enum
2. Implement handler in raw_hid_receive()
3. Add UI control in qmk_gui.html
4. Update EEPROM structure if needed
