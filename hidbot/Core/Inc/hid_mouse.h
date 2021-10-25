#ifndef __HID_KEYBOARD_H
#define __HID_KEYBOARD_H

#include <stdint.h>





//struct HID Mouse
typedef struct  
{
	uint8_t id;
	uint8_t buttons;
	int8_t x;
	int8_t y;
	int8_t wheel;
} mouseHID_t;

#endif 
