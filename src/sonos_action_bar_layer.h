#ifndef SONOS_ACTION_BAR_LAYER_H
#define SONOS_ACTION_BAR_LAYER_H

typedef struct
{
    HeapBitmap button_image_prev;
    HeapBitmap button_image_next;
    HeapBitmap button_image_play;
    HeapBitmap button_image_pause; 
    
    ActionBarLayer action_bar_layer;
} SonosActionBarLayer;

#endif //SONOS_ACTION_BAR_LAYER_H