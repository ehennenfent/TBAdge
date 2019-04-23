#include <Arduino.h>
#include "constants.h"

word mem_coord_transform(word in_coord){
    if (in_coord < 8){
        return in_coord + 24;
    }
    if (in_coord < 16){
        return in_coord + 8;
    }
    if (in_coord < 24){
        return in_coord - 8;
    }
    return in_coord - 24;
}

word buf_coord_transform(word in_coord){
    return mem_coord_transform(in_coord) + 32;
}

uint32_t colors[256];