#include <pebble.h>
	
#define KEY_TEMPERATURE 0

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
//static TextLayer *s_weather_layer;
static TextLayer *s_batttext_layer;
static BitmapLayer *s_background_layer;
//static BitmapLayer *s_weatherbg_layer;
static BitmapLayer *s_disconnected_layer;
static BitmapLayer *s_battery_layer;
static BitmapLayer *s_ischarging_layer;

static void update_time() {
	
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char time_buffer[] = "00:00";
	static char date_buffer[] = "Xxx 00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display time on the TextLayer
  text_layer_set_text(s_time_layer, time_buffer);
	
	// Getting weekday to write along date
	int wday_flag = tick_time -> tm_wday;
	
	if			 (wday_flag == 0) {strftime(date_buffer, sizeof(date_buffer), "Sun %d", tick_time);}
	else if  (wday_flag == 1) {strftime(date_buffer, sizeof(date_buffer), "Mon %d", tick_time);}
	else if  (wday_flag == 2) {strftime(date_buffer, sizeof(date_buffer), "Tue %d", tick_time);}
	else if  (wday_flag == 3) {strftime(date_buffer, sizeof(date_buffer), "Wed %d", tick_time);}
	else if  (wday_flag == 4) {strftime(date_buffer, sizeof(date_buffer), "Thu %d", tick_time);}
	else if  (wday_flag == 5) {strftime(date_buffer, sizeof(date_buffer), "Fri %d", tick_time);}
	else if  (wday_flag == 6) {strftime(date_buffer, sizeof(date_buffer), "Sat %d", tick_time);}
	
	// Writing date on the TextLayer
	text_layer_set_text(s_date_layer, date_buffer);
	
	//if (strcmp(text_layer_get_text(s_weather_layer), "") == 0){
	//	if (bitmap_layer_get_bitmap(s_weatherbg_layer) == gbitmap_create_with_resource(RESOURCE_ID_WEATHER_BG)){
	//		layer_set_hidden(bitmap_layer_get_layer(s_weatherbg_layer), true);
	//	}else{
	//		layer_set_hidden(bitmap_layer_get_layer(s_weatherbg_layer), false);
	//	}
	//}else{
	//	layer_set_hidden(bitmap_layer_get_layer(s_weatherbg_layer), false);
	//}
	
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

	update_time();
		
	// Get weather update every 30 minutes
	//if(tick_time->tm_min % 30 == 0) {
	//  // Begin dictionary
	//  DictionaryIterator *iter;
	//  app_message_outbox_begin(&iter);

	// Add a key-value pair
	//  dict_write_uint8(iter, 0, 0);
	
	// Send the message!
	//  app_message_outbox_send();
	//}
		
}

// Verify bluetooth connection
static void handle_bluetooth(bool connected) {
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth handling function opened");
	if	(connected) {
		layer_set_hidden(bitmap_layer_get_layer(s_disconnected_layer), true);
		APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth connected");
	}	else {
		layer_set_hidden(bitmap_layer_get_layer(s_disconnected_layer), false);
		vibes_short_pulse();
		APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth disconnected");
	}
	APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth handling function finished");
	
}

void handle_battery(BatteryChargeState charge) {
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Battery handling function opened");
	bitmap_layer_destroy(s_battery_layer);
	s_battery_layer = bitmap_layer_create(GRect(5, 5, 116, 116));
	
	// Write battery % to TextLayer:
	static char battery_buffer[32];
	snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", charge.charge_percent);
	text_layer_set_text(s_batttext_layer, battery_buffer);
	
	if (charge.charge_percent == 100){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_100));}
			else if (charge.charge_percent > 90){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_90));}
			else if (charge.charge_percent > 80){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_80));}
			else if (charge.charge_percent > 70){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_70));}
			else if (charge.charge_percent > 60){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_60));}
			else if (charge.charge_percent > 50){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_50));}
			else if (charge.charge_percent > 40){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_40));}
			else if (charge.charge_percent > 30){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_30));}
			else if (charge.charge_percent > 20){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_20));}
			else if (charge.charge_percent > 10){
		        bitmap_layer_set_bitmap(s_battery_layer, gbitmap_create_with_resource(RESOURCE_ID_BATT_10));}
	
	if (charge.is_charging){
		
		layer_set_hidden(bitmap_layer_get_layer(s_ischarging_layer), false);
		layer_set_hidden(text_layer_get_layer(s_batttext_layer), true);
		
	}

	else if (!charge.is_charging){
		
		layer_set_hidden(bitmap_layer_get_layer(s_ischarging_layer), true);
		layer_set_hidden(text_layer_get_layer(s_batttext_layer), false);
		
	}
	APP_LOG(APP_LOG_LEVEL_INFO, "Battery handling function finished");
	
}

