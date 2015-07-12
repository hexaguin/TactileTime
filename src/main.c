#include <pebble.h>
#include <vibeFunctions.c>

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

void tap_handler(AccelAxisType axis, int32_t direction) {
  vibe_time();
}

void handle_init(void) {
  window = window_create();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);

  hour_text_layer = text_layer_create(GRect(0, 0, 144, 107));
  text_layer_set_background_color(hour_text_layer, GColorBlack);
  text_layer_set_text_color(hour_text_layer, GColorWhite);
  text_layer_set_text(hour_text_layer, "00");
  text_layer_set_font(hour_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49)); //TODO: Make the numbers prettier. Maybe use custom font/rendering?
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  
  minute_text_layer = text_layer_create(GRect(0, 107, 144, 61));
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
