#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "config.h"
#include "http.h"
#include "util.h"
#include "debug.h"

#define MY_UUID HTTP_UUID
PBL_APP_INFO(MY_UUID, "Sonos Remote", "Cosmin", 1, 0, RESOURCE_ID_SONOS_APP_ICON, APP_INFO_STANDARD_APP);

typedef struct
{
	bool mute;
	bool playing;
} SonosStatus;

SonosStatus sonosStatus = {.mute = false, .playing = false};

Window window;
TextLayer textLayer;
ActionBarLayer action_bar;

void failed(int32_t cookie, int http_status, void* context) 
{
	debug("Failed", http_status);
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) 
{
	if(cookie != SONOSREMOTE_HTTP_COOKIE)
	{
		debug("Success not mine", http_status);
		return;
	}
	
	Tuple* data_tuple = dict_find(received, SONOSREMOTE_KEY_COMMAND);
		
	if(data_tuple) 
	{
		uint8_t command = data_tuple->value->uint8;
		
		data_tuple = dict_find(received, SONOSREMOTE_KEY_COMMAND);
		if(data_tuple)
		{
			uint8_t commandSuccess = data_tuple->value->uint8;
			if(commandSuccess)
			{
				switch(command)
				{
					case SONOSREMOTE_CMD_PLAY:
						debug("Success Play", http_status);
						sonosStatus.playing = true;
						break;
					case SONOSREMOTE_CMD_STOP:
						debug("Success Stop", http_status);
						sonosStatus.playing = false;
						break;
					case SONOSREMOTE_CMD_PAUSE:
						debug("Success Pause", http_status);
						sonosStatus.playing = false;			
						break;
					case SONOSREMOTE_CMD_PREV:
						debug("Success Prev", http_status);
						break;
					case SONOSREMOTE_CMD_NEXT:
						debug("Success Next", http_status);
						break;
					case SONOSREMOTE_CMD_INFO:
						data_tuple = dict_find(received, SONOSREMOTE_KEY_TRACK_INFO_TITLE);
						if(data_tuple)
						{
							char* title = data_tuple->value->cstring;
							text_layer_set_text(&textLayer, title);
						}
						break;
					default:
						debug("Unkown", command);
				}
			}
		}
	}
	else
		debug("Not found", 0);
}

void reconnect(void* context) 
{
	debug("Reconnect", 0);
}

void send_sonos_command(int command)
{
	// Build the HTTP request
	DictionaryIterator *body;
	static char sonosWebService[60];
	strcpy(sonosWebService, SONOSREMOTE_WEBSEVICE);
	strcat(sonosWebService, "/");
	strcat(sonosWebService, itoa(rand()));
	HTTPResult result = http_out_get(sonosWebService, SONOSREMOTE_HTTP_COOKIE, &body);
	if(result == HTTP_OK) 
	{
		dict_write_uint8(body, SONOSREMOTE_KEY_COMMAND, command);
		// Send it.
		result = http_out_send();
		if( result != HTTP_OK) 
		{
			debug("Err Send", result);
		}
	}
	else
	{
		debug("Err Get", result);
	}
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{
	send_sonos_command(SONOSREMOTE_CMD_PREV);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{
	send_sonos_command(SONOSREMOTE_CMD_NEXT);	
	//send_test();	
}


void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{
	if(sonosStatus.playing)
		send_sonos_command(SONOSREMOTE_CMD_PAUSE);		
	else
		send_sonos_command(SONOSREMOTE_CMD_PLAY);
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) 
{
	send_sonos_command(SONOSREMOTE_CMD_INFO);
}


// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) 
{
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}


// WindowHandlers
void handle_main_appear(Window *window)
{
    // We need to add the action_bar when the main-window appears. If we do this in handle_init it picks up wrong window-bounds and the size doesn't fit.
    action_bar_layer_add_to_window(&action_bar, window);
}

void handle_main_disappear(Window *window)
{
    // Since we add the layer on each appear, we remove it on each disappear.
    action_bar_layer_remove_from_window(&action_bar);
}

// Standard app initialisation

void handle_init(AppContextRef ctx) 
{
  window_init(&window, "Sonos");
  
  // Init resources
  resource_init_current_app(&APP_RESOURCES);
	
  window_set_window_handlers(&window, (WindowHandlers) 
  {
        .appear = (WindowHandler)handle_main_appear,
        .disappear = (WindowHandler)handle_main_disappear
  });
  
  // Init resources
  resource_init_current_app(&APP_RESOURCES);
    
  // Load some bitmaps 
  //heap_bitmap_init(&button_image_up, RESOURCE_ID_IMAGE_BUTTON_UP);
  //heap_bitmap_init(&button_image_down, RESOURCE_ID_IMAGE_BUTTON_DOWN);
  //heap_bitmap_init(&button_image_setup, RESOURCE_ID_IMAGE_BUTTON_SETUP);
  
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&textLayer, window.layer.frame);
  debug("Sonos", 0);
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(&window.layer, &textLayer.layer);
  // Attach our desired button functionality
  //window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

  // Initialize the action bar:
  action_bar_layer_init(&action_bar);
  // Associate the action bar with the window:
  //action_bar_layer_add_to_window(&action_bar, &window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(&action_bar,
                                             (ClickConfigProvider) click_config_provider);
  // Set the icons:
  // The loading the icons is omitted for brevity... See HeapBitmap.
  //action_bar_layer_set_icon(&action_bar, BUTTON_ID_UP, &my_icon_previous);
  //action_bar_layer_set_icon(&action_bar, BUTTON_ID_DOWN, &my_icon_next);
  layer_add_child(&window.layer, &action_bar.layer);
  
  http_set_app_id(SONOSREMOTE_APP_ID);
  http_register_callbacks((HTTPCallbacks){
		.failure=failed,
		.success=success,
		.reconnect=reconnect
	}, (void*)ctx);
	
	srand(time(NULL));
	send_sonos_command(SONOSREMOTE_CMD_INFO);
}


void pbl_main(void *params) 
{
  PebbleAppHandlers handlers = 
  {
    .init_handler = &handle_init,
	.messaging_info = 
	{
			.buffer_sizes = 
			{
				.inbound = 124,
				.outbound = 256,
			}
	}
  };
  app_event_loop(params, &handlers);
}
