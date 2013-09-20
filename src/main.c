
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "util.h"
	
#define SONOSREMOTE_HTTP_COOKIE 726128123
#define SONOSREMOTE_KEY_COMMAND 1 

#define SONOSREMOTE_KEY_PLAY 1
                                
#define MY_UUID { 0x2D, 0xA4, 0xE0, 0xE5, 0xC8, 0x16, 0x4F, 0x63, 0x95, 0xDA, 0x42, 0x8D, 0x75, 0xD3, 0xE0, 0x65 }
PBL_APP_INFO_SIMPLE(MY_UUID, "Sonos Remote", "Cosmin", 1 /* App version */);


Window window;

TextLayer textLayer;



void failed(int32_t cookie, int http_status, void* context) 
{
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) 
{
	text_layer_set_text(&textLayer, "Success Play");
}

void reconnect(void* context) 
{
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) 
{
}

// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  text_layer_set_text(&textLayer, "Up");
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  text_layer_set_text(&textLayer, "Down");

}


void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  static char buf[20];
	
  text_layer_set_text(&textLayer, "Select");
  
  // Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get("http://sonospebbleremote.appspot.com", SONOSREMOTE_HTTP_COOKIE, &body);
	if(result != HTTP_OK) 
	{
		strcpy(buf, "Err Select ");
		strcat(buf, itoa(result));
		text_layer_set_text(&textLayer, buf);
		return;
	}
	dict_write_int32(body, SONOSREMOTE_KEY_COMMAND, SONOSREMOTE_KEY_PLAY);
	
	// Send it.
	if(http_out_send() != HTTP_OK) 
	{
		text_layer_set_text(&textLayer, "Error Send");
		return;
	}
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  text_layer_set_text(&textLayer, "Select Long");
}


// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}


// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Button App");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&textLayer, window.layer.frame);
  text_layer_set_text(&textLayer, "Hello World");
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  layer_add_child(&window.layer, &textLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
  
  http_register_callbacks((HTTPCallbacks){
		.failure=failed,
		.success=success,
		.reconnect=reconnect,
		.location=location
	}, (void*)ctx);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
	}
		
  };
  app_event_loop(params, &handlers);
}
