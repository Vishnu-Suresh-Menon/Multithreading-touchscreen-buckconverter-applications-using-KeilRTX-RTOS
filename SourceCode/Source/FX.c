#include "FX.h"

FX16_16 Multiply_FX(FX16_16 a, FX16_16 b) {
	int64_t p, pa, pb;
	// Long multiply first. 
	pa = a;
	pb = b;
	p = pa * pb;

	// Check for overflow
	
	// normalize after multiplication
	// 	p /= 65536; // Too slow
	
	p >>= 16;
	
//	if (p>0)
//		p >>= 16;
//	else {
//		p >>= 16;
//		p = 0xffffffffffffffff-p;
//	}

	return (FX16_16)(p&0xffffffff);
}

FX16_16 Add_FX(FX16_16 a, FX16_16 b) {
	FX16_16 p;
	// Add. This will overflow if a+b > 2^16
	p = a + b;
	return p;
}

FX16_16 Subtract_FX(FX16_16 a, FX16_16 b) {
	FX16_16 p;
	p = a - b;
	return p;
}

void Test_FX(void) {
#if 0
	FX16_16 a, b, c;
	
	a = INT_TO_FX(16);
	b = INT_TO_FX(-16);
	c = Multiply_FX(a,b); // negative 256

	a = INT_TO_FX(16);
	b = INT_TO_FX(16);
	c = Multiply_FX(a,b);	// positive 256
#endif
}
