#include QMK_KEYBOARD_H
#include "pointing_device_scroll.h"
#include "raw_hid.h"

#define _LAYER0 0
#define _LAYER1 1
#define _LAYER2 2
#define _LAYER3 3
#define _LAYER4 4
#define _LAYER5 5
#define _LAYER6 6

#define LAYER1 MO(_LAYER1)
#define LAYER2 MO(_LAYER2)
#define LAYER3 MO(_LAYER3)
#define LAYER4 MO(_LAYER4)
#define LAYER5 MO(_LAYER5)
#define LAYER6 MO(_LAYER6)

// Custom keycodes for acceleration tuning
enum custom_keycodes {
    ACCEL_UP = SAFE_RANGE,
    ACCEL_DN
};

// Raw HID Protocol
#define HID_CMD_GET_CONFIG      0x01
#define HID_CMD_SET_POINTER_X   0x02
#define HID_CMD_SET_POINTER_Y   0x03
#define HID_CMD_SET_CARET_X     0x04
#define HID_CMD_SET_CARET_Y     0x05
#define HID_CMD_SET_TIMEOUT     0x06
#define HID_CMD_SET_BASE_SENS   0x07
#define HID_CMD_SAVE_EEPROM     0x08
#define HID_CMD_RESET_DEFAULTS  0x09
#define HID_CMD_SET_SCROLL_X    0x0A
#define HID_CMD_SET_SCROLL_Y    0x0B
#define HID_CMD_SET_AXIS_SNAP   0x0C

// EEPROM config structure (22 bytes)
typedef struct {
    uint8_t  magic;           // 0xAC = valid config
    int16_t  pointer_accel_x; // Quadratic coefficient for pointer X (8.8 fixed-point)
    int16_t  pointer_accel_y; // Quadratic coefficient for pointer Y
    int16_t  scroll_accel_x;  // Quadratic coefficient for scroll X
    int16_t  scroll_accel_y;  // Quadratic coefficient for scroll Y
    int16_t  caret_accel_x;   // Quadratic coefficient for caret X
    int16_t  caret_accel_y;   // Quadratic coefficient for caret Y
    uint16_t mouse_timeout;   // Timeout in ms
    int16_t  base_sensitivity;// Base sensitivity (8.8 fixed-point)
    int16_t  axis_snap_thresh;// Axis snapping threshold (8.8 fixed-point)
    uint8_t  reserved[1];     // Reserved for future use
} eeprom_config_t;

#define EEPROM_MAGIC 0xAC
#define EEPROM_CONFIG_ADDR ((eeprom_config_t*)0)  // Store at EEPROM address 0

//Scroll, Caret,  Volume, Mouse Acceleration

//pointing_device_set_rotational_transform_angle(30);
int16_t x;
int16_t y;

static bool scroll_mode = false;
static bool caret_mode = false;
static bool volume_mode = false;
static bool default_mode = true;

// Fixed-point math (8.8 format: 8 bits integer, 8 bits fractional)
#define FP_SCALE 256
#define FLOAT_TO_FP(x) ((int16_t)((x) * FP_SCALE))
#define FP_MUL(a, b) (((int32_t)(a) * (b)) >> 8)

// Runtime adjustable acceleration coefficients (8.8 fixed-point, can be updated via HID)
static int16_t pointer_accel_x_quad = FLOAT_TO_FP(5.0);    // Default: 5.0
static int16_t pointer_accel_y_quad = FLOAT_TO_FP(4.8);    // Default: 4.8
static int16_t scroll_accel_x_quad = FLOAT_TO_FP(5.0);     // Default: 5.0 (separate from pointer)
static int16_t scroll_accel_y_quad = FLOAT_TO_FP(4.8);     // Default: 4.8 (separate from pointer)
static int16_t caret_accel_x_quad = FLOAT_TO_FP(0.25);     // Default: 0.25
static int16_t caret_accel_y_quad = FLOAT_TO_FP(0.25);     // Default: 0.25
static int16_t base_sens_fp = FLOAT_TO_FP(0.1);            // Default: 0.1
static int16_t axis_snap_threshold = FLOAT_TO_FP(1.3);     // Default: 1.3 (axis snapping sensitivity)

