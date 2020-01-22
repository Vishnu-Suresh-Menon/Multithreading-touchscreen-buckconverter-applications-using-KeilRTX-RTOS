/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "math.h"

#include "gpio_defs.h"

#include <cmsis_os2.h>
#include "threads.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "LEDs.h"
#include "timers.h"
#include "sound.h"
#include "DMA.h"
#include "delay.h"
#include "profile.h"

#include "control.h"

volatile CTL_MODE_E control_mode=DEF_CONTROL_MODE;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	Init_Debug_Signals();
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,1);			
	
	Sound_Disable_Amp();
	
	LCD_Init();
	LCD_Text_Init(1);
	LCD_Erase();
	
	LCD_Erase();
	LCD_Text_PrintStr_RC(0,0, "Test Code");

#if 0
	// LCD_TS_Calibrate();
	LCD_TS_Test();
#endif
	Delay(70);
	LCD_Erase();

	Init_Buck_HBLED();
	
	osKernelInitialize();
	Create_OS_Objects();
	osKernelStart();	
}
