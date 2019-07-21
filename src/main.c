/* javaScriptClock v1.1 by ryanpcmcquen */
//
// Ryan P. C. McQuen | Everett, WA | ryanpcmcquen@member.fsf.org

#include <pebble.h>

static Window *s_main_window;

// set up a text layer for our time
// and our fake JS environment
static TextLayer *s_js_env;
static TextLayer *s_time_layer;

// declare variables for our custom font
static GFont s_js_font;
static GFont s_time_font;

static void update_time() {
  // get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // write the current hours and minutes into a buffer
  static char s_buffer[16];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "< \"%H:%M\"" : "< \"%I:%M\"", tick_time);
  
  // display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();  
}

static void main_window_load(Window *window) {
  // get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // assign custom font variables
  s_js_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_MONO_BOLD_14));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_MONO_BOLD_29));

  // create the TextLayer with specific bounds
  s_js_env = text_layer_create(
    GRect(5, 30, bounds.size.w, bounds.size.h)
  );
  
  // set up the fake JS environment
  text_layer_set_background_color(s_js_env, GColorClear);
  text_layer_set_text_color(s_js_env, GColorBlack);
  text_layer_set_text(s_js_env,
    "> jsClock.version;\n"
    "< 1.1\n"
    "> String(new Date\n"
    "   ).slice(16,21);"
  );
  text_layer_set_font(s_js_env, s_js_font);
  text_layer_set_text_alignment(s_js_env, GTextAlignmentLeft);
  
  // create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(5, 100, bounds.size.w, bounds.size.h)
  );
  
  // add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_js_env));

  // set up the watchface layout
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  
  // add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // destroy TextLayer(s)
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_js_env);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_js_font);
}


static void init() {
  // create main Window element and assign to pointer
  s_main_window = window_create();
  
  // set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // make sure the time is displayed from the start
  update_time();
  
  // register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
