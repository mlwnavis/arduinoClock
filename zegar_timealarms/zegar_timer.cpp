// <License>
//
#include "zegar_timer.h"
#include "zegar_display.h"

AlarmId timerAlarmId = 0;
bool timer_status=true;

void timer_init(){
  // setTime(hr,min,sec,day,month,yr)
  setTime(0,0,0,1,1,18); // set time to 00:00:00 Jan 1 2018
  timerAlarmId = Alarm.timerRepeat(1, timer_tick);
  timer_status = true; 
}

void timer_tick(){
  time_t t = now();
  display_time(hour(t), minute(t), second(t));
}

void timer_enable(){
  Alarm.enable(timerAlarmId);
  timer_status = true;
}

void timer_disable(){
  Alarm.disable(timerAlarmId);
  timer_status=false;
}

void timer_toggle(){
  if (timer_status)
    timer_disable();
  else 
    timer_enable();
  
}

