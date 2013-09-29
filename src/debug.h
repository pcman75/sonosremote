#ifndef DEBUG_H
#define DEBUG_H

#include "pebble_os.h"

void debug(char* message, int status);
void dump_dict(DictionaryIterator* iter);


#endif //DEBUG_H