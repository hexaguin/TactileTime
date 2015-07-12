#include <pebble.h>

Window *window;
TextLayer *hour_text_layer;
TextLayer *minute_text_layer;

void tap_handler(AccelAxisType axis, int32_t direction);

static void update_hour() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00";

  if(clock_is_24h_style()) {
    // Use 24 hour format
    strftime(buffer, sizeof("00"), "%H", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00"), "%I", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(hour_text_layer, buffer);
}

static void update_minute() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00";
  
  strftime(buffer, sizeof("00"), "%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(minute_text_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_hour();
  update_minute();
}

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
void tap_handler(AccelAxisType axis, int32_t direction) {
  vibe_time();
}


void handle_init(void) {
  window = window_create();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);

  hour_text_layer = text_layer_create(GRect(0, 0, 144, 84));
  text_layer_set_background_color(hour_text_layer, GColorBlack);
  text_layer_set_text_color(hour_text_layer, GColorWhite);
  text_layer_set_text(hour_text_layer, "00");
  text_layer_set_font(hour_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  
  minute_text_layer = text_layer_create(GRect(0, 84, 144, 84));
  text_layer_set_background_color(minute_text_layer, GColorBlack);
  text_layer_set_text_color(minute_text_layer, GColorWhite);
  text_layer_set_text(minute_text_layer, "00");
  text_layer_set_font(minute_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(minute_text_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hour_text_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minute_text_layer));
  
  update_hour();
  update_minute();
  
  window_stack_push(window, true);
}



void handle_deinit(void) {
  text_layer_destroy(hour_text_layer);
  text_layer_destroy(minute_text_layer);
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
