#include "timers.h"
#include "MKL25Z4.h"
#include "region.h"
#include "profile.h"
#include "debug.h"
// #include "HBLED.h"
#include "control.h"

volatile unsigned PIT_interrupt_counter = 0;
volatile unsigned LCD_update_requested = 0;
unsigned int PC_val = 0;

extern unsigned profile_samples;
extern volatile char profiling_enabled;

extern volatile unsigned int adx_lost, num_lost; 

void PWM_Init(TPM_Type * TPM, uint8_t channel_num, uint16_t period, uint16_t duty, 
	uint8_t pos_polarity, uint8_t prescaler_code)
{
	//turn on clock to TPM 
	switch ((int) TPM) {
		case (int) TPM0:
			SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
			break;
		case (int) TPM1:
			SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
			break;
		case (int) TPM2:
			SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
			break;
		default:
			break;
	}
	//set clock source for tpm
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

	//load the counter and mod
	TPM->MOD = period;

	if (pos_polarity) {
		//set channel to center-aligned high-true PWM
		TPM->CONTROLS[channel_num].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	} else {
		//set channel to center-aligned low-true PWM
		TPM->CONTROLS[channel_num].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
	}
	//set TPM to up-down and divide by 1 prescaler and clock mode
	TPM->SC = (TPM_SC_CPWMS_MASK | TPM_SC_PS(prescaler_code));
	//set trigger mode, keep running when in debug 
	TPM->CONF |= TPM_CONF_TRGSEL(0xA) | TPM_CONF_DBGMODE(3);
	// Set initial duty cycle
	TPM->CONTROLS[channel_num].CnV = duty;

	#if USE_TPM0_INTERRUPT // if using interrupt 
	TPM0->SC |= TPM_SC_TOIE_MASK;
	// Configure NVIC 
	NVIC_SetPriority(TPM0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);	
#endif

	// Start the timer counting
	TPM->SC |= TPM_SC_CMOD(1);
}

void PWM_Set_Value(TPM_Type * TPM, uint8_t channel_num, uint16_t value) {
	TPM->CONTROLS[channel_num].CnV = value;
}

void TPM0_IRQHandler() {
	static uint32_t control_divider = SW_CTL_FREQ_DIV_FACTOR;
	
	FPTB->PSOR = MASK(DBG_IRQTPM);
	//clear pending IRQ flag
	TPM0->SC |= TPM_SC_TOF_MASK; 

	control_divider--;
	if (control_divider == 0) {
		control_divider = SW_CTL_FREQ_DIV_FACTOR;
		// Start conversion
		ADC0->SC1[0] = ADC_SC1_AIEN(1) | ADC_SENSE_CHANNEL;
		
	#if USE_ADC_INTERRUPT
		// can return immediately
	#else
		// Call control function, which will wait for ADC coco
		Control_HBLED();
	#endif
	}
	FPTB->PCOR = MASK(DBG_IRQTPM);
}
void PIT_Init(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}

void PIT_Start(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void PIT_Stop(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

void PIT_IRQHandler() {
	unsigned int s, e;
  unsigned int i;
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		// Do ISR work
		// Profiler
		if (profiling_enabled > 0) {
			PC_val = *((unsigned int *) (__current_sp()+CUR_FRAME_SIZE+RET_ADX_OFFSET));
			profile_samples++;
  	
			/* look up function in table and increment counter  */
			for (i=0; i<NumProfileRegions; i++) {
				s = RegionTable[i].Start;
				e = RegionTable[i].End;
				if ((PC_val >= s) && (PC_val <= e)) {
					RegionCount[i]++;
					break; // break out of the for loop
				}
			}
			if (i == NumProfileRegions) {
				adx_lost = PC_val;
				num_lost++;
			}
		}
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	} 
}

void TPM0_Init(void) {
	//turn on clock to TPM 
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	//set clock source for tpm
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
}

void Configure_TPM0_for_DMA(uint32_t period_us)
{

	// disable TPM
	TPM0->SC = 0;
	
	//load the counter and mod
	TPM0->MOD = TPM_MOD_MOD(period_us*48);

	//set TPM to count up and divide by 1 prescaler and clock mode
	TPM0->SC = (TPM_SC_DMA_MASK | TPM_SC_PS(0));
	
#if 0 // if using interrupt for debugging
	// Enable TPM interrupts for debugging
	TPM0->SC |= TPM_SC_TOIE_MASK;

	// Configure NVIC 
	NVIC_SetPriority(TPM0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);	
#endif

}

void TPM0_Start(void) {
// Enable counter
	TPM0->SC |= TPM_SC_CMOD(1);
}




// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
