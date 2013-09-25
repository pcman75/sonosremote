
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "config.h"
#include "http.h"
#include "util.h"
	

                                
//#define MY_UUID { 0x2D, 0xA4, 0xE0, 0xE5, 0xC8, 0x16, 0x4F, 0x63, 0x95, 0xDA, 0x42, 0x8D, 0x75, 0xD3, 0xE0, 0x65 }

#define MY_UUID HTTP_UUID
PBL_APP_INFO_SIMPLE(MY_UUID, "Sonos Remote", "Cosmin", 1 /* App version */);


Window window;

TextLayer textLayer;

void debug(char* message, int status)
{
	static char debug_buf[20];
	strcpy(debug_buf, message);
	strcat(debug_buf, " ");
	strcat(debug_buf, itoa(status));
	text_layer_set_text(&textLayer, debug_buf);
}

void failed(int32_t cookie, int http_status, void* context) 
{
	debug("Failed", http_status);
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) 
{
	if(cookie != SONOSREMOTE_HTTP_COOKIE) 
		return;
	
	Tuple* data_tuple = dict_find(received, SONOSREMOTE_KEY_COMMAND);
	if(data_tuple) 
	{
		uint8_t command = data_tuple->value->uint8;;
		switch(command)
		{
			case SONOSREMOTE_CMD_PLAY:
				debug("Success Play", http_status);
				break;
			case SONOSREMOTE_CMD_STOP:
				debug("Success Stop", http_status);
				break;
			case SONOSREMOTE_CMD_PAUSE:
				debug("Success Pause", http_status);
				break;
			case SONOSREMOTE_CMD_PREV:
				debug("Success Prev", http_status);
				break;
			case SONOSREMOTE_CMD_NEXT:
				debug("Success Next", http_status);
				break;
			case SONOSREMOTE_CMD_INFO:
				debug("Success Info", http_status);
				break;
		}
	}
}

void reconnect(void* context) 
{
	debug("Reconnect", 0);
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) 
{
	text_layer_set_text(&textLayer, "Location");
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

  
	
  text_layer_set_text(&textLayer, "Select");
  
  // Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get(SONOSREMOTE_WEBSEVICE, SONOSREMOTE_HTTP_COOKIE, &body);
	if(result != HTTP_OK) 
	{
		debug("Err Get", result);
		return;
	}
	dict_write_int32(body, SONOSREMOTE_KEY_COMMAND, SONOSREMOTE_CMD_PLAY);
	
	// Send it.
	result = http_out_send();
	if( result != HTTP_OK) 
	{
		debug("Err Send", result);
		text_layer_set_text(&textLayer, "Err Send");
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
  
  http_set_app_id(SONOSREMOTE_APP_ID);
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