// Mouse button timeout feature (can be updated via HID)
static uint32_t last_trackball_activity = 0;
static bool mouse_button_mode = false;
static uint16_t mouse_timeout_ms = 750;  // Default: 750ms


layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _LAYER4:  // If we're on the _LAYER4 layer enable Scroll mode
            set_scroll_mode(SM_DRAG);
            pointing_device_set_cpi(100);
            scroll_mode = true;
            caret_mode = false;
            volume_mode = false;
            default_mode = false;
            break;
        case _LAYER5:  // If we're on the _LAYER5 layer enable Caret mode
            set_scroll_mode(SM_CARET);
            pointing_device_set_cpi(300);
            caret_mode = true;
            scroll_mode = false;
            volume_mode = false;
            default_mode = false;
            break;
        case _LAYER6:  // If we're on the _LAYER6 layer enable Volume mode
            set_scroll_mode(SM_VOLUME);
            pointing_device_set_cpi(200);
            volume_mode = true;
            scroll_mode = false;
            caret_mode = false;
            default_mode = false;
            break;
        default:
            set_scroll_mode(SM_NONE);    
            pointing_device_set_cpi(100);
            default_mode = true;
            caret_mode = false;
            volume_mode = false;
            scroll_mode = false;
            break;
    }
    return state;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    int16_t raw_x = mouse_report.x;
    int16_t raw_y = mouse_report.y;

    if (caret_mode) {
        // Symmetric axis snapping - uses runtime adjustable threshold
        int16_t abs_x = abs(mouse_report.x);
        int16_t abs_y = abs(mouse_report.y);
        // axis_snap_threshold is in 8.8 format, so multiply abs_y by threshold and divide by FP_SCALE
        if (FP_MUL(abs_y, axis_snap_threshold) < abs_x) {
            mouse_report.y = 0;
        } else if (FP_MUL(abs_x, axis_snap_threshold) < abs_y) {
            mouse_report.x = 0;
        }
        // X with reduced acceleration, Y with higher sensitivity (uses runtime adjustable coefficients)
        int16_t abs_x_mult = abs(mouse_report.x);
        int16_t abs_y_mult = abs(mouse_report.y);
        int16_t accel_x = FP_MUL(abs_x_mult * abs_x_mult, caret_accel_x_quad) + FP_MUL(abs_x_mult, FLOAT_TO_FP(1.3));
        int16_t accel_y = FP_MUL(abs_y_mult * abs_y_mult, caret_accel_y_quad) + FP_MUL(abs_y_mult, FLOAT_TO_FP(1.0));
        x = (mouse_xy_report_t)(mouse_report.x > 0 ? accel_x : -accel_x);
        y = (mouse_xy_report_t)(mouse_report.y > 0 ? accel_y : -accel_y);
    }

    else if (volume_mode) {
        x = mouse_report.x;
        y = 2 * mouse_report.y;
    }

    else if (scroll_mode || default_mode) {
        // Scroll mode: axis snapping (favor stronger axis)
        if (scroll_mode) {
            if (abs(mouse_report.x) > abs(mouse_report.y)) {
                mouse_report.y = 0;
            } else {
                mouse_report.x = 0;
            }
        }
        // Quadratic acceleration with mode-specific curves
        int16_t abs_x = abs(mouse_report.x);
        int16_t abs_y = abs(mouse_report.y);

        // Use scroll-specific curves in scroll mode, pointer curves in default mode
        int16_t x_quad_coeff = scroll_mode ? scroll_accel_x_quad : pointer_accel_x_quad;
        int16_t y_quad_coeff = scroll_mode ? scroll_accel_y_quad : pointer_accel_y_quad;

        int16_t accel_x = FP_MUL(abs_x * abs_x, x_quad_coeff) + FP_MUL(abs_x, FP_MUL(base_sens_fp, FLOAT_TO_FP(0.6)));
        int16_t accel_y = FP_MUL(abs_y * abs_y, y_quad_coeff) + FP_MUL(abs_y, FP_MUL(base_sens_fp, FLOAT_TO_FP(0.6)));
        x = (mouse_xy_report_t)(mouse_report.x > 0 ? accel_x : -accel_x);
        y = (mouse_xy_report_t)(mouse_report.y > 0 ? accel_y : -accel_y);
    }

    mouse_report.x = x;
    mouse_report.y = y;

    // Mouse button timeout: activate mouse mode on trackball movement
    if (raw_x != 0 || raw_y != 0) {
        last_trackball_activity = timer_read32();
        mouse_button_mode = true;
    }

    // Check timeout and deactivate mouse mode if idle (uses runtime adjustable timeout)
    if (mouse_button_mode && timer_elapsed32(last_trackball_activity) > mouse_timeout_ms) {
        mouse_button_mode = false;
    }

    return mouse_report;

}

