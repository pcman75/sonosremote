#include "debug.h"
#include "util.h"
#include "pebble_app.h"

//TODO:
extern TextLayer textLayer;

void debug(char* message, int status)
{
	static char debug_buf[20];
	strcpy(debug_buf, message);
	strcat(debug_buf, " ");
	strcat(debug_buf, itoa(status));
	text_layer_set_text(&textLayer, debug_buf);
	APP_LOG(APP_LOG_LEVEL_DEBUG, debug_buf);
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
