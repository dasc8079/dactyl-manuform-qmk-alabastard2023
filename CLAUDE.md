# Dactyl-Manuform 4x6 Trackball Keyboard Firmware Project

## 🎯 Project Status: MODERN QMK INCOMPATIBLE - Using 2023 Fork
**Last Updated**: October 1, 2025
**Current Status**: ✅ All improvements implemented and tested!

---

## ⚠️ CRITICAL: This Uses Alabastard's 2023 Custom QMK Fork

**Repository**: `/Users/darrenschaeffer/Desktop/qmk_2023/`
**Branch**: `feature_pointing_device_scroll_framework`
**QMK Version**: 0.18.17-611-g4eda1fea51 (November 2022)
**Status**: ✅ Confirmed working (Oct 1, 2025)

### Why Custom Fork is Required:
1. **SROM Firmware**: This PMW3360 sensor requires SROM firmware for motion detection
2. **Modern QMK Incompatible**: QMK removed SROM in Nov 2024 for GPL licensing
3. **Extensive Testing**: Confirmed modern QMK cannot work with this sensor (Oct 1, 2025)
4. **Custom Scroll Framework**: Alabastard's SM_DRAG/SM_CARET/SM_VOLUME not in standard QMK

**📖 See**: `PORTING_ANALYSIS.md` for complete technical analysis

---

## Hardware Configuration

- **Keyboard**: Dactyl-Manuform 4x6 split keyboard
- **Trackball**: PMW3360 sensor (right side, POINTING_DEVICE_RIGHT)
- **Microcontroller**: ATmega32U4 (28KB flash)
- **Bootloader**: qmk-dfu
- **Connection**: I2C split communication
- **Status**: ✅ Working (flashed Oct 1, 2025)

---

## Quick Start: Compile & Flash

### Working Configuration Location
**Path**: `/Users/darrenschaeffer/Desktop/qmk_2023/`

### Compile
```bash
cd /Users/darrenschaeffer/Desktop/qmk_2023
make clean
make handwired/dactyl_manuform/4x6:dasc8079
```

**Current Output**: `handwired_dactyl_manuform_4x6_dasc8079.hex`
**Size**: 27,780 / 28,672 bytes (96%, **892 bytes free**)

### Flash to Keyboard
```bash
qmk flash -kb handwired/dactyl_manuform/4x6 -km dasc8079
# Press reset button when prompted
```

Or manual flash:
```bash
dfu-programmer atmega32u4 erase --force && \
dfu-programmer atmega32u4 flash --force handwired_dactyl_manuform_4x6_dasc8079.hex && \
dfu-programmer atmega32u4 reset
```

### Latest Working Firmware
**File**: `handwired_dactyl_manuform_4x6_dasc8079.hex`
**Keymap**: dasc8079
**Compiled**: October 1, 2025
**Size**: 27,780 bytes (892 bytes free)
**Status**: ✅ All improvements implemented (see Recent Changes below)

---

## Recent Changes (October 1, 2025)

### ✅ NEW: Mouse Button Timeout (Auto-Mouse Layer)
**Status**: ✅ Implemented

**Feature**: J, L, H keys dynamically switch between typing and mouse buttons based on trackball activity

**Behavior**:
- **Mouse Mode Active**: When trackball is moved OR within 500ms of last movement
  - J key = Left click (BTN1)
  - L key = Right click (BTN2)
  - H key = Middle click (BTN3)
- **Typing Mode**: After 500ms of trackball inactivity OR any non-thumb key press
  - J, L, H return to normal typing keys

**Timeout**: 500ms after last trackball movement

**Key Exclusions**: Thumb buttons and layer keys don't cancel mouse mode
- Space, Enter, Ctrl, Alt, Cmd, Back/Forward buttons
- Layer hold keys (K, Home, End)

**Implementation**: keymap.c:145-180 (process_record_user)

### ✅ NEW: Axis Snapping for Scroll and Caret Modes
**Status**: ✅ Implemented

**Feature**: Trackball movement snaps to strongest axis (horizontal OR vertical, never diagonal)

**Behavior**:
- If abs(X) > abs(Y): Zero out Y movement, commit to horizontal
- If abs(Y) >= abs(X): Zero out X movement, commit to vertical

**Modes Affected**:
- **Scroll Mode** (Layer 4): Prevents diagonal scrolling
- **Caret Mode** (Layer 5): Prevents diagonal arrow key navigation

