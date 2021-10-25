/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_def.h"
///#include "usb_st_types.h"
#include "hid_mouse.h"
#include "hid_keyboard.h"
#include "hid_state.h"
#include "usbd_customhid.h"
#include "usbd_storage_if.h"
#include "mxconstants.h"
#include "usb_st_types.h"
#include "ve_module.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define STATES_LEN  2
typedef struct {
	uint8_t type;
	uint8_t name;
	uint8_t states[STATES_LEN];

} key;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CURSOR_STEP         5

//commands
#define CMD_LEN             20
#define ERR_TOO_LNG_CMD     1
#define ERR_WRNG_CMD        2
#define GOOD_CMD            0
#define KEYS_SIZE   (USB_HID_KEY_Z - USB_HID_KEY_A + 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define KEYS_SIZE   (USB_HID_KEY_Z - USB_HID_KEY_A + 1)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

osThreadId defaultTaskHandle;
osThreadId Usb_TaskHandle;
osThreadId Mouse_TaskHandle;
osThreadId Hid_TaskHandle;
osThreadId Ram_MSD_TaskHandle;
osMessageQId Usb_QueueHandle;
osMessageQId Hid_QueueHandle;
osTimerId delay_click_event_tmrHandle;
osTimerId periodic_click_event_tmrHandle;
osMutexId Ram_MSD_MutexHandle;

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t ram_msd[RAM_STORAGESIZ_USER];

extern __IO s_usbd_st_info st_info;
extern hid_state_t hid_state;
extern keyboardHID_t keyboardHID_zerrors;

__IO uint8_t hid_usb_init = 0;

//time in ms
uint32_t action_delay_time = 0;
uint32_t action_time = 0;

key keys[KEYS_SIZE];
uint16_t keys_tic = 0;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void Start_Usb_Task(void const * argument);
void Start_Mouse_Task(void const * argument);
void Start_Hid_Task(void const * argument);
void Start_Ram_MSD_Task(void const * argument);
void delay_click_event_tmr_Callback(void const * argument);
void periodic_click_event_Callback(void const * argument);

