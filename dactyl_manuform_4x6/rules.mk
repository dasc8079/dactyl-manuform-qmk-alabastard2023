# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = qmk-dfu

# Build Options
#change yes to no to disable
#
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys (using trackball instead)
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
RAW_ENABLE = yes            # Enable Raw HID for WebHID GUI
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
SPLIT_KEYBOARD = yes
POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE = no
POINTING_DEVICE_SCROLL_ENABLE = yes

DEBOUNCE_TYPE = asym_eager_defer_pk
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = pmw3360
#ROTATIONAL_TRANSFORM_ANGLE = -30
#POINTING_DEVICE_ROTATION_90 = yes

# VIAL support removed for compatibility

#Reduce size of file
EXTRAFLAGS += -flto
