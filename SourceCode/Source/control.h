#ifndef CONTROL_H
#define CONTROL_H

#include "FX.h"
#include "config.h"
#include "UI.h"

// Flash parameters
#define FLASH_PERIOD_MS (600)
#define FLASH_CURRENT_MA (40)
#define FLASH_DURATION_MS (100)
#define ENABLE_PRE_FLASH (0)
#define FLASH_STORE_PLOT_MS (496)
#define QUEUE_CONV_TIME (710)

// Switching parameters
#define PWM_HBLED_CHANNEL (4)
#define PWM_PERIOD (500) 
/* 48 MHz input clock. 
	PWM frequency = 48 MHz/(PWM_PERIOD*2) 
	Timer is in count-up/down mode. */
#define LIM_DUTY_CYCLE (PWM_PERIOD-1)

// Control approach configuration
#define USE_ASYNC_SAMPLING 				0
#define USE_SYNC_NO_FREQ_DIV 			1
#define USE_SYNC_SW_CTL_FREQ_DIV 	0
#define USE_SYNC_HW_CTL_FREQ_DIV 	0

#define SW_CTL_FREQ_DIV_FACTOR (4) // Software division in ISR
#define HW_CTL_FREQ_DIV_CODE (0) // Not used, not supported

#if USE_ASYNC_SAMPLING
#define 	USE_TPM0_INTERRUPT 0
#define 	USE_ADC_HW_TRIGGER 0
#define 	USE_ADC_INTERRUPT 1
#endif

#if USE_SYNC_NO_FREQ_DIV
#define 	USE_TPM0_INTERRUPT 0
#define 	USE_ADC_HW_TRIGGER 1
#define 	USE_ADC_INTERRUPT 1
#endif

#if USE_SYNC_SW_CTL_FREQ_DIV
#define 	USE_TPM0_INTERRUPT 1
#define 	USE_ADC_HW_TRIGGER 0
#define 	USE_ADC_INTERRUPT 1
#endif

#if USE_SYNC_HW_CTL_FREQ_DIV
#define 	USE_TPM0_INTERRUPT 0
#define 	USE_ADC_HW_TRIGGER 1
#define 	USE_ADC_INTERRUPT 1
#endif



// Control Parameters
// default control mode: OpenLoop, BangBang, Incremental, PID, PID_FX
// #define DEF_CONTROL_MODE (Incremental)
#define DEF_CONTROL_MODE (PID_FX)

// Incremental controller: change amount
#define INC_STEP (PWM_PERIOD/100)

// Proportional Gain, scaled by 2^8
#define PGAIN_8 (0x0028)

// PID (floating-point) gains. Guaranteed to be non-optimal. 
#define P_GAIN_FL (0.600f)
#define I_GAIN_FL (0.000f)
#define D_GAIN_FL (0.000f)

// PID_FX (fixed-point) gains. Guaranteed to be non-optimal. 
#define P_GAIN_FX (70000)	
#define I_GAIN_FX (40) 
#define D_GAIN_FX (40000)

// Data type definitions
typedef struct {
	float dState; // Last position input
	float iState; // Integrator state
	float iMax, iMin; // Maximum and minimum allowable integrator state
	float pGain, // proportional gain
				iGain, // integral gain
				dGain; // derivative gain
} SPid;

typedef struct {
	FX16_16 dState; // Last position input
	FX16_16 iState; // Integrator state
	FX16_16 iMax, iMin; // Maximum and minimum allowable integrator state
	FX16_16 pGain, // proportional gain
				iGain, // integral gain
				dGain; // derivative gain
} SPidFX;

typedef enum {OpenLoop, BangBang, Incremental, Proportional, PID, PID_FX} CTL_MODE_E;

// Functions
void Init_Buck_HBLED(void);
void Update_Set_Current(void);

// Handler functions (callbacks)
void Control_OnOff_Handler (UI_FIELD_T * fld, int v);
void Control_IntNonNegative_Handler (UI_FIELD_T * fld, int v);
void Control_DutyCycle_Handler(UI_FIELD_T * fld, int v);

// Shared global variables
extern volatile int g_set_current; // Default starting LED current
extern volatile int g_peak_set_current;
extern volatile int g_flash_duration;
extern volatile int g_flash_period; 

extern volatile int g_measured_current;
extern volatile int32_t g_duty_cycle;  // global to give debugger access

extern volatile int g_enable_control;
extern volatile CTL_MODE_E control_mode;
extern volatile int g_enable_flash;
extern volatile int error;

extern SPidFX plantPID_FX;
extern SPid plantPID;

extern volatile int g_measured_current_array[240], g_set_current_array[240];
extern uint32_t start_plot;
static uint32_t  start_store_array;

#define NUM_OF_SAMPLES (4)
#define SCREEN_WIDTH  (240)

// Hardware configuration
#define ADC_SENSE_CHANNEL (8)

#define R_SENSE (2.2f)
#define R_SENSE_MO ((int) (R_SENSE*1000))

#define V_REF (3.3f)
#define V_REF_MV ((int) (V_REF*1000))

#define ADC_FULL_SCALE (0x10000)
#define MA_SCALING_FACTOR (1000)


#ifndef DAC_POS
	#define DAC_POS 30
#endif

#define DAC_RESOLUTION 4096

#define MA_TO_DAC_CODE(i) ((i)*(2.2f*DAC_RESOLUTION/V_REF_MV))
// #define MA_TO_DAC_CODE(i) (i*2.2*DAC_RESOLUTION/V_REF_MV) // Introduces timing delay and interesting bug!

#endif // #ifndef CONTROL_H
