#ifndef HID_STATE_H
#define HID_STATE_H
//hid_state.h
#include "hid_mouse.h"
#include "hid_keyboard.h"

#define M_BUTTON_NOP   0
#define M_BUTTON_1     1
#define M_BUTTON_2     2
#define M_BUTTON_3     4
#define M_BUTTON_MAX   10
#define WHEEL M_BUTTON_MAX + 1
#define M_X            WHEEL +  1
#define M_Y            M_X + 1
#define MOUSE_TYPE     M_Y + 1
#define KEYBOARD_TYPE  MOUSE_TYPE+1
#define DELAY_TYPE     KEYBOARD_TYPE+1
#define CLICK_TYPE     DELAY_TYPE+1
#define PRESS_TYPE     CLICK_TYPE+1



//this str. contatins info about the keys
// which are pressed in mouse, keyboard .
typedef struct __hid_state
{
	keyboardHID_t keyboardHID;
	mouseHID_t    mouseHID;
	
	uint8_t curr_hid_type; // thr type of hid command : MOUSE_TYPE, KEYBOARD_TYPE, DELAY_TYPE
	
} hid_state_t;


#endif
