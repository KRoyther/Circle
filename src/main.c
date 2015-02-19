#include <pebble.h>
	
#define KEY_TEMPERATURE 0

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weather_layer;
static BitmapLayer *s_background_layer;
static BitmapLayer *s_weatherbg_layer;

static void update_time() {
	
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

	update_time();
	// Get weather update every 30 minutes
	if(tick_time->tm_min % 30 == 0) {
	  // Begin dictionary
	  DictionaryIterator *iter;
	  app_message_outbox_begin(&iter);

	  // Add a key-value pair
	  dict_write_uint8(iter, 0, 0);
	
	  // Send the message!
	  app_message_outbox_send();
	}

}

static void main_window_load(Window *window) {
	
	// Create GBitmap, then set to created BitmapLayer
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, gbitmap_create_with_resource(RESOURCE_ID_PEBBLE_BG));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	
	// Create GBitmap, then set to created BitmapLayer
	s_weatherbg_layer = bitmap_layer_create(GRect(91, 0, 53, 62));
	bitmap_layer_set_bitmap(s_weatherbg_layer, gbitmap_create_with_resource(RESOURCE_ID_WEATHER_BG));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weatherbg_layer));

	// Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 120, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_R_40)));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	// Create temperature Layer
	s_weather_layer = text_layer_create(GRect(100, 15, 36, 25));
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_text_color(s_weather_layer, GColorBlack);
	text_layer_set_font(s_weather_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_R_20)));
	text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
	text_layer_set_text(s_weather_layer, "○");
	
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
	
	// Make sure the time is displayed from the start
  update_time();

}

static void main_window_unload(Window *window) {

	// Destroy TextLayer
 	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_weather_layer);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
	bitmap_layer_destroy(s_weatherbg_layer);
	
}
	
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
	// Store incoming information
	static char temperature_buffer[16];
	
	// Read first item
  Tuple *t = dict_read_first(iterator);
	
	snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)t->value->int32);
	text_layer_set_text(s_weather_layer, temperature_buffer);
	
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	// Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
	
	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	// Make sure the time is displayed from the start
	update_time();
	
}

static void deinit() {
	
	// Destroy Window
  window_destroy(s_main_window);
	
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}