// Per-key tapping term configuration for improved tap-hold behavior
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(LAYER2, KC_SPC):     // Left thumb (Space)
        case LT(LAYER3, KC_ENT):     // Right thumb (Enter)
            return 230;              // Longer for thumbs (less precise than fingers)
        case MT(MOD_RGUI, KC_BTN5):  // Thumb mouse button with Cmd modifier
            return 200;              // Standard timing for mouse button
        default:
            return TAPPING_TERM;     // Use global default (200ms)
    }
}

// Per-key hold-on-other-key-press behavior
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(LAYER2, KC_SPC):     // Space - don't activate layer if another key pressed
        case LT(LAYER3, KC_ENT):     // Enter - don't activate layer if another key pressed
            return false;            // Requires key to be held without other keys
        default:
            return true;             // Default behavior for other keys
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Mouse button timeout: intercept J/L/H when in mouse mode
    if (mouse_button_mode) {
        uint16_t btn_code = 0;
        switch (keycode) {
            case KC_J: btn_code = KC_BTN1; break;  // Left click
            case KC_L: btn_code = KC_BTN2; break;  // Right click
            case KC_H: btn_code = KC_BTN3; break;  // Middle click
        }

        if (btn_code) {
            if (record->event.pressed) {
                register_code(btn_code);
                last_trackball_activity = timer_read32();
            } else {
                unregister_code(btn_code);
            }
            return false;
        } else if (keycode != KC_LGUI && keycode != LT(LAYER2, KC_SPC) &&
                   keycode != LT(LAYER3, KC_ENT) && keycode != KC_LCTL &&
                   keycode != KC_LALT && keycode != MT(MOD_RGUI, KC_BTN5) &&
                   keycode != MT(MOD_RSFT, KC_BTN4) && keycode != KC_BSPC &&
                   keycode != LT(LAYER4, KC_HOME) && keycode != LT(LAYER3, KC_END) &&
                   keycode != LT(LAYER4, KC_K)) {
            mouse_button_mode = false;
        }
    }

    switch (keycode) {
        case ACCEL_UP:
            if (record->event.pressed) {
                base_sens_fp += FLOAT_TO_FP(0.1);  // +26 in fixed-point
                if (base_sens_fp > FLOAT_TO_FP(3.0)) base_sens_fp = FLOAT_TO_FP(3.0);  // Max 768
            }
            return false;
        case ACCEL_DN:
            if (record->event.pressed) {
                base_sens_fp -= FLOAT_TO_FP(0.1);  // -26 in fixed-point
                if (base_sens_fp < FLOAT_TO_FP(0.1)) base_sens_fp = FLOAT_TO_FP(0.1);  // Min 26
            }
            return false;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_LAYER0] = LAYOUT( // Default layout, mouse button 1 instead of "j"
        QK_GESC,    KC_Q,       KC_W,    KC_E,   		KC_R,    			 KC_T,                                                  KC_Y,                   KC_U,    				KC_I,	    			KC_O,               KC_P,   	 KC_MINS,
        KC_TAB,     KC_A,       KC_S,    KC_D,          KC_F,                KC_G,                                                  KC_H,                   KC_J, 				LT(LAYER4, KC_K),	 	KC_L,   SFT_T(KC_SCLN),     KC_QUOT,
        KC_DEL,     KC_Z,       KC_X,    KC_C,    		KC_V,    			 KC_B,              			                        KC_N,                   LT(LAYER5, KC_M),   	LT(LAYER6, KC_COMM), 	            KC_DOT,  		    KC_SLSH, 	 KC_BSLS,
                                KC_LPRN, KC_RPRN,                                                			   				                                                        KC_EXLM,	            KC_UNDS,
                                                        KC_LGUI, 	    LT(LAYER2, KC_SPC),	        	 		                    KC_NO,                  LT(LAYER3, KC_ENT),      
                                                        KC_LCTL,             KC_LALT,             			 		                MT(MOD_RGUI, KC_BTN5),  MT(MOD_RSFT, KC_BTN4),
                                                        LT(LAYER4, KC_HOME), LT(LAYER3, KC_END),   			 	                    KC_BSPC, 			    KC_NO
    ),

 [_LAYER1] = LAYOUT(

KC_TRNS, 	KC_TRNS, 	KC_TRNS, 			KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/	                                        KC_TRNS, 	KC_TRNS, 	KC_TRNS, 			KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	//KC_TRNS,
KC_TRNS, 	KC_TRNS, 	KC_TRNS, 			KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/	                                        KC_BTN3, 	S(KC_BTN1), KC_BTN2, 			KC_TRNS, 	KC_TRNS, 	KC_TRNS,	//KC_TRNS,
KC_TRNS, 	KC_TRNS, 	KC_TRNS, 			KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/	                                        KC_TRNS, 	KC_TRNS, 	KC_TRNS, 			KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	//KC_TRNS,
						KC_LBRC,			KC_RBRC, 						                                                                                KC_TRNS, 			KC_TRNS, 
S(KC_TRNS), KC_TRNS,						                                                                                        KC_NO, 	    KC_ENT, 
S(KC_RCTL), S(KC_RALT), 						                                                                                    KC_TRNS, 	KC_TRNS,
KC_TRNS, 	KC_TRNS,                                                                                                                KC_TRNS,    KC_NO
		
),
 
    [_LAYER2] = LAYOUT( // Capital letters with "J" and caps

KC_CAPS, 		S(KC_Q), 		S(KC_W), 				S(KC_E), 		S(KC_R),		S(KC_T), 		/*S(KC_NO)*/               S(KC_Y), 		S(KC_U), 		S(KC_I),			S(KC_O), 		S(KC_P), 		S(KC_MINS), 	//S(KC_MPLY), 
S(KC_TAB),  	S(KC_A),		S(KC_S), 				S(KC_D), 		S(KC_F), 		S(KC_G),		/*S(KC_NO)*/               S(KC_H), 		S(KC_J), 		S(KC_K),			S(KC_L), 		S(KC_SCLN), 	S(KC_QUOT),		//S(KC_MNXT), 
S(KC_LGUI), 	S(KC_Z), 		S(KC_X), 				S(KC_C), 		S(KC_V), 		S(KC_B), 		/*S(KC_NO)*/               S(KC_N), 		S(KC_M), 		S(KC_COMM), 		S(KC_DOT), 		S(KC_SLSH),		S(KC_BSLS), 	//S(KC_MPRV),          
								S(KC_LBRC), 		 	S(KC_RBRC),						                                                                            S(KC_EXLM),     	S(KC_EQL), 
S(KC_TRNS), 	KC_TRNS, 	        					                                                                           KC_NO, 	        S(KC_TRNS),
S(KC_TRNS), 	S(KC_TRNS), 						                                                                               S(KC_TRNS), 	    KC_TRNS,
KC_TRNS, 	    KC_TRNS, 						                                                                                   S(KC_TRNS),      KC_NO

),

    [_LAYER3] = LAYOUT(

KC_AT, 		KC_HASH, 	KC_DLR, 			KC_PERC, 	KC_AMPR, 	KC_LBRC,	/*KC_NO*/	                                        KC_RBRC, 	KC_P7, 		KC_P8, 				KC_P9, 		KC_PERC, 	KC_CIRC,	//KC_TRNS,
KC_F1, 		KC_F2, 		KC_F3, 				KC_F4, 		KC_F5, 		KC_F12, 	/*KC_NO*/		                                    KC_EQL,  	KC_P4, 		KC_P5, 				KC_P6, 		KC_PPLS,    KC_ASTR,	//KC_TRNS,
KC_F6, 		KC_F7, 		KC_F8, 				KC_F9, 		KC_F10, 	KC_F11, 	/*KC_NO*/		                                    KC_TILD, 	KC_P1, 		KC_P2, 				KC_P3, 		KC_PMNS, 	KC_PSLS, 	//KC_TRNS,
						KC_F12, 			KC_PSCR, 						                                                                                KC_P0, 				KC_PDOT,
KC_TRNS, 	KC_TRNS, 		                                                                                                        KC_NO, 	    KC_TRNS,				 
KC_TRNS, 	KC_TRNS, 					                                                                                            KC_TRNS, 	KC_TRNS,	
KC_TRNS, 	KC_TRNS, 					                                                                                            KC_TRNS, 	KC_NO	

),  

    [_LAYER4] = LAYOUT(

LALT(KC_F4),	KC_WSCH, 	KC_INSERT, 			KC_UP,   	LCTL(KC_R), LALT(KC_T), 	/*KC_NO*/                                   LCTL_T(KC_Y), 	ACCEL_DN,	KC_SLEP, 	 	ACCEL_UP, 	KC_MPRV,    KC_NO, 	 	//KC_TRNS,
LCTL(KC_W), 	LCTL(KC_S), KC_LEFT, 			KC_DOWN, 	KC_RIGHT, 	LALT(KC_F), 	/*KC_NO*/                                   KC_BTN3, 		KC_J, 		KC_TRNS, 		KC_BTN2, 	KC_MPLY,    KC_MSTP,    //KC_TRNS,                           
RCS(KC_Z), 		KC_UNDO, 	KC_CUT, 			KC_COPY, 	KC_PSTE, 	RCS(KC_T), 		/*KC_NO*/                                   KC_MYCM, 		KC_TRNS, 	KC_TRNS, 		KC_MUTE, 	KC_MNXT,    KC_NO, 	 	//KC_TRNS,KC_MSTP, 	KC_MPRV,
							LCTL(KC_T), 		LCTL(KC_W), 						                                                                            KC_SYSTEM_SLEEP,		QK_BOOT, 		 
KC_TRNS,     KC_TRNS, 						                                                                                        KC_NO, 	        KC_TRNS,
KC_TRNS,     KC_TRNS, 							                                                                                    KC_TRNS, 		KC_RSFT, 
KC_TRNS,     KC_TRNS,     						                                                                                    KC_RGUI, 	    KC_NO			

),

    [_LAYER5] = LAYOUT(

KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO, 	                                   KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO,
KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO, 	                                   KC_NO, 		KC_NO, 	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO,
KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO, 	                               	KC_NO, 		KC_TRNS, 	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO,
							KC_NO, 				KC_NO, 						                                                                            KC_NO, 				KC_NO,
KC_NO, 		KC_NO,					                                                                          	        	KC_NO, 	       		KC_NO,
KC_NO, 		KC_NO,						                                                                          	      	KC_NO, 			KC_NO,
KC_NO, 		KC_NO,     						                                                                              	KC_NO, 	   		KC_NO

),

    [_LAYER6] = LAYOUT(

KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO, 	                                   KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO,
KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO, 	                                   KC_NO, 		KC_NO, 	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO,
KC_NO, 		KC_NO,	KC_NO, 				KC_NO, 	KC_NO, 	KC_NO, 	                               	KC_NO, 		KC_NO, 	KC_TRNS, 				KC_NO, 	KC_NO, 	KC_NO,
							KC_NO, 				KC_NO, 						                                                                            KC_NO, 				KC_NO,
KC_NO, 		KC_NO,					                                                                          	        	KC_NO, 	       		KC_NO,
KC_NO, 		KC_NO,						                                                                          	      	KC_NO, 			KC_NO,
KC_NO, 		KC_NO,     						                                                                              	KC_NO, 	   		KC_NO

)

};