**Implementation**: keymap.c:76-81 (caret), keymap.c:95-100 (scroll)

### ✅ FIXED: Caret Mode Acceleration
**Status**: ✅ Completed

**Problem**: Caret mode had no acceleration (1:1 movement), felt disconnected from pointer

**Solution**: Applied gentle quadratic acceleration for arrow key navigation
```c
// Caret mode now uses (keymap.c:90-91):
x = abs_x * abs_x * 0.8 + abs_x * base_sens * 0.6;  // Symmetric X and Y
y = abs_y * abs_y * 0.8 + abs_y * base_sens * 0.6;
```

**Benefits**:
- Gentle acceleration (0.8 coefficient, half of pointer's 1.6)
- Symmetric X/Y acceleration (no directional bias)
- More controlled than pointer mode, suitable for arrow key navigation
- Retains precision for small movements via axis snapping

### ✅ FIXED: Mac Browser Navigation (Forward/Back)
**Status**: ✅ Implemented

**Problem**: BTN4/BTN5 mouse buttons didn't work for forward/back in Mac Safari/Finder

**Solution**: Replaced with cross-platform system keycodes
- **Old**: MT(MOD_RSFT, KC_BTN4), MT(MOD_RGUI, KC_BTN5)
- **New**: MT(MOD_RSFT, KC_WWW_BACK), MT(MOD_RGUI, KC_WWW_FORWARD)

**Benefits**: Works on both Mac and Windows in browsers and file managers

**Location**: Right thumb buttons (keymap.c:156)

### ✅ FIXED: Cross-Platform Sleep Function
**Status**: ✅ Implemented

**Problem**: KC_SLEP may not work reliably on Mac

**Solution**: Replaced with system sleep keycode
- **Old**: KC_SLEP
- **New**: KC_SYSTEM_SLEEP

**Key**: Hold K + press ! (exclamation mark key)

**Benefits**: Works reliably on both Mac and Windows

**Location**: Layer 4 (keymap.c:201)

### ✅ UPDATED: Sensitivity Adjustment Key Moved
**Status**: ✅ Completed

**Change**: Moved increase sensitivity from I to O key
- **Decrease**: Hold K + U (unchanged)
- **Increase**: Hold K + O (was I)

**Reason**: Better ergonomics and avoid conflict with other keys

**Location**: Layer 4 (keymap.c:198)

### ⚙️ OPTIMIZATION: Console Debugging Disabled
**Status**: ✅ Completed

**Change**: Disabled CONSOLE_ENABLE to save space for new features

**Space Saved**: ~1,400 bytes

**Result**: Firmware size reduced from 28,660 bytes (12 free) to 27,748 bytes (924 free)

**Trade-off**: No runtime console debugging, but all features fit comfortably

**Note**: Can re-enable for debugging by setting `CONSOLE_ENABLE = yes` in rules.mk

---

## Current Firmware Size Constraint

**Capacity**: 28,672 bytes (ATmega32U4 limit)
**Used**: 27,780 bytes
**Free**: **892 bytes** (96% full)

### Implications
- **Comfortable space** for small features and tweaks
- **VIA support**: Still needs 2,118 bytes (would require ~1,200 bytes optimization)
- **Console debugging**: Can be re-enabled if needed (~1,400 bytes cost)

### Size Reduction Options (if more space needed)
1. Remove unused layers: ~200 bytes per layer
2. Simplify mouse timeout logic: ~100-200 bytes
3. Remove axis snapping: ~50 bytes per mode
4. Disable NKRO: ~200 bytes

---

## Trackball Configuration

### Current Modes

#### Default Mode (Layer 0)
- **CPI**: 100
- **Acceleration**: Tuned quadratic `x² × 1.6 + x × 0.6 × base_sens`
- **Base Sensitivity**: 0.5 (adjustable via Layer 4 + U/I)
- **Status**: ✅ Working well

#### Scroll Mode (Layer 4) - Hold K
- **CPI**: 100
- **Acceleration**: Same as default (tuned quadratic)
- **Scroll Framework**: SM_DRAG (converts X/Y to scroll wheel H/V)
- **Divisor**: 16 (from Alabastard scroll framework)
- **Features**: Axis snapping (prevents diagonal scrolling)

#### Caret Mode (Layer 5) - Hold N or M
- **CPI**: 300
- **Acceleration**: Gentle quadratic (symmetric X/Y: both use 0.8 coefficient)
  - Formula: `x² × 0.8 + x × 0.6 × base_sens`
  - Half the strength of pointer mode for precise arrow key control
- **Scroll Framework**: SM_CARET (trackball → arrow keys)
- **Divisor**: 32 (from Alabastard scroll framework)
- **Features**: Axis snapping (prevents diagonal movement)

#### Volume Mode (Layer 6) - Hold Comma
- **CPI**: 200
- **Acceleration**: 2× on Y-axis only
- **Scroll Framework**: SM_VOLUME (trackball Y → volume control)
- **Divisor**: 32 (from Alabastard scroll framework)

### Mouse Button Timeout (Auto-Mouse Layer)

**Feature**: J, L, H keys automatically become mouse buttons when trackball is active

**Timeout**: 500ms after last trackball movement

**Mouse Mode Keys**:
- **J** = Left click (BTN1)
- **L** = Right click (BTN2)
- **H** = Middle click (BTN3)

**Deactivation Triggers**:
- 500ms timeout with no trackball movement
- Any non-thumb key press (excludes Space, Enter, Ctrl, Alt, Cmd, layer keys)

**Benefits**:
- No dedicated mouse button layer needed
- Seamless switching between typing and mouse control
- Trackball movement instantly activates mouse buttons
- Natural timeout returns to typing mode

---

## Known Issues & Future Work

### 1. Mode Switching Flags (Low Priority)
**Status**: Minor issue, not critical

**Problem**: Caret and volume mode don't fully reset all flags in `layer_state_set_user()`

**Location**: keymap.c lines 40-58

**Fix Needed**:
```c
case _LAYER5:  // Caret mode
    set_scroll_mode(SM_CARET);
    pointing_device_set_cpi(300);
    caret_mode = true;
    scroll_mode = false;    // ADD THIS
    volume_mode = false;    // ADD THIS
    default_mode = false;   // ADD THIS
    break;
```

### 2. 🎯 PLANNED: Web-Based Acceleration Curve Editor
**Status**: Feasibility confirmed, implementation deferred

**Goal**: Create visual tool to design custom acceleration curves

**Features**:
- Standalone HTML file (no installation)
- Visual curve editor with draggable control points
- Real-time preview of RAW input → OUTPUT mapping
- Export C code for firmware
- Test curves before flashing

**Implementation**: Lookup table approach
```c
// Easy to tune - just change output values!
const int16_t accel_curve_x[] = {0, 1,  2,  3,  4,  5,   6,   7,   8};
const int16_t accel_out_x[] =  {0, 1,  5, 12, 24, 42,  68, 105, 150};
```

**When**: After current acceleration is validated through real-world use

### 3. Requested Features (Backlog)

#### Paste-Match-Style Macro
- **Keycode**: Opt+Shift+Cmd+V (Mac)
- **Trigger**: Hold mouse button 3
- **Status**: Not implemented
- **Space Required**: ~50 bytes
- **Note**: 924 bytes available, can be implemented

#### ✅ COMPLETED: Fix MB3/MB4 Platform Compatibility
- **Status**: ✅ Implemented using KC_WWW_BACK and KC_WWW_FORWARD

#### ✅ COMPLETED: Replace KC_SLEP with Mac-Compatible Sleep
- **Status**: ✅ Implemented using KC_SYSTEM_SLEEP

#### ✅ COMPLETED: Mouse Button Timeout
- **Status**: ✅ Implemented with 500ms timeout (J/L/H auto-mouse keys)

---

## File Locations

### Working Repository
**Path**: `/Users/darrenschaeffer/Desktop/qmk_2023/`
- ✅ Clean, compilable
- ✅ Not in iCloud (avoids sync issues)
- ✅ Alabastard fork with scroll framework
- ✅ SROM included and working
- **USE THIS FOR ALL DEVELOPMENT**

### Key Files
- **Active Keymap**: `keyboards/handwired/dactyl_manuform/4x6/keymaps/dasc8079/keymap.c`
- **Default Keymap** (original state): `keyboards/handwired/dactyl_manuform/4x6/keymaps/default/keymap.c`
- **Config**: `keyboards/handwired/dactyl_manuform/4x6/config.h`
- **Rules**: `keyboards/handwired/dactyl_manuform/4x6/rules.mk`
- **Scroll Framework**: `quantum/pointing_device/pointing_device_scroll.h`

### Keymap Split
- **dasc8079**: All improvements (mouse timeout, axis snapping, gentle caret acceleration, etc.)
- **default**: Original state preserved (console debugging enabled, no new features)

### Backup Firmware (iCloud - Reference Only)
**Path**: `/Users/darrenschaeffer/Library/Mobile Documents/com~apple~CloudDocs/Keyboard/WORKING_CONFIG_Jan2023/`
- Reference only
- DO NOT compile from here (iCloud sync blocks builds)
- Keep for disaster recovery

### Corrupted Repository (DO NOT USE)
**Path**: `/Users/darrenschaeffer/Documents/QMK_Keyboards/alabastard_backup/qmk_firmware/`
- ❌ Compilation hangs
- ❌ Filesystem/corruption issues
- ❌ DO NOT USE

### Modern QMK (Incompatible - Can Delete)
**Path**: `/Users/darrenschaeffer/Documents/QMK_Keyboards/qmk_fresh_2025/`
- ❌ No SROM support
- ❌ Trackball doesn't work
- ❌ Extensively tested - confirmed incompatible
- Safe to delete (keep for reference only if desired)

---

## Testing Protocol

1. **Flash firmware** via QMK Toolbox or command line
2. **Unplug USB and TRRS** to power cycle
3. **Test default mode**: Slow movements and flicks
4. **Test mouse button timeout**:
   - Move trackball, immediately press J/L/H (should be mouse buttons)
   - Wait 500ms, press J/L/H (should be typing keys)
   - Move trackball, press any letter key (should cancel mouse mode)
5. **Test scroll mode** (hold K):
   - Verify acceleration feels good
   - Test axis snapping (horizontal/vertical only, no diagonal)
6. **Test caret mode** (hold N/M):
   - Check symmetric X/Y acceleration
   - Test axis snapping (arrow keys should snap to axis)
7. **Test volume mode** (hold comma): Y-axis volume control
8. **Test browser navigation**: Press thumb buttons (should go back/forward)
9. **Test sleep function**: Hold K + press ! (should sleep computer)
10. **Adjust sensitivity**: Layer 4 + U (down) or O (up) if needed

---

## Alabastard Scroll Framework (Custom QMK Feature)

### Available Modes

#### SM_DRAG (Scroll Mode)
- Converts trackball X/Y movement to scroll wheel H/V
- **Divisor**: 16 (configurable via `SCROLL_DRAG_DIVISOR`)
- **Features**: Axis snapping available
- **Use Case**: Document/web scrolling

#### SM_CARET (Arrow Key Mode)
- Trackball movement sends arrow key presses
- **Divisor**: 32 (configurable via `SCROLL_CARET_DIVISOR`)
- **Features**: Built-in axis snapping
- **Use Case**: Code navigation without mouse

#### SM_VOLUME (Volume Control)
- Trackball Y-axis controls system volume
- **Divisor**: 32 (configurable via `SCROLL_VOL_DIVISOR`)
- **Use Case**: Media control

### API Functions
```c
void set_scroll_mode(uint8_t scroll_mode);
uint8_t get_scroll_mode(void);
void scroll_mode_toggle(uint8_t scroll_mode);
```

**Documentation**: See `quantum/pointing_device/pointing_device_scroll.h`

---

## Advanced QMK Features (For Future Reference)

### Features Currently Missing

#### Auto Mouse Layer (HIGH PRIORITY)
**Purpose**: Automatically activate mouse button layer on trackball movement, deactivate after timeout

**Benefits**:
- Solves mouse button timeout requirement automatically
- No manual layer switching needed
- Trackball movement → instant mouse button access
- Configurable timeout returns to keyboard mode

**Implementation**:
```c
// config.h
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_TIME 650              // Layer timeout (ms)
#define AUTO_MOUSE_DELAY 200             // Lockout after keyboard key
#define AUTO_MOUSE_THRESHOLD 10          // Minimum movement to trigger

// keymap.c
void pointing_device_init_user(void) {
    set_auto_mouse_layer(1);
    set_auto_mouse_enable(true);
}
```

**Blocker**: Modern QMK feature (not in 0.18.17 fork)

#### Sniping Mode (Precision DPI)
**Purpose**: Temporary ultra-low DPI for pixel-perfect cursor control

**Implementation**: Hold "Snipe" key → DPI drops to 200-300

**Blocker**: Not in 2023 fork

#### Runtime DPI Cycling
**Purpose**: Cycle through DPI presets without reflashing

**Implementation**: Custom keycodes `DPI_UP`, `DPI_DN`, `DPI_CYCLE`

**Blocker**: Space constraint (12 bytes free)

#### Chordal Hold / Bilateral Combinations
**Purpose**: Eliminate tap-hold timing ambiguity using "opposite hands" rule

**Use Case**: Fix thumb modifier issues (Space/Enter dual-function)

**Blocker**: Modern QMK feature

#### QMK Combos (Key Chording)
**Purpose**: Press multiple keys simultaneously for complex actions

**Example**: Q + W = Undo, E + R = Redo

**Blocker**: Space constraint

---

## Development Notes

### Console Debugging Workflow
1. Enable console: `CONSOLE_ENABLE = yes` in rules.mk
2. Disable mousekeys to free space: `MOUSEKEY_ENABLE = no`
3. Add uprintf() statements in keymap.c
4. Compile and flash
5. Monitor: `qmk console`

### Acceleration Curve Tuning Process
**Goal**: Balance precision at low speeds with full-screen traversal on flicks

**Method**:
1. Add console debugging to see RAW → OUTPUT mapping
2. Test typical movements (small adjustments + big flicks)
3. Identify sweet spot for quadratic and linear coefficients
4. Iterate until both precision and traversal feel right

**Results** (Oct 1, 2025):
- Quadratic: 1.6 (X), 2.0 (Y)
- Linear: 0.6 × base_sens
- Base sensitivity: 0.5 (user-adjustable)

### Asymmetric X/Y Acceleration
**Reason**: User preference for vertical flicks to be more aggressive

**Implementation**:
- X-axis: Quadratic × 1.6
- Y-axis: Quadratic × 2.0 (25% more aggressive)

**Result**: Vertical flicks feel more responsive for scrolling

---

## Important Learnings (October 1, 2025)

### 1. SROM Firmware is Hardware-Specific
- This PMW3360 sensor variant REQUIRES SROM firmware
- Newer sensor variants may work without SROM
- Cannot be determined without physical testing
- Modern QMK removed SROM → incompatible with this hardware

### 2. Custom Forks Can Be Permanent
- Alabastard's scroll framework is valuable custom code
- Not merged into mainline QMK
- Must maintain fork indefinitely
- **This is OK** - fork is stable and functional

### 3. Firmware Size Matters for ATmega32U4
- 28KB flash limit is real constraint
- At 99.9% capacity, every feature is a tradeoff
- Must optimize before adding features
- Modern QMK features often exceed available space

### 4. Formula-Based Acceleration is Hard to Tune
- Math formulas are difficult to visualize
- Small coefficient changes have non-linear effects
- **Better approach**: Lookup table with visual curve editor
- Allows direct tuning of each input → output mapping

### 5. Console Debugging is Essential for Tuning
- Cannot tune acceleration "blind"
- Seeing RAW → OUTPUT values reveals actual behavior
- Worth the 1,400 byte overhead during development
- Disable for production builds

---

## Repository Management

### Keep These Directories
- ✅ `/Users/darrenschaeffer/Desktop/qmk_2023/` - **WORKING REPOSITORY**
- ✅ `/Users/darrenschaeffer/Library/.../WORKING_CONFIG_Jan2023/` - Backup reference
- ✅ `/Users/darrenschaeffer/Documents/Newest Old Version/` - Documentation

### Safe to Delete (After Verification)
- ❌ `/Users/darrenschaeffer/Documents/QMK_Keyboards/alabastard_backup/` - Corrupted
- ❌ `/Users/darrenschaeffer/Documents/QMK_Keyboards/qmk_fresh_2025/` - Modern QMK (incompatible)
- ❌ Any other QMK directories that aren't the working qmk_2023 folder

**Before deleting**: Verify you have working firmware flashed and qmk_2023 compiles successfully!

---

## Next Steps

### Immediate (Ready to Implement)
1. ✅ Acceleration curve tuned - validate through daily use
2. ⏳ Monitor for any issues with current curve
3. ⏳ Test runtime sensitivity adjustment in real workflows

### Short Term (When validated)
1. Create web-based acceleration curve editor (visual tuning tool)
2. Convert acceleration to lookup table approach
3. Fix mode switching flag bugs (if they cause issues)

### Long Term (Space Permitting)
1. Optimize firmware to free space for new features
2. Add paste-match-style macro
3. Fix MB3/MB4 platform compatibility
4. Replace KC_SLEP with Mac-compatible sleep

### Blocked (Need Modern QMK or More Space)
- Auto mouse layer (modern QMK feature)
- VIA support (needs 2,118 bytes)
- Sniping mode (modern QMK feature)
- Chordal hold (modern QMK feature)

---

## 🎨 PLANNED: Custom Web GUI for Parameter Control

**Status**: Detailed plan documented, ready to implement when approved
**Updated**: October 2, 2025

### Overview: Custom VIA Alternative

Instead of full VIA (3,000-3,500 bytes), build a lightweight custom solution using Raw HID protocol that provides exactly what we need with minimal firmware overhead.

**Space Cost**: ~500-700 bytes (vs VIA's 3,000+ bytes)
**Savings**: ~2,300-2,800 bytes

### Phase 1: Acceleration Curve Editor (PRIMARY GOAL)

#### Web Application Features

**Main Feature: Bezier Curve Editor** 🎨
- Interactive HTML5 canvas with draggable control points
- Visual representation of current acceleration curve
- Real-time preview of input → output mapping
- "Test flick" visualization showing cursor movement
- Before/After comparison slider

**Control Interface**:
```
Pointer Mode:
├─ X-axis Bezier curve (2 control points)
├─ Y-axis Bezier curve (2 control points)
└─ Base sensitivity slider

Caret Mode:
├─ X-axis Bezier curve
├─ Y-axis Bezier curve
├─ X snap threshold (1.0 - 3.0)
└─ Y snap threshold (1.0 - 3.0)

Mouse Settings:
└─ Timeout slider (250 - 2000ms)
```

**How Bezier → Firmware Works**:
1. User drags Bezier control points in web UI
2. JavaScript samples the curve at key points
3. Calculates quadratic approximation: `ax² + bx + c`
4. Sends coefficients to keyboard via WebHID
5. Firmware stores in EEPROM (persistent)
6. Immediate effect without reflashing

#### Firmware Implementation

**Raw HID Protocol** (~500-700 bytes):
```c
// Raw HID message structure
#define RAW_EPSIZE 32
#define CMD_SET_ACCEL 0x01
#define CMD_GET_ACCEL 0x02
#define CMD_SET_TIMEOUT 0x03
#define CMD_SAVE_EEPROM 0x04

// EEPROM storage structure
typedef struct {
    float pointer_x_quad;      // x² coefficient
    float pointer_x_linear;    // x coefficient
    float pointer_y_quad;
    float pointer_y_linear;
    float caret_x_quad;
    float caret_x_linear;
    float caret_y_quad;
    float caret_y_linear;
    float caret_x_threshold;
    float caret_y_threshold;
    uint16_t mouse_timeout;
    float base_sens;
    uint32_t magic;  // Validation
} accel_config_t;

// Load on startup, save on change
void load_accel_config(void);
void save_accel_config(void);
```

**Web App Features**:
- Single HTML file (no installation)
- Works in Chrome/Edge (WebHID support)
- Cross-platform (Mac, Windows, Linux)
- Save/Load profiles in browser localStorage
- Export settings to JSON file
- Import settings from JSON

**Implementation Effort**: 13-18 hours
- Raw HID protocol: 4-6 hours
- Web UI framework: 3-4 hours
- Bezier curve editor: 6-8 hours

### Phase 2: On-Screen Keymap Display (FUTURE)

**Status**: Documented for future implementation
**Priority**: Secondary (nice-to-have)

#### Approach A: Web-Based Overlay (Recommended Start)

**How It Works**:
1. Browser window runs in kiosk/always-on-top mode
2. Semi-transparent overlay shows keymap
3. Raw HID sends layer change events from keyboard
4. JavaScript updates SVG keymap visualization
5. Hides when layer key released

**Implementation**:
```javascript
// Raw HID listener
device.addEventListener('inputreport', event => {
    const layerId = event.data.getUint8(0);
    updateKeymapDisplay(layerId);
});

// SVG keymap renderer
function updateKeymapDisplay(layer) {
    // Load layer keymap from embedded data
    // Highlight active keys
    // Position overlay near cursor or fixed screen position
}
```

**Firmware Cost**: ~100-200 bytes (just layer event notifications)

**Pros**:
- Cross-platform (works anywhere Chrome runs)
- Easy to build (6-8 hours)
- Uses same tech stack as parameter editor
- Can combine with parameter app

**Cons**:
- Browser window must stay open
- Not a true "tray app"
- Can't be fully transparent system overlay
- Focus management issues

**Development Effort**: 6-8 hours

#### Approach B: Native Tray App (Future Enhancement)

**How It Works**:
1. Native application (Swift/macOS, C#/Windows)
2. System tray icon
3. Listens to Raw HID events
4. Draws true transparent overlay using OS APIs
5. Shows/hides on modifier hold

**Platform Implementations**:

**macOS (Swift)**:
```swift
// NSWindow with transparent background
// Accessibility permissions required
// Uses NSScreen for positioning
```

**Windows (C#/WPF)**:
```csharp
// LayeredWindow with WS_EX_TRANSPARENT
// SetWindowsPos with HWND_TOPMOST
// P/Invoke for raw HID
```

**Firmware Cost**: ~100-200 bytes (same layer notifications)

**Pros**:
- True system overlay experience
- Proper tray icon integration
- Can be fully transparent
- Professional native feel

**Cons**:
- Platform-specific code (need 2 apps)
- Distribution complexity (code signing, installers)
- **Much more work**: 20-30 hours per platform
- Requires separate apps for Mac/Windows

**Development Effort**:
- macOS: 20-30 hours
- Windows: 20-30 hours
- Linux: Not planned

**Recommendation**: Start with Approach A (web overlay), upgrade to native app only if needed.

### Required Optimizations for GUI Implementation

To make space for Raw HID protocol (~500-700 bytes), need to free ~500-800 bytes:

**Optimization Plan** (NOT YET IMPLEMENTED):

1. **Remove Transparent Layers** (~400-600 bytes):
   - Layer 5 (Caret): Fully transparent, just mode flag
   - Layer 6 (Volume): Fully transparent, just mode flag
   - Consolidate into Layer 1 or direct mode activation

2. **Optimize Mouse Timeout Code** (~100-150 bytes):
   - Simplify logic without changing functionality
   - Combine duplicate checks

3. **Convert to Integer Math** (~100-200 bytes) [OPTIONAL]:
   - Replace `float` with fixed-point integers
   - Keep same feel, just more efficient
   - Note: Loses K+O/K+U runtime adjustment (but GUI replaces it)

**Total Savings**: ~600-950 bytes
**After Adding GUI**: ~100-400 bytes free buffer

### Dependencies & Constraints

**Browser Requirements**:
- Chrome 89+ or Edge 89+ (WebHID API)
- Firefox: Not supported (no WebHID)
- Safari: Not supported (no WebHID)

**Firmware Requirements**:
- Raw HID support (built into QMK)
- EEPROM storage available (have ~1KB unused)
- Alabastard fork compatible (yes, HID is standard QMK)

**Development Tools Needed**:
- HTML/CSS/JavaScript (no build tools needed)
- Chrome DevTools for debugging
- QMK build environment (already have)

---

## 🔧 FUTURE: VIA-Style Keymap Editor

**Status**: Feasibility analysis for future consideration
**Updated**: October 2, 2025

### Adding Keymap/Layer Editing to Web GUI

**Question**: How hard to add VIA-style keymap editing to the custom web app?

**Answer**: Moderately complex but feasible in ~20-30 additional hours

### Implementation Approach

#### Firmware Side (~800-1,200 bytes)

**Required QMK Features** (already available):
```c
// Dynamic keymap support (subset of VIA)
#define DYNAMIC_KEYMAP_LAYER_COUNT 4    // We have 7 layers
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 1024

// Raw HID commands for keymap
#define CMD_GET_KEYMAP 0x10
#define CMD_SET_KEY 0x11
#define CMD_GET_LAYER_COUNT 0x12
```

**EEPROM Layout**:
```
[0-50]:     Acceleration config
[51-1024]:  Dynamic keymap storage
```

#### Web App Side (~20-30 hours)

**UI Components Needed**:

1. **Keymap Visualizer** (~8-10 hours):
   - SVG keyboard layout (4x6 split)
   - Layer tabs (0-6)
   - Key labels showing current mapping
   - Click key to edit

2. **Key Picker Dialog** (~6-8 hours):
   - Searchable keycode list
   - Categories: Basic, Modifiers, Layers, Media, etc.
   - QMK keycode → human-readable labels
   - Support for complex keycodes (LT, MT, etc.)

3. **Layer Management** (~3-4 hours):
   - Add/remove layers (up to firmware limit)
   - Duplicate layer
   - Clear layer
   - Import/Export layer JSON

4. **Firmware Integration** (~3-4 hours):
   - WebHID protocol for keymap read/write
   - EEPROM persistence
   - Validation (keycode exists, layer valid)
   - Conflict detection

**Total Web Development**: ~20-30 hours

### Space Analysis

**Firmware Cost**:
- Dynamic keymap storage: ~800 bytes
- EEPROM management: ~200 bytes
- Raw HID handlers: ~200 bytes
- **Total: ~1,200 bytes**

**Combined with Parameter GUI**:
- Parameter control: ~500-700 bytes
- Keymap editing: ~1,200 bytes
- **Total: ~1,700-1,900 bytes**

**After Optimizations** (save ~600-950 bytes):
- Current free: 1,250 bytes
- After optimization: +600-950 = 1,850-2,200 bytes free
- After GUI: -1,900 = **~0-300 bytes free (TIGHT)**

### Feasibility: TIGHT BUT POSSIBLE

**To fit both Parameter GUI + Keymap Editor**:

May need additional ~0-300 bytes depending on optimization results:

**Options if needed**:
1. Reduce layer count from 7 to 4 (~300 bytes)
2. Remove unused Layer 1 keys (~100 bytes)
3. Simplify axis snapping (~50 bytes per mode)
4. Use LTO_ENABLE more aggressively (~100-200 bytes)

**Recommendation**:
1. ✅ Start with Parameter GUI only (~500-700 bytes) - achievable now
2. ✅ Validate usefulness through real use
3. ⏳ If keymap editing is truly needed, optimize further
4. ⏳ Add keymap editor as Phase 2 (~1,200 more bytes)

### Comparison to Full VIA

**Full VIA**:
- Size: 3,000-3,500 bytes
- Features: Keymap, macros, lighting, encoders
- We use: Keymap + parameters only
- Overhead: ~1,500-2,000 bytes wasted

**Custom Implementation**:
- Size: 1,700-1,900 bytes (parameter + keymap)
- Features: Exactly what we need (Bezier curves, keymap)
- No overhead
- **Savings: ~1,100-1,800 bytes**

### Development Priority

**Phase 1** (Highest Value):
- Acceleration curve Bezier editor ⭐⭐⭐⭐⭐
- Parameter sliders (thresholds, timeout) ⭐⭐⭐⭐
- EEPROM persistence ⭐⭐⭐⭐

**Phase 2** (Medium Value):
- Keymap visualization (read-only) ⭐⭐⭐
- Key labels and layout display ⭐⭐⭐

**Phase 3** (Nice to Have):
- Full keymap editing ⭐⭐
- Layer management ⭐⭐
- Import/Export ⭐⭐

**Phase 4** (Future):
- On-screen keymap overlay (web or native) ⭐

---

## Status Log

- **[2025-10-01 17:00]** ✅ Acceleration curve tuned to user satisfaction
  - Replaced exponential with quadratic: `x² × 1.6 + x × 0.6 × base_sens`
  - Added runtime adjustment via Layer 4 + U/I keys
  - Fixed caret mode for symmetric sensitivity
  - Firmware: 28,660/28,672 bytes (12 bytes free)
  - Status: Ready for real-world validation

- **[2025-10-01 14:00]** ✅ Confirmed 2023 fork is permanent requirement
  - Extensive testing of modern QMK: SROM incompatibility confirmed
  - Created PORTING_ANALYSIS.md with full technical details
  - Baseline firmware working: 28,486 bytes

- **[2025-09-29 19:30]** ⚠️ Discovered iCloud sync blocking compilation
  - Moved to `/Users/darrenschaeffer/Desktop/qmk_2023/` outside iCloud
  - Added debug monitoring code (console output)

- **[2025-09-29 18:49]** 🚀 Started acceleration curve optimization project

---

**Project maintained by user with assistance from Claude Code**
**QMK Firmware Fork**: Alabastard's `feature_pointing_device_scroll_framework` branch