static void main_window_load(Window *window) {
	
	// Creating background bitmap layer
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, gbitmap_create_with_resource(RESOURCE_ID_PEBBLE_BG));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	
	// Creating weather background bitmap layer
	//s_weatherbg_layer = bitmap_layer_create(GRect(91, 0, 53, 62));
	//bitmap_layer_set_bitmap(s_weatherbg_layer, gbitmap_create_with_resource(RESOURCE_ID_WEATHER_BG));
	//layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weatherbg_layer));
	
	// Creating disconnected icon
	s_disconnected_layer = bitmap_layer_create(GRect(100, -5, 53, 62));
	bitmap_layer_set_bitmap(s_disconnected_layer, gbitmap_create_with_resource(RESOURCE_ID_DISCONNECTED));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_disconnected_layer));
	
	// Creating battery bitmap layer
	s_battery_layer = bitmap_layer_create(GRect(5, 5, 116, 116));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_layer));
	
	// Creating charging bitmap layer
	s_ischarging_layer = bitmap_layer_create(GRect(111, 130, 10, 20));
	bitmap_layer_set_bitmap(s_ischarging_layer, gbitmap_create_with_resource(RESOURCE_ID_CHARGING));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_ischarging_layer));

	// Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 120, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_R_40)));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	// Create date Layer
	s_date_layer = text_layer_create(GRect(0, 100, 120, 25));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorBlack);
	text_layer_set_font(s_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_R_20)));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	
	// Create temperature Layer
	//s_weather_layer = text_layer_create(GRect(100, 15, 40, 25));
	//text_layer_set_background_color(s_weather_layer, GColorClear);
	//text_layer_set_text_color(s_weather_layer, GColorBlack);
	//text_layer_set_font(s_weather_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_R_20)));
	//text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
	//text_layer_set_text(s_weather_layer, "");
	//layer_set_hidden(text_layer_get_layer(s_weather_layer), true);
	
	// Add it as a child layer to the Window's root layer
	//layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
	
	// Create battery percentage Layer
	s_batttext_layer = text_layer_create(GRect(90, 130, 55, 25));
	text_layer_set_background_color(s_batttext_layer, GColorClear);
	text_layer_set_text_color(s_batttext_layer, GColorBlack);
	text_layer_set_font(s_batttext_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_R_20)));
	text_layer_set_text_alignment(s_batttext_layer, GTextAlignmentCenter);
	
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_batttext_layer));
	
	handle_battery(battery_state_service_peek());
	battery_state_service_subscribe(&handle_battery);

	handle_bluetooth(bluetooth_connection_service_peek());
	bluetooth_connection_service_subscribe(&handle_bluetooth);
	
	// Make sure the time is displayed from the start
  update_time();

}

static void main_window_unload(Window *window) {

	// Destroy TextLayer
 	text_layer_destroy(s_time_layer);
	//text_layer_destroy(s_weather_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_batttext_layer);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
	//bitmap_layer_destroy(s_weatherbg_layer);
	bitmap_layer_destroy(s_battery_layer);
	bitmap_layer_destroy(s_ischarging_layer);
	bitmap_layer_destroy(s_disconnected_layer);
	
}
	
//static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
	// Store incoming information
	//static char temperature_buffer[32];
	
	// Read first item
  //Tuple *t = dict_read_first(iterator);
	
	//snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)t->value->int32);
	//text_layer_set_text(s_weather_layer, temperature_buffer);
	
//}

//static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
//}

//static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
//}

//static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
//}

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
	//app_message_register_inbox_received(inbox_received_callback);
	//app_message_register_inbox_dropped(inbox_dropped_callback);
	//app_message_register_outbox_failed(outbox_failed_callback);
	//app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	//app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
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