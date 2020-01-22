#include <stdint.h>
#include <MKL25Z4.h>
#include <cmsis_os2.h>

#include "timers.h"
#include "LEDs.h"
#include "threads.h"
#include "gpio_defs.h"
#include "debug.h"

uint16_t * Reload_DMA_Source[2]={0,0};
uint32_t Reload_DMA_Byte_Count=0;
uint32_t DMA_Playback_Count=0;
uint8_t read_buffer_num=0;


void DMA_Init(void) {
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

void Configure_DMA_For_Playback(uint16_t * source1, uint16_t * source2, uint32_t count, uint32_t num_playbacks) {
	
	// Disable DMA channel in order to allow changes
	DMAMUX0->CHCFG[0] = 0;

	Reload_DMA_Source[0] = source1;
	Reload_DMA_Source[1] = source2;
	read_buffer_num = 0;

	Reload_DMA_Byte_Count = count*2;
	DMA_Playback_Count = num_playbacks;
	
	// Generate DMA interrupt when done
	// Increment source, transfer words (16 bits)
	// Enable peripheral request
	DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK | 
											DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) |
											DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;

	
	// Configure NVIC for DMA ISR
	NVIC_SetPriority(DMA0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(DMA0_IRQn); 
	NVIC_EnableIRQ(DMA0_IRQn);	

	// Enable DMA MUX channel without periodic triggering
	// select TPM0 overflow as trigger
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(54);   
}

void Start_DMA_Playback() {
	
	// Select TPM0 as trigger for DMA
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(54);   

	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source[read_buffer_num]);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));
	
	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(Reload_DMA_Byte_Count);
	
	// verify done flag is cleared
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; 
	
	// Enable DMA
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;

	// start the timer running
	TPM0_Start();
}

void DMA0_IRQHandler(void) {
	// Set debug signal
	PTB->PSOR = MASK(DBG_IRQDMA);

	// Clear done flag 
	DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK; 
	
	if (--DMA_Playback_Count > 0) { 
		// Signal event requesting source buffer refill
		osThreadFlagsSet(t_Refill_Sound_Buffer, EV_REFILL_SOUND_BUFFER);
#if USE_DOUBLE_BUFFER		
		// switch to other buffer
		read_buffer_num = 1 - read_buffer_num; 
#endif
		Control_RGB_LEDs(0,0,read_buffer_num);			
		// Start playback again
		Start_DMA_Playback();
	}
	
	// Clear debug signal
	PTB->PCOR = MASK(DBG_IRQDMA);
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