static void GetPointerData(void);
uint8_t parse_cmd(char cmd[]);
uint8_t parse_ram_msd(void);
uint8_t init_hid(void);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	osMutexDef(Ram_MSD_Mutex);
	Ram_MSD_MutexHandle = osMutexCreate(osMutex(Ram_MSD_Mutex));
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* definition and creation of delay_click_event_tmr */
	osTimerDef(delay_click_event_tmr, delay_click_event_tmr_Callback);
	delay_click_event_tmrHandle = osTimerCreate(osTimer(delay_click_event_tmr),
			osTimerOnce, NULL);

	/* definition and creation of periodic_click_event_tmr */
	osTimerDef(periodic_click_event_tmr, periodic_click_event_Callback);
	periodic_click_event_tmrHandle = osTimerCreate(
			osTimer(periodic_click_event_tmr), osTimerPeriodic, NULL);

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	osMessageQDef(Usb_Queue, 16, uint16_t);
	Usb_QueueHandle = osMessageCreate(osMessageQ(Usb_Queue), NULL);

	/* definition and creation of Hid_Queue */
	osMessageQDef(Hid_Queue, 16, uint16_t);
	Hid_QueueHandle = osMessageCreate(osMessageQ(Hid_Queue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */

	/* definition and creation of Usb_Task */
	osThreadDef(Usb_Task, Start_Usb_Task, osPriorityRealtime, 0, 128);
	Usb_TaskHandle = osThreadCreate(osThread(Usb_Task), NULL);

	/* definition and creation of Mouse_Task */
	osThreadDef(Mouse_Task, Start_Mouse_Task, osPriorityNormal, 0, 128);
	Mouse_TaskHandle = osThreadCreate(osThread(Mouse_Task), NULL);

	/* definition and creation of Hid_Task */
	osThreadDef(Hid_Task, Start_Hid_Task, osPriorityHigh, 0, 128);
	Hid_TaskHandle = osThreadCreate(osThread(Hid_Task), NULL);

	/* definition and creation of Ram_MSD_Task */
	osThreadDef(Ram_MSD_Task, Start_Ram_MSD_Task, osPriorityNormal, 0, 128);
	Ram_MSD_TaskHandle = osThreadCreate(osThread(Ram_MSD_Task), NULL);

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */

	hid_usb_init = 1;//HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);

	MX_USB_DEVICE_Init();

	/* Infinite loop */
	uint8_t tst = 1;
	uint8_t tst1 = 1;

	init_hid();

	for (;;) {
		if (tst1) {
			for (uint8_t i = 0; i < USB_HID_KEY_Z - USB_HID_KEY_A + 1; i++) {
				keys[i].type = KEYBOARD_TYPE;
				keys[i].name = 'a' + i;

				for(uint8_t j = 0; j < STATES_LEN;j++)
				  keys[i].states[j] =  (i & 1)?(0xFF):(0xFF);//(0xAA); test1

//test1				keys[i].states[0] = *((uint8_t *) (VIRT_EEPROM_ADDR
//						+ i * STATES_LEN));
//				keys[i].states[1] = *((uint8_t *) (VIRT_EEPROM_ADDR
//						+ i * STATES_LEN + 1));





			}

			tst1 = 0;
		}

		if (tst) {
			//ve_operate();
			for (uint32_t i = 0; i < 255; i++) {
				ram_msd[i] = *((uint8_t *) (VIRT_EEPROM_ADDR + i));

			}
			tst = 0;
		}
		osDelay(1);
		vTaskSuspend(NULL);

	}

  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* Start_Ram_MSD_Task function */
void Start_Ram_MSD_Task(void const * argument) {

	for (;;) {
		osDelay(1);
		if (hid_usb_init == 1)
			vTaskSuspend(NULL); //if we are his we dont need it.

		if (st_info.eof_flg == 1) {
			st_info.eof_flg = 0;
			osMutexWait(Ram_MSD_MutexHandle, osWaitForever);
			ve_operate(&st_info);
			osMutexRelease(Ram_MSD_MutexHandle);
		}

	}

}

/* delay_click_event_tmr_Callback function */
void delay_click_event_tmr_Callback(void const * argument) {

	uint8_t * keyboardHID_p = &(hid_state.keyboardHID.k_a);
	for (uint16_t j = 0; j < KEYS_SIZE; j++) {
		*(keyboardHID_p + j) = 0;
	}

	USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, &hid_state.keyboardHID,
			sizeof(keyboardHID_t));

}

/* periodic_click_event_Callback function */
void periodic_click_event_Callback(void const * argument) {

	static uint8_t * keyboardHID_p = &(hid_state.keyboardHID.k_a);
	static uint8_t bit_select = 0x01;
	static uint8_t i = 0;

	*(keyboardHID_p + i) =
			((keys[i].states[keys_tic]) & bit_select) ?
					(keys[i].name - 0x5D) : (0); //key_a = a;

	if (i == KEYS_SIZE - 1) //passed all the keys
			{
		i = 0;

		while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, &hid_state.keyboardHID,
				sizeof(keyboardHID_t)) != USBD_OK)
			; //add a tick


	}
	if (bit_select == 0x80) {
		(keys_tic == STATES_LEN - 1) ? (keys_tic = 0) : (keys_tic++);
		bit_select = 0x01;
	} else {
		bit_select = bit_select << 1;
	}

	i++;

}

/* Start_Hid_Task function */
void Start_Hid_Task(void const * argument) {
	uint8_t * keyboardHID_p = &(hid_state.keyboardHID.k_a);
	static uint8_t bit_select = 0x01;
	/* Infinite loop */
	for (;;) {
		osDelay(100);

		if (hid_usb_init == 1) {
			osTimerStart(periodic_click_event_tmrHandle, 10);
			vTaskSuspend(NULL);

		}

	}

}

