
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "config.h"
#include "http.h"
#include "util.h"
	

                                
//#define MY_UUID { 0x2D, 0xA4, 0xE0, 0xE5, 0xC8, 0x16, 0x4F, 0x63, 0x95, 0xDA, 0x42, 0x8D, 0x75, 0xD3, 0xE0, 0x65 }

#define MY_UUID HTTP_UUID
PBL_APP_INFO_SIMPLE(MY_UUID, "Sonos Remote", "Cosmin", 1 /* App version */);

typedef struct
{
	bool mute;
	bool playing;
} SonosStatus;

SonosStatus sonosStatus = {.mute = false, .playing = false};

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

void dump_dict(DictionaryIterator* iter)
{
	static char buf[500];
	static char row[50];
	
	buf[0] = 0;
	
	Tuple *tuple = dict_read_first(iter);
	while (tuple) 
	{	
		unsigned int uvalue = 0;
  		if(tuple->type == TUPLE_UINT)
		{			 
			if(tuple->length == 1)
			{
				uvalue = tuple->value->uint8;
			}
			else if(tuple->length == 2)
			{
				uvalue = tuple->value->uint16;
			}
			else if(tuple->length == 4)
			{
				uvalue = tuple->value->uint32;
			}
			snprintf(row, 50, "[%u,%u]", (unsigned)tuple->key, uvalue);
		}
		else if (tuple->type == TUPLE_INT)
		{
			int value = 0;
			if(tuple->length == 1)
			{
				value = tuple->value->int8;
			}
			else if(tuple->length == 2)
			{
				value = tuple->value->int16;
			}
			else if(tuple->length == 4)
			{
				value = tuple->value->int32;
			}
			snprintf(row, 50, "[%u,%d]", (unsigned)tuple->key, value);
		}
		else if (tuple->type == TUPLE_CSTRING)
		{
			snprintf(row, 50, "[%u,%s]", (unsigned)tuple->key, tuple->value->cstring);
		}
		
		strcat(buf, row);
		tuple = dict_read_next(iter);
	}
	//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "dump_dict");
	text_layer_set_text(&textLayer, buf);
	
}

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
	
	//Tuple* data_tuple = dict_find(received, SONOSREMOTE_KEY_COMMAND);
	//Tuple* data_tuple = dict_read_first(received);
	dump_dict(received);
	
	/*
	if(data_tuple) 
	{
		uint8_t command = data_tuple->value->uint8;
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
				debug("Success Info", http_status);
				break;
			default:
				debug("Unkown", command);
		}
	}
	
	else
		debug("Not found", 0);
	*/
}

void reconnect(void* context) 
{
	debug("Reconnect", 0);
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) 
{
	text_layer_set_text(&textLayer, "Location");
}

void send_sonos_command(int command)
{
	// Build the HTTP request
	DictionaryIterator *body;
	char sonosWebService[60];
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
	send_sonos_command(SONOSREMOTE_CMD_MUTE);
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

void handle_init(AppContextRef ctx) 
{
  (void)ctx;

  window_init(&window, "Button App");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&textLayer, window.layer.frame);
  debug("Sonos", HTTP_NOT_ENOUGH_STORAGE);
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
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
	
	srand(time(NULL));
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
				.inbound = 256,
				.outbound = 256,
			}
	}
  };
  app_event_loop(params, &handlers);
}
