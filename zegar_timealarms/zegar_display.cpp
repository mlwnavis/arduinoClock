// <License>
//
#include "zegar_display.h"

LiquidCrystal_I2C lcd(0x27,16,2);

const char * TIME_FORMAT_STR = ".%c..%02d:%02d:%02d....";
const int LINE_LEN = 16;

void display_init() {
  lcd.init(); 
  lcd.backlight();
}

void display_reset() {
  lcd.clear();
}

void  display_time(int h, int m, int s){
  static const int BUFF_SIZE = LINE_LEN + 1;
  static char t_str[BUFF_SIZE];
  
  snprintf(t_str, BUFF_SIZE, TIME_FORMAT_STR,' ', h, m, s);
  lcd.setCursor(0,0); // set the cursor to column 15, line 0
  lcd.print(t_str);
}

#ifdef DEBUG
const char * DEBUG_STR =  "%s";

void debug_display(char* s)
{
  static const int BUFF_SIZE = LINE_LEN + 1;
  static char t_str[BUFF_SIZE];
  
  snprintf(t_str, BUFF_SIZE, DEBUG_STR, s);
  lcd.setCursor(0,1); // set the cursor to column 15, line 0
  lcd.print(t_str);
  
}
#endif
