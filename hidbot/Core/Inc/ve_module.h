#ifndef  __VE_MODULE_H
#define  __VE_MODULE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "usb_st_types.h"
#include "mxconstants.h"

#define UINT32_T_LEN 8

extern uint8_t ram_msd[RAM_STORAGESIZ_USER ] ;
//extern __IO s_usbd_st_info st_info; //TBD!

//static void ve_pgs_prep(s_usbd_st_info *);
//static void ve_prg_blks(s_usbd_st_info *);
//static uint8_t ve_is_pg_erased(uint32_t );
uint8_t ve_operate(s_usbd_st_info * );

#endif
