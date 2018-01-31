// <License>
//
#define DEBUG
#include "zegar_timer.h"
#include "zegar_display.h"
#include "zegar_knob.h"

void setup() 
{
  knob_init();
  display_init();
  timer_init();
}

//                               Short         Long 
// knob_handlers_t knob_handlers = {NULL, timer_toggle};

knob_handlers_t knob_handlers = {NULL, timer_toggle, NULL, timer_toggle};


void loop() 
{
  Alarm.delay(0);

  knob_check(knob_handlers);

/*  switch (knob_pressed()){
    case ShortPress:
      timer_toggle();
      break;
    case LongPress:
    case NonePress:
    default:
      break; 
  } */
}

