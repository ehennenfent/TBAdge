#ifndef TBAS_H
#define TBAS_H

#include <Arduino.h>
#include "constants.h"
#include <deque>

bool single_step();
void reset();
void do_io(byte mode);
byte dequeue_fifo();
void set_draw_method(void (*new_meth)(byte));

byte * get_memory();
std::deque<byte>& get_buf();

#endif