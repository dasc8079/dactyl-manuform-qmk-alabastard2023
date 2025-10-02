/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

/* Use I2C or Serial, not both */
//#define USE_SERIAL
#define USE_I2C

/* Select hand configuration */
//#define MASTER_LEFT
#define MASTER_RIGHT

//#define EE_HANDS

#undef ROTATIONAL_TRANSFORM_ANGLE
#define ROTATIONAL_TRANSFORM_ANGLE -30

// Tap-hold timing configuration for improved Space/Enter behavior
#define TAPPING_TERM 200                    // Global default (ms)
#define PERMISSIVE_HOLD                     // Better for fast typing
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY     // Per-key hold behavior
#define QUICK_TAP_TERM 120                  // Prevent accidental holds on rapid taps
#define TAPPING_TERM_PER_KEY                // Enable per-key timing customization
