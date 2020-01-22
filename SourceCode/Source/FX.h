#ifndef FX_H
#define FX_H
#include <stdint.h>

typedef int32_t FX16_16;

#define FL_TO_FX(x)	((FX16_16)((x)*65536.0))
#define INT_TO_FX(x) ((FX16_16)((x)*65536))
#define FX_TO_INT(x) ((int32_t)((x)/65536))
#define FX_TO_FL(x) ((float)((x)/65536.0))

FX16_16 Multiply_FX(FX16_16 a, FX16_16 b);
FX16_16 Add_FX(FX16_16 a, FX16_16 b);
FX16_16 Subtract_FX(FX16_16 a, FX16_16 b);


#endif // FX_H
