#include <stdint.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include <MKL25Z4.h>

#include "LCD.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"
#include "threads.h"
#include "sound.h"
#include "DMA.h"
#include "gpio_defs.h"
#include "debug.h"
#include "control.h"
#include "UI.h"

#include "ST7789.h"
#include "T6963.h"

void Thread_Read_TS(void * arg); // 
void Thread_Update_Screen(void * arg); // 
void Thread_Sound_Manager(void * arg); // 
void Thread_Refill_Sound_Buffer(void * arg); //
void Thread_Buck_Update_Setpoint(void * arg);

osThreadId_t t_Read_TS, t_Sound_Manager, t_US, t_Refill_Sound_Buffer, t_BUS;

// Thread priority options: osPriority[RealTime|High|AboveNormal|Normal|BelowNormal|Low|Idle]

const osThreadAttr_t Read_TS_attr = {
  .priority = osPriorityNormal            
};

const osThreadAttr_t Update_Screen_attr = {
  .priority = osPriorityNormal, 
	.stack_size = 512
};

const osThreadAttr_t BUS_attr = {
  .priority = osPriorityAboveNormal            
};

osMutexId_t LCD_mutex;

const osMutexAttr_t LCD_mutex_attr = {
  "LCD_mutex",     // human readable mutex name
  osMutexPrioInherit    // attr_bits
};


void Create_OS_Objects(void) {
	LCD_mutex = osMutexNew(&LCD_mutex_attr);

	t_Read_TS = osThreadNew(Thread_Read_TS, NULL, &Read_TS_attr);  
	t_US = osThreadNew(Thread_Update_Screen, NULL, &Update_Screen_attr);
	t_BUS = osThreadNew(Thread_Buck_Update_Setpoint, NULL, &BUS_attr);
	
	request_msg_id = osMessageQueueNew(2,sizeof(REQUEST_MSG),NULL);  // Creates a queue for the request message and returns a msg queue ID
	result_msg_id = osMessageQueueNew(2,sizeof(RESULT_MSG),NULL);    // Creates a queue for the result message and returns a msg queue ID
}

void Thread_Read_TS(void * arg) {
	PT_T p;
	
	while (1) {
		DEBUG_START(DBG_TREADTS);
		if (LCD_TS_Read(&p)) { 
			UI_Process_Touch(&p);
		}
		DEBUG_STOP(DBG_TREADTS);
		osDelay(THREAD_READ_TS_PERIOD_MS);
	}
}

 void Thread_Update_Screen(void * arg) {
	
 UI_Draw_Screen(1);
	 
	while (1) {
		DEBUG_START(DBG_TUPDATESCR);
		UI_Draw_Screen(0);
	
		DEBUG_STOP(DBG_TUPDATESCR);
		osDelay(THREAD_UPDATE_SCREEN_PERIOD_MS);
	}
}

 void Thread_Buck_Update_Setpoint(void * arg) {
	while (1) {
		osDelay(THREAD_BUS_PERIOD_MS);
		Update_Set_Current();
	}
 }
 