// Load config from EEPROM on startup
void keyboard_post_init_user(void) {
    eeprom_config_t config;
    eeprom_read_block(&config, EEPROM_CONFIG_ADDR, sizeof(eeprom_config_t));

    if (config.magic == EEPROM_MAGIC) {
        // Valid config found, load it
        pointer_accel_x_quad = config.pointer_accel_x;
        pointer_accel_y_quad = config.pointer_accel_y;
        scroll_accel_x_quad = config.scroll_accel_x;
        scroll_accel_y_quad = config.scroll_accel_y;
        caret_accel_x_quad = config.caret_accel_x;
        caret_accel_y_quad = config.caret_accel_y;
        mouse_timeout_ms = config.mouse_timeout;
        base_sens_fp = config.base_sensitivity;
        axis_snap_threshold = config.axis_snap_thresh;
    }
    // If no valid config, use defaults (already initialized)
}

// Save current config to EEPROM
void save_config_to_eeprom(void) {
    eeprom_config_t config = {
        .magic = EEPROM_MAGIC,
        .pointer_accel_x = pointer_accel_x_quad,
        .pointer_accel_y = pointer_accel_y_quad,
        .scroll_accel_x = scroll_accel_x_quad,
        .scroll_accel_y = scroll_accel_y_quad,
        .caret_accel_x = caret_accel_x_quad,
        .caret_accel_y = caret_accel_y_quad,
        .mouse_timeout = mouse_timeout_ms,
        .base_sensitivity = base_sens_fp,
        .axis_snap_thresh = axis_snap_threshold,
        .reserved = {0}
    };
    eeprom_update_block(&config, EEPROM_CONFIG_ADDR, sizeof(eeprom_config_t));
}