/* Start_Usb_Task function */
void Start_Usb_Task(void const * argument) {

	for (;;) {
		osDelay(1);
		vTaskSuspend(NULL);
	}

}

/* Start_Mouse_Task function */
void Start_Mouse_Task(void const * argument) {
	vTaskSuspend(NULL);
	osEvent event;
	/* Infinite loop */
	for (;;) {
		osDelay(1);
		event = osMessageGet(Hid_QueueHandle, osWaitForever);
		if (event.status == osEventMessage)
			switch (event.value.v) {
			case M_BUTTON_NOP: {
				hid_state.mouseHID.buttons = M_BUTTON_NOP;
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case M_BUTTON_1: {
				hid_state.mouseHID.buttons = M_BUTTON_1;
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case M_BUTTON_2: {
				hid_state.mouseHID.buttons = M_BUTTON_2;
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case M_BUTTON_3: {
				hid_state.mouseHID.buttons = M_BUTTON_3;
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case WHEEL: {
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case M_X: {
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case M_Y: {
				hid_state.curr_hid_type = MOUSE_TYPE;
				break;
			}

			case KEYBOARD_TYPE: {
				hid_state.curr_hid_type = KEYBOARD_TYPE;
				break;
			}

			}

		if (hid_state.curr_hid_type == MOUSE_TYPE)
			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
					(uint8_t *) &hid_state.mouseHID, sizeof(mouseHID_t));
		else if (hid_state.curr_hid_type == KEYBOARD_TYPE) {

			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, &hid_state.keyboardHID,
					sizeof(keyboardHID_t));
			osTimerStart(delay_click_event_tmrHandle, 500);

		}

	}

}

//TODO add cklick / press logic
/*
 * parses the cmd
 * @param cmd, the command should end with '\0'
 */
uint8_t parse_cmd(char cmd[]) {
	uint8_t inp_type /*mouse, delay or keyboard*/, cmd_type /*click or press*/;

	uint16_t cmd_len = strlen(cmd);
	uint16_t i = 0;
	char pattern_m[] = "m"; //mouse
	int16_t pattern_m_len = strlen(pattern_m);

	char pattern_c[] = "c"; //click
	int16_t pattern_c_len = strlen(pattern_c);

	char pattern_p[] = "p"; //press
	int16_t pattern_p_len = strlen(pattern_p);

	char pattern_k[] = "k"; //keboard
	int16_t pattern_k_len = strlen(pattern_k);

	char delimiter = ',';

	uint8_t input_value = 0;
	uint8_t input_time = 0;

	//decide press or click
	if (cmd[i] == 'c')
		cmd_type = CLICK_TYPE;
	else if (cmd[i] == 'p')
		cmd_type = PRESS_TYPE;
	else
		return ERR_WRNG_CMD;
	i++; //one step

	if (cmd[i] != delimiter)
		return ERR_WRNG_CMD;
	i++; //scip delimiter

	while (i < pattern_m_len) {

		if (pattern_m[i] == cmd[i])
			inp_type = MOUSE_TYPE;
		else if (pattern_k[i] == cmd[i])
			inp_type = KEYBOARD_TYPE;
		else
			return ERR_WRNG_CMD;
		i++; //one step
	}

	if (cmd[i] != delimiter)
		return 1;
	i++; //scip delimiter

	if (inp_type == MOUSE_TYPE) {

		if (cmd[i] == 'b') //buttons
				{
			//< mouse,b,3 >
			i++; //'b'
			if (cmd[i] != delimiter)
				return ERR_WRNG_CMD;
			i++; //scip delimiter

			input_value = cmd[i] - '0';
			if (input_value > M_BUTTON_MAX)
				return ERR_WRNG_CMD;

			switch (input_value) {
			case M_BUTTON_NOP: {
				osMessagePut(Hid_QueueHandle, M_BUTTON_NOP, 0);
				break;
			}

			case M_BUTTON_1: {
				osMessagePut(Hid_QueueHandle, M_BUTTON_1, 0);
				break;
			}

			case M_BUTTON_2: {
				osMessagePut(Hid_QueueHandle, M_BUTTON_2, 0);
				break;
			}

			case M_BUTTON_3: {
				osMessagePut(Hid_QueueHandle, M_BUTTON_3, 0);
				break;
			}

			}

		} else if (cmd[i] == 'w') //wheel
				{
			//< m,v,100 >
			i++; //'b'
			if (cmd[i] != delimiter)
				return ERR_WRNG_CMD;
			i++; //scip delimiter

			hid_state.mouseHID.wheel += atoi(cmd + i);
			osMessagePut(Hid_QueueHandle, WHEEL, 0);

		} else if (cmd[i] == 'x') // pointer
				{
			i++; //'x'
			if (cmd[i] != delimiter)
				return ERR_WRNG_CMD;
			i++; //scip delimiter

			hid_state.mouseHID.x += atoi(cmd + i);
			osMessagePut(Hid_QueueHandle, M_X, 0);

		} else if (cmd[i] == 'y') // pointer
				{
			i++; //'y'
			if (cmd[i] != delimiter)
				return ERR_WRNG_CMD;
			i++; //scip delimiter

			hid_state.mouseHID.y += atoi(cmd + i);
			osMessagePut(Hid_QueueHandle, M_Y, 0);

		}
	} else if (inp_type = KEYBOARD_TYPE)
	{
		//TODO:
		//1.press spesific button on the keyboard if 'a' press hid_state.keyboardHID.k_a...
		hid_state.keyboardHID.k_a = cmd[i] - 0x5D;
		osMessagePut(Hid_QueueHandle, KEYBOARD_TYPE, 0);
	} else if (inp_type = DELAY_TYPE)
	{
		//TODO use timer
		osMessagePut(Hid_QueueHandle, DELAY_TYPE, 0);
	}

	return GOOD_CMD;

}

uint8_t parse_ram_msd()
{
	char delimiter = ';';
	char cmd[CMD_LEN];
	uint8_t cmd_i = 0;

	char pattern[] = "EOF";
	int16_t pattern_len = strlen(pattern);

	static uint16_t cur_ind_in_ram = 0;

	if(cur_ind_in_ram >= RAM_STORAGESIZ_USER - pattern_len)
		cur_ind_in_ram = 0;

	for(uint16_t i= cur_ind_in_ram; i < RAM_STORAGESIZ_USER - pattern_len; i++)
	{
		//check for EOF
		for(uint8_t k =0; k < pattern_len; k++)
		{
			if(ram_msd[i+k] != pattern[k])
			{
				break;
			}

			if(k == pattern_len -1)
			{
				cur_ind_in_ram = 0;
				return 0;
			}
		}

		if(ram_msd[i]!=delimiter)
		{
			cmd[cmd_i] = ram_msd[i];
			cmd_i++;

			if(cmd_i >= CMD_LEN )
				return ERR_TOO_LNG_CMD; //error too long command
		}
		else
		{
			cmd[cmd_i+1]='\0';
			cmd_i =0;
			parse_cmd(cmd);
			osDelay(1000); //change delete

			cur_ind_in_ram = i+1;
			return 0;


		}

	}
	return 0;
}


/**
 * @param  pbuf: Pointer to report
 * @brief  Gets Pointer Data.
 * @retval None
 */
static void GetPointerData(void) {

	(hid_state.mouseHID.buttons) ?
			(hid_state.mouseHID.buttons = 0) : (hid_state.mouseHID.buttons = 2);

}

uint8_t init_hid(void) {
	hid_state.keyboardHID.id = 1;
	hid_state.keyboardHID.modifiers = 0;
	hid_state.mouseHID.id = 2;
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/