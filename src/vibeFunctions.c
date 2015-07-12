#include <pebble.h>

void tap_handler(AccelAxisType axis, int32_t direction);

static void vibe_digit(int d) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "buzzing %d", d);
  if(d==0){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "buzz 0");
    uint32_t segments[] = { 300, 200};
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
    };
    vibes_enqueue_custom_pattern(pat);
    psleep(800);
  } else {
    uint32_t segments[] = { 150, 200};
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
    };
    for(int i = 0; i<d; i++) {
      vibes_enqueue_custom_pattern(pat);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "buzz");
      psleep(350);
    }
    psleep(300);
  }
  
  psleep(400);
}

static void vibe_time() {
  accel_tap_service_unsubscribe();
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int hour = tick_time->tm_hour;
  int minute = tick_time->tm_min;
  
  if(clock_is_24h_style()==false) {
    if(hour==0){ //start converting to 12 hour time
      hour = 12;
    } else if(hour>12) {
      hour-=12;
    }
  }
  
  int min1 = minute/10; //digit values. 1 is the first digit, 2 is second
  int min2 = minute%10;
  int hour1 = hour/10; 
  int hour2 = hour%10;
  
  vibe_digit(hour1);
  vibe_digit(hour2);
  psleep(150);
  vibe_digit(min1);
  vibe_digit(min2);

  psleep(1000);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "done buzzing time");
  accel_tap_service_subscribe(tap_handler);
}