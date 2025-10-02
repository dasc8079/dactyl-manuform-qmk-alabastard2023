 #include QMK_KEYBOARD_H

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

//Scroll, Caret,  Volume, Mouse Acceleration

//pointing_device_set_rotational_transform_angle(30);
int16_t x;
int16_t y;

static bool scroll_mode = false;
static bool caret_mode = false;
static bool volume_mode = false;
static bool default_mode = true;

// Runtime adjustable base sensitivity (linear multiplier)
static float base_sens = 0.5;


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
        case _LAYER5:  // If we're on the _LAYER5 layer enable Carert mode
            set_scroll_mode(SM_CARET);
            pointing_device_set_cpi(300);
            caret_mode = true;
            break;
        case _LAYER6:  // If we're on the _LAYER6 layer enable Volume mode
            set_scroll_mode(SM_VOLUME);
            pointing_device_set_cpi(200);
            volume_mode = true;
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
        // Symmetric sensitivity for both axes
        x = mouse_report.x * base_sens;
        y = mouse_report.y * base_sens;
    }

    else if (volume_mode) {
        x = mouse_report.x;
        y = 2 * mouse_report.y;
    }

    else if (scroll_mode) {
        // Tuned quadratic: gentler low end, aggressive high end
        int16_t abs_x = abs(mouse_report.x);
        int16_t abs_y = abs(mouse_report.y);
        x = (mouse_xy_report_t)(mouse_report.x > 0 ? (abs_x * abs_x * 1.6 + abs_x * base_sens * 0.6) : -(abs_x * abs_x * 1.6 + abs_x * base_sens * 0.6));
        y = (mouse_xy_report_t)(mouse_report.y > 0 ? (abs_y * abs_y * 2.0 + abs_y * base_sens * 0.6) : -(abs_y * abs_y * 2.0 + abs_y * base_sens * 0.6));
    }

    else if (default_mode) {
        // Tuned quadratic: gentler low end, aggressive high end
        int16_t abs_x = abs(mouse_report.x);
        int16_t abs_y = abs(mouse_report.y);
        x = (mouse_xy_report_t)(mouse_report.x > 0 ? (abs_x * abs_x * 1.6 + abs_x * base_sens * 0.6) : -(abs_x * abs_x * 1.6 + abs_x * base_sens * 0.6));
        y = (mouse_xy_report_t)(mouse_report.y > 0 ? (abs_y * abs_y * 2.0 + abs_y * base_sens * 0.6) : -(abs_y * abs_y * 2.0 + abs_y * base_sens * 0.6));
    }

    // Console debug output
    if (raw_x != 0 || raw_y != 0) {
        uprintf("RAW: x=%d y=%d | OUT: x=%d y=%d\n", raw_x, raw_y, x, y);
    }

    mouse_report.x = x;
    mouse_report.y = y;

    return mouse_report;

}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ACCEL_UP:
            if (record->event.pressed) {
                base_sens += 0.1;
                if (base_sens > 3.0) base_sens = 3.0;
                uprintf("Base sensitivity: %.1f\n", base_sens);
            }
            return false;
        case ACCEL_DN:
            if (record->event.pressed) {
                base_sens -= 0.1;
                if (base_sens < 0.1) base_sens = 0.1;
                uprintf("Base sensitivity: %.1f\n", base_sens);
            }
            return false;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_LAYER0] = LAYOUT( // Default layout, mouse button 1 instead of "j"
        QK_GESC,    KC_Q,       KC_W,    KC_E,   		KC_R,    			 KC_T,                                                  KC_Y,                   KC_U,    				KC_I,	    			KC_O,               KC_P,   	 KC_MINS,
        KC_TAB,     KC_A,       KC_S,    KC_D,          KC_F,                KC_G,                                                  KC_H,                   KC_BTN1, 				LT(LAYER4, KC_K),	 	LT(LAYER1, KC_L),   SFT_T(KC_SCLN),     KC_QUOT,
        KC_DEL,     KC_Z,       KC_X,    KC_C,    		KC_V,    			 KC_B,              			                        LT(LAYER5, KC_N),       LT(LAYER5, KC_M),   	LT(LAYER6, KC_COMM), 	            KC_DOT,  		    KC_SLSH, 	 KC_BSLS,
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

LALT(KC_F4),	KC_WSCH, 	KC_INSERT, 			KC_UP,   	LCTL(KC_R), LALT(KC_T), 	/*KC_NO*/                                   LCTL_T(KC_Y), 	ACCEL_DN,	ACCEL_UP, 	 	KC_EJCT, 	KC_MPRV,    KC_NO, 	 	//KC_TRNS,
LCTL(KC_W), 	LCTL(KC_S), KC_LEFT, 			KC_DOWN, 	KC_RIGHT, 	LALT(KC_F), 	/*KC_NO*/                                   KC_BTN3, 		KC_J, 		KC_TRNS, 		KC_BTN2, 	KC_MPLY,    KC_MSTP,    //KC_TRNS,                           
RCS(KC_Z), 		KC_UNDO, 	KC_CUT, 			KC_COPY, 	KC_PSTE, 	RCS(KC_T), 		/*KC_NO*/                                   KC_MYCM, 		KC_TRNS, 	KC_TRNS, 		KC_MUTE, 	KC_MNXT,    KC_NO, 	 	//KC_TRNS,KC_MSTP, 	KC_MPRV,
							LCTL(KC_T), 		LCTL(KC_W), 						                                                                            KC_SLEP,		QK_BOOT, 		 
KC_TRNS,     KC_TRNS, 						                                                                                        KC_NO, 	        KC_TRNS,
KC_TRNS,     KC_TRNS, 							                                                                                    KC_TRNS, 		KC_RSFT, 
KC_TRNS,     KC_TRNS,     						                                                                                    KC_RGUI, 	    KC_NO			

),

    [_LAYER5] = LAYOUT(

KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/                                   KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	//KC_TRNS,
KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/                                   KC_TRNS, 		KC_TRNS, 	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS,	//KC_TRNS,                           
KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/                               	KC_TRNS, 		KC_TRNS, 	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	//KC_TRNS,
							KC_TRNS, 				KC_TRNS, 						                                                                            KC_TRNS, 				KC_TRNS, 
KC_TRNS, 		KC_TRNS,					                                                                          	        	KC_NO, 	       		KC_TRNS,
KC_TRNS, 		KC_TRNS,						                                                                          	      	KC_TRNS, 			KC_TRNS, 
KC_TRNS, 		KC_TRNS,     						                                                                              	KC_TRNS, 	   		KC_NO			

),

    [_LAYER6] = LAYOUT(

KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/                                   KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	//KC_TRNS,
KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/                                   KC_TRNS, 		KC_TRNS, 	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS,	//KC_TRNS,                           
KC_TRNS, 		KC_TRNS,	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	/*KC_NO*/                               	KC_TRNS, 		KC_TRNS, 	KC_TRNS, 				KC_TRNS, 	KC_TRNS, 	KC_TRNS, 	//KC_TRNS,
							KC_TRNS, 				KC_TRNS, 						                                                                            KC_TRNS, 				KC_TRNS, 
KC_TRNS, 		KC_TRNS,					                                                                          	        	KC_NO, 	       		KC_TRNS,
KC_TRNS, 		KC_TRNS,						                                                                          	      	KC_TRNS, 			KC_TRNS, 
KC_TRNS, 		KC_TRNS,     						                                                                              	KC_TRNS, 	   		KC_NO			

)

};

 