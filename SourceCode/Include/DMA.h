#ifndef DMA_H
#define DMA_H

#include <stdint.h>

void DMA_Init(void);
void Configure_DMA_For_Playback(uint16_t * source1, uint16_t * source2, uint32_t count, uint32_t num_playbacks);
void Start_DMA_Playback(void);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
