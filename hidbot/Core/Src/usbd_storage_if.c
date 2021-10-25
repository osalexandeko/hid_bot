/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @brief          : Memory management layer
  ******************************************************************************
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "mxconstants.h"
#include "usbd_storage_if.h"
/* USER CODE BEGIN INCLUDE */
//#include "cmsis_os.h"
#include "usb_st_types.h"
/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_STORAGE 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_TYPES */

	

/* USER CODE END PRIVATE_TYPES */ 
/**
  * @}
  */ 

/** @defgroup USBD_STORAGE_Private_Defines
  * @{
  */ 
#define STORAGE_LUN_NBR                  1  
#define STORAGE_BLK_NBR                  0x10000  
#define STORAGE_BLK_SIZ                  0x200

/* USER CODE BEGIN PRIVATE_DEFINES */
#define EOF_LEN 3
/* USER CODE END PRIVATE_DEFINES */
  
/**
  * @}
  */ 

/** @defgroup USBD_STORAGE_Private_Macros
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_MACRO */
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */ 

/** @defgroup USBD_STORAGE_IF_Private_Variables
  * @{
  */
/* USER CODE BEGIN INQUIRY_DATA_FS */ 
/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata_FS[] = {/* 36 */
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
}; 
/* USER CODE END INQUIRY_DATA_FS */ 

/* USER CODE BEGIN PRIVATE_VARIABLES */

 
__IO uint8_t ram_msd[RAM_STORAGESIZ_USER];

__IO s_usbd_st_info st_info = {.eof_flg=0, .blk_len =0}; //1 iff file is in ram

uint32_t BankNumber = 0;
//uint32_t Address = 0, PAGEError_t = 0;
// 
//// /*Variable used for Erase procedure*/
// static FLASH_EraseInitTypeDef EraseInitStruct;
// 
// __IO uint64_t dword_to_write = 0;

__IO uint8_t block_counter = 0;

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */ 
 
/** @defgroup USBD_STORAGE_IF_Exported_Variables
  * @{
  */ 
  extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */ 
  
/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @{
  */
static int8_t STORAGE_Init_FS (uint8_t lun);
static int8_t STORAGE_GetCapacity_FS (uint8_t lun, 
                           uint32_t *block_num, 
                           uint16_t *block_size);
static int8_t  STORAGE_IsReady_FS (uint8_t lun);
static int8_t  STORAGE_IsWriteProtected_FS (uint8_t lun);
static int8_t STORAGE_Read_FS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);
static int8_t STORAGE_Write_FS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS (void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

static void eof_check(uint32_t blk_addr); //cheks if EOF is reached

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */ 
  
USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *)STORAGE_Inquirydata_FS,
};

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : STORAGE_Init_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Init_FS (uint8_t lun)
{
  /* USER CODE BEGIN 2 */ 
  return (USBD_OK);
  /* USER CODE END 2 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_GetCapacity_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetCapacity_FS (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */   
 	
	*block_num  = STORAGE_BLK_NBR_USER;   
  *block_size = STORAGE_BLK_SIZ_USER;
	
  return (USBD_OK);
  /* USER CODE END 3 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_IsReady_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsReady_FS (uint8_t lun)
{
  /* USER CODE BEGIN 4 */ 
  return (USBD_OK);
  /* USER CODE END 4 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_IsWriteProtected_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsWriteProtected_FS (uint8_t lun)
{
  /* USER CODE BEGIN 5 */ 
  return (USBD_OK);
  /* USER CODE END 5 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_Read_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Read_FS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,                       
                        uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */ 
	
	__IO uint32_t addr_rd = ((blk_addr)*STORAGE_BLK_SIZ_USER) ;
	__IO uint32_t i = 0;
  __IO uint8_t c;
	
	__IO uint16_t blk_len_tr = blk_len;
	
	 
	 for ( i = 0; i < STORAGE_BLK_SIZ_USER*blk_len; i++)
	 {
			c = ram_msd[addr_rd  + i];
			buf[i] = c;
	 }
	 
	 

	 
	  
	
   return (USBD_OK);
  /* USER CODE END 6 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_Write_FS
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Write_FS (uint8_t lun, 
                         uint8_t *buf, 
                         uint32_t blk_addr,
                         uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */ 
	__IO uint32_t addr_wr = ((blk_addr)*STORAGE_BLK_SIZ_USER) ;
	__IO uint32_t j = 0;
	__IO uint8_t ch;

	//__IO uint16_t blk_len_t = blk_len;

	 
	for ( j = 0; j <  STORAGE_BLK_SIZ_USER*blk_len ; j++)
	{
		ch = buf[j];
		ram_msd[addr_wr + j] = ch;
	}
	
	eof_check(blk_addr);
	st_info.blk_len = blk_len;
	
//	block_counter ++;
		
//	if(block_counter >= STORAGE_BLK_NBR_USER)
//	{
//		block_counter = 0;
	
//	//write data to flash
//		 //	uint32_t NbOfPages = 1;//FLASH_PAGE_SIZE 2 kbytes
//	
//	HAL_FLASH_Unlock();
//	
//	/* Clear OPTVERR bit set on virgin samples */
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
//	
//	/* Get the bank */
//	
//	EraseInitStruct.NbPages = NUM_OF_PGS;
//	EraseInitStruct.PageAddress = VIRT_EEPROM_ADDR;
//	EraseInitStruct.Banks     = 3;
//	
//	
//	
//	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError_t) != HAL_OK)
//	{
//		/*
//		Error occurred while mass erase.
//		User can add here some code to deal with this error.
//		To know the code error, user can call function 'HAL_FLASH_GetError()'
//		*/
//		/* Infinite loop */
//		Error_Handler();
//	}
//	//dword_to_write  = ((0x05ULL << 24 )|(0x06ULL << 16 )|(0x07ULL << 8 )|(0x08ULL ));
//	//dword_to_write = dword_to_write << 32;
//	//dword_to_write  = ((0x01 << 24 )|(0x02 << 16 )|(0x03 << 8 )|(0x04 ));
//	dword_to_write  = ((0x01ULL << 56 )|(0x02ULL << 48 )|(0x03ULL << 40 )|(0x04ULL << 32))|(0x05 << 24 )|(0x06 << 16 )|(0x07 << 8 )|(0x08 );
//	
//	if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, VIRT_EEPROM_ADDR,  dword_to_write) != HAL_OK)
//	{
//		Error_Handler();
//	}
//	
//	HAL_FLASH_Lock();
//	 }
	 
  return (USBD_OK);
  /* USER CODE END 7 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_GetMaxLun_FS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetMaxLun_FS (void)
{
  /* USER CODE BEGIN 8 */ 
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */ 
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

static void eof_check(uint32_t blk_addr)
{
	uint8_t pattern[EOF_LEN] = "EOF";
	uint32_t i,j;

	for(i = STORAGE_BLK_SIZ_USER*blk_addr; i < (STORAGE_BLK_SIZ_USER-EOF_LEN)*(blk_addr+1);i++)
	{
		for(j = 0; j < EOF_LEN ;j++)
		{
			if(ram_msd[i + j] != pattern[j])
			{
				break;
			}
			else if(j < EOF_LEN -1)
				continue;
				
			st_info.eof_flg = 1;
			return;
		}	
	}
	
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */ 

/**
  * @}
  */  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
