#ifndef COLORS_H
#define COLORS_H
#include <stdint.h>

typedef struct {
	uint8_t R, G, B; // note: using 5-6-5 color mode for LCD. 
									 // Values are left aligned here
} COLOR_T;

extern COLOR_T black, 
	white,
	red,
	green,
	blue,
	yellow,
	cyan,
	magenta,
	dark_red,
	dark_green,
	dark_blue,
	dark_yellow,
	dark_cyan,
	dark_magenta,
	orange,
	light_gray, 
	dark_gray;

#endif // COLORS_H
