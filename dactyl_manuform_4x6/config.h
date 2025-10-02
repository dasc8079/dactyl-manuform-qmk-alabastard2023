#pragma once

#include "config_common.h"

/* key matrix size */
// Rows are doubled-up
#define MATRIX_ROWS 10
#define MATRIX_COLS 6

// row-driven
#define MATRIX_ROW_PINS { D5, C7, F1, F0, B6 }
#define MATRIX_COL_PINS { D4, C6, D7, E6, B4, B5 }

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION ROW2COL

#define DEBOUNCE 5
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
#define TAPPING_TERM 200
#define POLLING true

//pointing device
#define PMW33XX_CS_PIN F7
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT
#define POINTING_DEVICE_INVERT_X
#define PMW33XX_CPI 100 // Incriments of 100

#define MOUSE_EXTENDED_REPORT // Makes the acceleration more accurate 
#define PMW33XX_LIFTOFF_DISTANCE 0x15
#define POINTING_DEVICE_SCROLL_ENABLE
#define POINTING_DEVICE_INVERT_V

//#undef ROTATIONAL_TRANSFORM_ANGLE
//#define ROTATIONAL_TRANSFORM_ANGLE -30
//#define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE