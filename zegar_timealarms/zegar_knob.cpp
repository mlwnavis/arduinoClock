// License
//
#include "zegar_knob.h"
#include "Arduino.h"
#include <TimeAlarms.h>

#include "simplehsm.h"

#define DEBUG
#ifdef DEBUG
#include "zegar_display.h"
#endif

#define PIN_A 3
#define PIN_B 4

#define KNOB_PORT 5
#define LONG_PRESS_TIME 3

static AlarmId press_timer_id = dtINVALID_ALARM_ID;
static int pinALast;
static bool time_set = false;
static time_t edit_time;


// Funkcja sygnalizująca długie naciśnięcie
static void long_press();

static void setLongPressTimeout() 
{
  press_timer_id = Alarm.timerOnce(LONG_PRESS_TIME, long_press);
  Alarm.enable(press_timer_id);
  Alarm.delay(0);
}

static void clearLongPressTimeout()
{
  Alarm.disable(press_timer_id);
  Alarm.free(press_timer_id);
  press_timer_id = dtINVALID_ALARM_ID;
  Alarm.delay(0);
}

static enum 
{
  SIG_CHECK = SIG_USER,
  SIG_KEY_PRESSED,
  SIG_KEY_RELEASED,
  SIG_LONG_PRESS_TIMEOUT,
  SIG_STEP_RIGHT,
  SIG_STEP_LEFT,
} key_press_t;

static void init_knob_hardware()
{
      pinMode(KNOB_PORT, INPUT_PULLUP);
      pinMode(PIN_A, INPUT);
      pinMode(PIN_B, INPUT);
      pinALast = digitalRead(PIN_A);
}

static simplehsm_t hsm = {NULL};

static stnext on(int signal, void *param);
static stnext edit_clock_down(int signal, void *param);
static stnext edit_clock_up(int signal, void *param);
static stnext clock_key_up(int signal, void *param);
static stnext clock_key_pressed(int signal, void *param);
static stnext clock_key_pressed_long(int signal, void *param);
static stnext edit_key_up(int signal, void *param);
static stnext edit_key_pressed(int signal, void *param);
static stnext edit_key_pressed_long(int signal, void *param);
static stnext clock_set(int signal, void *param); 
static stnext disp_temp(int signal, void *param);

static stnext on(int signal, void *param)
{
  switch (signal) {
    case SIG_INIT:
      init_knob_hardware();
      simplehsm_transition_state(&hsm, clock_key_up);
      return stnone;

    default:
      return stnone;
  }
}

static stnext clock_key_up(int signal, void *param) 
{
  #ifdef DEBUG
  debug_display("1000");
  #endif

  switch (signal) 
  {
    case SIG_KEY_PRESSED:
      simplehsm_transition_state(&hsm, clock_key_pressed);
      return stnone;
    
    case SIG_STEP_RIGHT:
    case SIG_STEP_LEFT:
    {
      handler_f handler = ((knob_handlers_t *) param)->long_to_edit;
      if (handler != NULL)
        handler();
      simplehsm_transition_state(&hsm, disp_temp);
      return stnone;
    }
    
    default:
      return stnone;
  }
}

static stnext clock_key_pressed(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("1100");
  #endif
  switch (signal) 
  {
    case SIG_ENTRY:
      setLongPressTimeout();
      return stnone;

    case SIG_KEY_RELEASED:
    {
      clearLongPressTimeout();

      simplehsm_transition_state(&hsm, clock_key_up);
      return stnone;
    }

    case SIG_LONG_PRESS_TIMEOUT:
      simplehsm_transition_state(&hsm, clock_key_pressed_long);
      return stnone;

    default:
      return stnone;
  }
}

static stnext clock_key_pressed_long(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("1110");
  #endif
  switch (signal)
  {
    case SIG_ENTRY:
      clearLongPressTimeout();
      return stnone;

    case SIG_KEY_RELEASED:
    {
      handler_f handler = ((knob_handlers_t *) param)->long_to_edit;
      if (handler != NULL)
        handler();

      simplehsm_transition_state(&hsm, edit_key_up);
      return stnone;
    }

    default:
      return stnone;
  }
}

static stnext edit_key_up(int signal, void *param) 
{
  #ifdef DEBUG
  debug_display("2000");
  #endif

  
  switch (signal) 
  {
    case SIG_ENTRY:
    {
      time_set = false;
      edit_time = now();
      return stnone;
    }
    case SIG_KEY_PRESSED:
      simplehsm_transition_state(&hsm, edit_key_pressed);
      return stnone;

    case SIG_STEP_RIGHT:
     //simplehsm_transition_state(&hsm, edit_clock_up);
      time_set = true;
      edit_time -= 60;
      display_time(hour(edit_time), minute(edit_time), second(edit_time));
      return stnone;
    
    case SIG_STEP_LEFT:
      //simplehsm_transition_state(&hsm, edit_clock_down);
      time_set = true;
      edit_time += 60;
      display_time(hour(edit_time), minute(edit_time), second(edit_time));
      
      return stnone;

      
    default:
      return stnone;
  }
}

static stnext edit_clock_down(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("2100");
  #endif
  switch(signal)
  {
    default:
      return stnone;
  }
}

static stnext edit_clock_up(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("2200");
  #endif
  switch(signal)
  {
    default:
      return stnone;
  }
}

static stnext edit_key_pressed(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("3000");
  #endif
  switch (signal) 
  {
    case SIG_ENTRY:
      setLongPressTimeout();
      return stnone;

    case SIG_KEY_RELEASED:
    {
      clearLongPressTimeout();

      if (time_set) 
        setTime(hour(edit_time), minute(edit_time), 0, day(edit_time), month(edit_time), year(edit_time));

      handler_f handler = ((knob_handlers_t *) param)->long_from_edit;
      if (handler != NULL)
        handler();
      simplehsm_transition_state(&hsm, clock_key_up);
      return stnone;
    }

    case SIG_LONG_PRESS_TIMEOUT:
      simplehsm_transition_state(&hsm, edit_key_pressed_long);
      return stnone;

    default:
      return stnone;
  }
}

static stnext edit_key_pressed_long(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("3300");
  #endif
  switch (signal)
  {
    case SIG_ENTRY:
      clearLongPressTimeout();
      return stnone;

    case SIG_KEY_RELEASED:
    {
      handler_f handler = ((knob_handlers_t *) param)->long_from_edit;
      if (handler != NULL)
        handler();

      simplehsm_transition_state(&hsm, clock_key_up);
      return stnone;
    }

    default:
      return stnone;
  }
}

static stnext clock_set(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("3330");
  #endif
  
  switch(signal)
  {
    case SIG_EXIT:
      return stnone;
      
    default:
      return stnone;
  }
} 

static stnext disp_temp(int signal, void *param)
{
  #ifdef DEBUG
  debug_display("4000");
  #endif
  switch(signal){
    case SIG_ENTRY:
        display_reset();
        return stnone;
        
    case SIG_STEP_LEFT:
    case SIG_STEP_RIGHT:
    {
      handler_f handler = ((knob_handlers_t *) param)->long_from_edit;
      if (handler != NULL)
        handler();
      simplehsm_transition_state(&hsm, clock_key_up);
      return stnone;
    }
    
    default:
      return stnone;
    }
}

static void long_press()
{
  simplehsm_signal_current_state(&hsm, SIG_LONG_PRESS_TIMEOUT, NULL); 
}

void knob_init()
{
  simplehsm_init_transition_state(&hsm, on);
}

void knob_check(knob_handlers_t handlers) 
{
  int aVal = digitalRead(PIN_A);

  if (aVal != pinALast)
  {
      if (digitalRead(PIN_B) != aVal)
      {
        simplehsm_signal_current_state(&hsm, SIG_STEP_RIGHT, (void *)&handlers);
      }
      else
      {
        simplehsm_signal_current_state(&hsm, SIG_STEP_LEFT, (void *)&handlers);
      }
  }
  else if (digitalRead(KNOB_PORT) == LOW)
    simplehsm_signal_current_state(&hsm, SIG_KEY_PRESSED, (void *)&handlers);
  else
  {
    simplehsm_signal_current_state(&hsm, SIG_KEY_RELEASED, (void *)&handlers);
  }
}

/*
static volatile bool long_press_detected = false;

static void long_press()
{
  long_press_detected = true;
}

void knob_init()
{
    pinMode(KNOB_PORT, INPUT_PULLUP);
}

KeyPress_t knob_pressed()
{
  static AlarmId press_timer_id = dtINVALID_ALARM_ID;
  
  if (press_timer_id == dtINVALID_ALARM_ID) // before key down
  {
    if (digitalRead(KNOB_PORT) == LOW) // key down
    {
      long_press_detected = false;
      press_timer_id = Alarm.timerOnce(LONG_PRESS_TIME, long_press);
    }
    return NonePress;
  }
  else // after key down
  {  
     if (digitalRead(KNOB_PORT) == HIGH) // key up
     {
        Alarm.free(press_timer_id);
        press_timer_id = dtINVALID_ALARM_ID;

        if (long_press_detected)
          return LongPress;
        else
          return ShortPress;
     }
     else
        return NonePress;
  }
}
*/

