#include "ve_module.h"

uint32_t Address = 0, PAGEError_t = 0;
 
// /*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
 
__IO uint64_t dword_to_write = 0;

extern uint8_t ram_msd[RAM_STORAGESIZ_USER] ;


//test
s_usbd_st_info tst_info; 


/*
*@param uint32_t p_addr the address of the page
*
*/
static uint8_t  ve_is_pg_erased(uint32_t p_addr)
{
	__IO uint8_t c =0;
	for(uint32_t i = 0; i < FLASH_PAGE_SIZE;i++)
	{
		c = *((uint8_t *)(p_addr + i));
		if( c != 0xFF )
		{
			return 0;
		}
		
	}
	
	return 1;
	
}


/*
* prepares the flash for programming.
*
*@param st_info storage information
*
*/
static void ve_pgs_prep(s_usbd_st_info * p_st_info)
{
	__IO uint8_t pg_num = ( p_st_info->blk_len / 2) + (p_st_info->blk_len & 1);
	
	
	HAL_FLASH_Unlock();
	
	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
	
	/* Get the bank */
	
	EraseInitStruct.NbPages = 1;
	//EraseInitStruct.PageAddress = VIRT_EEPROM_ADDR;
	EraseInitStruct.Banks     = 3;
	
	for(uint8_t pg_curr = 0;pg_curr < pg_num; pg_curr++)
	{
		EraseInitStruct.PageAddress = VIRT_EEPROM_ADDR + FLASH_PAGE_SIZE*pg_curr;
		 
		if(ve_is_pg_erased(EraseInitStruct.PageAddress)) continue;
			
		
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError_t) != HAL_OK)
		{
			/*
			Error occurred while mass erase.
			User can add here some code to deal with this error.
			To know the code error, user can call function 'HAL_FLASH_GetError()'
			*/
			/* Infinite loop */
			//test1 Error_Handler();
		}
	
	}
	
	HAL_FLASH_Lock();
}


/*
* programming the flash.
*
*@param st_info storage information
*
*/
static void ve_prg_blks(s_usbd_st_info * p_st_info)
{
	HAL_FLASH_Unlock();
	
	
	for (uint32_t j = 0; j <  STORAGE_BLK_SIZ_USER*(p_st_info->blk_len) ; j+= UINT32_T_LEN)
	{
		 
//		dword_to_write  = (((uint64_t )ram_msd[j + 4] << 56 )|((uint64_t )ram_msd[j + 5] << 48 )|
//											 ((uint64_t )ram_msd[j + 6] << 40 )|((uint64_t )ram_msd[j + 7] << 32))|
//											(((uint64_t )ram_msd[j    ] << 24 )|((uint64_t )ram_msd[j + 1] << 16) |
//											 ((uint64_t )ram_msd[j + 2] << 8  )|((uint64_t )ram_msd[j + 3]));
		
//			dword_to_write  = (((uint64_t )ram_msd[j ] << 56 )|((uint64_t )ram_msd[j + 1] << 48 )|
//											  ((uint64_t )ram_msd[j + 2] << 40 )|((uint64_t )ram_msd[j + 3] << 32))|
//										  	(((uint64_t )ram_msd[j + 4] << 24 )|((uint64_t )ram_msd[j + 5] << 16)|
//											  ((uint64_t )ram_msd[j + 6] << 8  )|((uint64_t )ram_msd[j + 7]));
		
		dword_to_write  = (((uint64_t )ram_msd[j ]   )|((uint64_t )ram_msd[j + 1] << 8 )|
											  ((uint64_t )ram_msd[j + 2] << 16 )|((uint64_t )ram_msd[j + 3] << 24))|
										  	(((uint64_t )ram_msd[j + 4] << 32 )|((uint64_t )ram_msd[j + 5] << 40)|
											  ((uint64_t )ram_msd[j + 6] << 48  )|((uint64_t )ram_msd[j + 7]<<56));
		
		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, VIRT_EEPROM_ADDR+j,  dword_to_write) != HAL_OK)
		{
			Error_Handler();
		}
	}
	
	HAL_FLASH_Lock();
}



uint8_t ve_operate( s_usbd_st_info * p_st_info )
{
	ve_pgs_prep(p_st_info);
	ve_prg_blks(p_st_info);
	return 0;
}
