#ifndef __HID_MOUSE_H
#define __HID_MOUSE_H
//hid_keyboard.h
#include <stdint.h>



// USB media codes
#define USB_HID_SCAN_NEXT 0x01
#define USB_HID_SCAN_PREV 0x02
#define USB_HID_STOP      0x04
#define USB_HID_EJECT     0x08
#define USB_HID_PAUSE     0x10
#define USB_HID_MUTE      0x20
#define USB_HID_VOL_UP    0x40
#define USB_HID_VOL_DEC   0x80

// USB keyboard codes
#define USB_HID_MODIFIER_LEFT_CTRL   0x01
#define USB_HID_MODIFIER_LEFT_SHIFT  0x02
#define USB_HID_MODIFIER_LEFT_ALT    0x04
#define USB_HID_MODIFIER_LEFT_GUI    0x08 // (Win/Apple/Meta)
#define USB_HID_MODIFIER_RIGHT_CTRL  0x10
#define USB_HID_MODIFIER_RIGHT_SHIFT 0x20
#define USB_HID_MODIFIER_RIGHT_ALT   0x40
#define USB_HID_MODIFIER_RIGHT_GUI   0x80

#define K_BUTTON_NOP   0

#define USB_HID_KEY_A     0x04
#define USB_HID_KEY_B     0x05
#define USB_HID_KEY_C     0x06
#define USB_HID_KEY_D     0x07
#define USB_HID_KEY_E     0x08
#define USB_HID_KEY_F     0x09
#define USB_HID_KEY_G     0x0A
#define USB_HID_KEY_H     0x0B
#define USB_HID_KEY_I     0x0C
#define USB_HID_KEY_J     0x0D
#define USB_HID_KEY_K     0x0E
#define USB_HID_KEY_L     0x0F
#define USB_HID_KEY_M     0x10
#define USB_HID_KEY_N     0x11
#define USB_HID_KEY_O     0x12
#define USB_HID_KEY_P     0x13
#define USB_HID_KEY_Q     0x14
#define USB_HID_KEY_R     0x15
#define USB_HID_KEY_S     0x16
#define USB_HID_KEY_T     0x17
#define USB_HID_KEY_U     0x18
#define USB_HID_KEY_V     0x19
#define USB_HID_KEY_W     0x1A
#define USB_HID_KEY_X     0x1B
#define USB_HID_KEY_Y     0x1C
#define USB_HID_KEY_Z     0x1D

 

//struct HID Mouse
//typedef struct  
//{
//	uint8_t buttons;
//	int8_t x;
//	int8_t y;
//	int8_t wheel;
//} mouseHID_t;

// HID Keyboard
typedef struct __keyboardHID_t
{
		uint8_t id;
		uint8_t modifiers;
		uint8_t k_a;
		uint8_t k_b;
		uint8_t k_c;
	  uint8_t k_d;
		uint8_t k_e;
		uint8_t k_f;
		uint8_t k_g;
	  uint8_t k_h;
		uint8_t k_i;
		uint8_t k_j;
		uint8_t k_k;
	  uint8_t k_l;
		uint8_t k_m;
		uint8_t k_n;
		uint8_t k_o;
	  uint8_t k_p;
	  uint8_t k_q;
	  uint8_t k_r;
		uint8_t k_s;
		uint8_t k_t;
		uint8_t k_u;
	  uint8_t k_v;
		uint8_t k_w;
		uint8_t k_x;
		uint8_t k_y;
	  uint8_t k_z;
} keyboardHID_t;

//// HID Media
//struct mediaHID_t
//{
//	uint8_t id;
//	uint8_t keys;
//}; 

#endif 
