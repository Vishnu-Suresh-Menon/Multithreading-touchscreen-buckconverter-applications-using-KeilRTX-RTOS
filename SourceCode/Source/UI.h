#ifndef UI_H
#define UI_H
#include "LCD.h"

// Definitions
#define UI_LABEL_LEN (16)
#define UI_UNITS_LEN (4)

// Structures
typedef struct sUI_FIELD_T UI_FIELD_T;
typedef struct sUI_FIELD_T {
	char Label[UI_LABEL_LEN]; 
	char Units[UI_UNITS_LEN];
	char Buffer[2*UI_LABEL_LEN]; // Holds latest field text: Label Val Units
	volatile int * Val;
	volatile char * ValT; // Unused. Future expansion for text values.
	PT_T RC; // Starting row (Y) and column (X) of field
	COLOR_T * ColorFG, * ColorBG;
	char Updated, Selected, ReadOnly, Volatile;
	void (*Handler)(UI_FIELD_T * fld, int v); // Handler function to change value based on slider pos v
} UI_FIELD_T ;

typedef struct  {
	int Val; // Is 0 when touched at horizontal middle 
	PT_T UL, LR;
	PT_T BarUL, BarLR;
	COLOR_T * ColorFG, * ColorBG, * ColorBorder;
} UI_SLIDER_T;

#define UI_NUM_FIELDS (sizeof(Fields)/sizeof(UI_FIELD_T))
#define UI_SLIDER (100)

#define UI_SLIDER_HEIGHT 		(25)
#define UI_SLIDER_WIDTH 		(LCD_WIDTH)
#define UI_SLIDER_BAR_WIDTH (8)

// Function prototypes
void UI_Draw_Screen(int first_time);
int UI_Identify_Control(PT_T * p);
void UI_Process_Touch(PT_T * p);

void UI_Update_Field_Values (UI_FIELD_T * f, int num);
void UI_Draw_Fields(UI_FIELD_T * f, int num);
void graphical_current_plot();

#endif // UI_H