// Reset to factory defaults
void reset_to_defaults(void) {
    pointer_accel_x_quad = FLOAT_TO_FP(5.0);
    pointer_accel_y_quad = FLOAT_TO_FP(4.8);
    scroll_accel_x_quad = FLOAT_TO_FP(5.0);
    scroll_accel_y_quad = FLOAT_TO_FP(4.8);
    caret_accel_x_quad = FLOAT_TO_FP(0.25);
    caret_accel_y_quad = FLOAT_TO_FP(0.25);
    base_sens_fp = FLOAT_TO_FP(0.1);
    mouse_timeout_ms = 750;
    axis_snap_threshold = FLOAT_TO_FP(1.3);
}

// Raw HID receive handler
void raw_hid_receive(uint8_t *data, uint8_t length) {
    uint8_t cmd = data[0];

    switch (cmd) {
        case HID_CMD_GET_CONFIG: {
            // Send current config back to host (10 parameters total)
            uint8_t response[32] = {0};
            response[0] = HID_CMD_GET_CONFIG;
            response[1] = (pointer_accel_x_quad >> 8) & 0xFF;
            response[2] = pointer_accel_x_quad & 0xFF;
            response[3] = (pointer_accel_y_quad >> 8) & 0xFF;
            response[4] = pointer_accel_y_quad & 0xFF;
            response[5] = (scroll_accel_x_quad >> 8) & 0xFF;
            response[6] = scroll_accel_x_quad & 0xFF;
            response[7] = (scroll_accel_y_quad >> 8) & 0xFF;
            response[8] = scroll_accel_y_quad & 0xFF;
            response[9] = (caret_accel_x_quad >> 8) & 0xFF;
            response[10] = caret_accel_x_quad & 0xFF;
            response[11] = (caret_accel_y_quad >> 8) & 0xFF;
            response[12] = caret_accel_y_quad & 0xFF;
            response[13] = (mouse_timeout_ms >> 8) & 0xFF;
            response[14] = mouse_timeout_ms & 0xFF;
            response[15] = (base_sens_fp >> 8) & 0xFF;
            response[16] = base_sens_fp & 0xFF;
            response[17] = (axis_snap_threshold >> 8) & 0xFF;
            response[18] = axis_snap_threshold & 0xFF;
            raw_hid_send(response, length);
            break;
        }

        case HID_CMD_SET_POINTER_X:
            pointer_accel_x_quad = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_POINTER_Y:
            pointer_accel_y_quad = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_CARET_X:
            caret_accel_x_quad = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_CARET_Y:
            caret_accel_y_quad = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_TIMEOUT:
            mouse_timeout_ms = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_BASE_SENS:
            base_sens_fp = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_SCROLL_X:
            scroll_accel_x_quad = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_SCROLL_Y:
            scroll_accel_y_quad = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SET_AXIS_SNAP:
            axis_snap_threshold = (data[1] << 8) | data[2];
            break;

        case HID_CMD_SAVE_EEPROM:
            save_config_to_eeprom();
            break;

        case HID_CMD_RESET_DEFAULTS:
            reset_to_defaults();
            break;
    }
}

