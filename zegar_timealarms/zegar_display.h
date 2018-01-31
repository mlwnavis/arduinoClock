// <License>
//
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define DEBUG

void display_init();

void display_reset();

void display_time(int h, int m, int s);

#ifdef DEBUG
void debug_display(char*);
#endif
