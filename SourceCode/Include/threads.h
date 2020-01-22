#ifndef THREADS_H
#define THREADS_H

#include <cmsis_os2.h>
#include "LCD_driver.h"
#include <MKL25Z4.H>

#define THREAD_READ_TS_PERIOD_MS (50)  // 1 tick/ms
#define THREAD_UPDATE_SCREEN_PERIOD_MS (100)
#define THREAD_BUS_PERIOD_MS (1)

#define USE_LCD_MUTEX (1)

// Custom stack sizes for larger threads

void Init_Debug_Signals(void);

// Events for sound generation and control
#define EV_PLAYSOUND (1) 
#define EV_SOUND_ON (2)
#define EV_SOUND_OFF (4)

#define EV_REFILL_SOUND_BUFFER  (1)

void Create_OS_Objects(void);
 
extern osThreadId_t t_Read_TS, t_Read_Accelerometer, t_Sound_Manager, t_US, t_Refill_Sound_Buffer;
extern osMutexId_t LCD_mutex;
extern osMessageQueueId_t request_msg_id, result_msg_id;
extern osStatus_t status, status_X_pos, status_Y_pos;

typedef struct
{
	uint32_t channelNum;
	osMessageQueueId_t Msg;
}REQUEST_MSG;

typedef struct
{
	uint32_t channelNum;
	uint32_t result;	
}RESULT_MSG;

extern REQUEST_MSG request_msg;
extern RESULT_MSG result_msg;

typedef enum {priority, queued} conversion_type; 

#endif // THREADS_H

