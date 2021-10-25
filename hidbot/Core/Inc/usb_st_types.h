#ifndef __USB_ST_TYPES_H
#define __USB_ST_TYPES_H

#include <stdint.h>

typedef struct 
{
	uint8_t eof_flg ; //1 iff file is in ram
	uint32_t blk_len; //number of blockes
} s_usbd_st_info;

 

#endif 
