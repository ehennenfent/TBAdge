#include "lib.c"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "Adafruit_miniTFTWing.h"
#include <deque>

Adafruit_miniTFTWing ss;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

byte program[MAX_PROGRAM] = {'+', '+', '=', '+', '+', '+', '+', '+', '+', '[', '-', '>', '+', '+', 
                             '+', '+', '+', '+', '+', '+', '<', ']', '>', '+', '?', '+', '?', '+', '?'};
byte memory[MEMORY_SIZE];
std::deque<byte> buffer;

word mpointer;
word epointer;
byte io_mode;


void render(byte * mem, std::deque<byte> &buf){
  for (word i = 0; i < MEMPIXELS; i++){
    pixels.setPixelColor(mem_coord_transform(i), colors[mem[i]]);
  }

  for (word i = 0; i < BUFPIXELS; i++){
    pixels.setPixelColor(buf_coord_transform(i), (i < buffer.size()) ? colors[buf[i]] : colors[0]);
  }

  pixels.show();
}


void setup() {
  pixels.begin();

  ss.tftReset();
  ss.setBacklight(0x0); // fully on

  tft.initR(INITR_MINI160x80);
  tft.setRotation(3);

  reset();

  colors[0] = pixels.Color(0, 0, 0);
  for (word i = 1; i < 256; i+= 7){
    byte c = max(1, i/3); // ((i - 1) / 7) + 1;
    colors[i + 0] = pixels.Color(c, 0, 0);
    colors[i + 1] = pixels.Color(0, c, 0);
    colors[i + 2] = pixels.Color(0, 0, c);
    colors[i + 3] = pixels.Color(c, c, 0);
    colors[i + 4] = pixels.Color(c, 0, c);
    colors[i + 5] = pixels.Color(0, c, c);
    colors[i + 6] = pixels.Color(c, c, c);
  }

  pixels.show();
}

void loop() {
  pixels.clear();
  reset();

  while(single_step()){
    render(memory, buffer);
    delay(500);
  }

  delay(1500);
}

bool single_step(){
  draw_char(program[epointer]);
  switch(program[epointer]){
    case '>':
      if (mpointer != MEMORY_SIZE) mpointer++;
      break;
    case '<':
      if (mpointer != 0) mpointer--;
      break;
    case '+':
      if (memory[mpointer] != BYTE_MAX) memory[mpointer]++;
      break;
    case '-':
      if (memory[mpointer] != 0) memory[mpointer]--;
      break;
    case '[':
      if (memory[mpointer] == 0){
        word step = epointer + 1;
        int num_rb_needed = 1;
        while (step < MAX_PROGRAM){
          step++;
          if (program[step] == '[') num_rb_needed++;
          if (program[step] == ']') num_rb_needed--;
          if (num_rb_needed == 0) break;
        }
        epointer = step;
      }
      break;
    case ']':
      if (memory[mpointer] != 0){
        word step = epointer;
        int num_lb_needed = 1;
        while (step > 0){
          step--;
          if (program[step] == ']') num_lb_needed++;
          if (program[step] == '[') num_lb_needed--;
          if (num_lb_needed == 0) break;
        }
        epointer = step;
      }
      break;
    case '=':
      io_mode = memory[mpointer];
      break;
    case '?':
      do_io(io_mode);
      break;
    case '\x00':
      return false;
    default:
      break;
  }

  epointer++;
  if (epointer == MAX_PROGRAM){
    return false;
  }
  return true;
}

void reset(){
  memset(memory, 0, MEMORY_SIZE);
  buffer.clear();
  mpointer = 0;
  epointer = 0;
  io_mode = 0;
}

void do_io(byte mode){
  switch (mode){
    case 2:
      draw_char(memory[mpointer]);
      break;
    case 6:
      buffer.clear();
      for (word i = 0; i < MAX_PROGRAM; i++){
        buffer.push_back(program[i]);
        if (program[i] == '\x00') break;
      }
      break;
    case 8:
      if (buffer.size() < BUFFER_SIZE) buffer.push_back(memory[mpointer]);
      break;
    case 9:
      if (!buffer.empty()){
        memory[mpointer] = buffer.back();
        buffer.pop_back();
      }
      else{
        memory[mpointer] = 0;
      }
      break;
    case 10:
      memory[mpointer] = dequeue_fifo();
      break;
    case 11:
      buffer.clear();
      break;
    case 12:
      if (0 <= memory[mpointer] <= 25){
        memory[mpointer] += 97;
      }
      break;
    case 13:
      if (0 <= memory[mpointer] <= 25){
        memory[mpointer] += 65;
      }
      break;
    case 14:
      if (0 <= memory[mpointer] <= 9){
        memory[mpointer] += 48;
      }
      break;
    case 15:
      switch (memory[mpointer]){
        case 0:
          memory[mpointer] = '+';
        case 1:
          memory[mpointer] = '-';
        case 2:
          memory[mpointer] = '<';
        case 3:
          memory[mpointer] = '>';
        case 4:
          memory[mpointer] = '[';
        case 5:
          memory[mpointer] = ']';
        case 6:
          memory[mpointer] = '=';
        case 7:
          memory[mpointer] = '?';
        default:
          break;
      }
      break;
    case 20:
      memory[mpointer] &= dequeue_fifo();
      break;
    case 21:
      memory[mpointer] |= dequeue_fifo();
      break;
    case 22:
      memory[mpointer] = (dequeue_fifo() ? true : false);
      break;
    case 23:
      memory[mpointer] ^= dequeue_fifo();
      break;
    case 24:
      memory[mpointer] = (byte) mpointer;
      break;
    case 25:
      memory[mpointer] = (byte) (epointer + 1);
      break;
    case 26:
      if ((int) epointer - memory[mpointer] >= 0) epointer -= memory[mpointer];
      break;
    case 27:
      if (epointer + memory[mpointer] < MAX_PROGRAM) epointer += memory[mpointer];
      break;
  }
}

byte dequeue_fifo(){
  if (buffer.empty()) return 0;
  byte o = buffer.front();
  buffer.pop_front();
  return o;
}

void draw_char(byte c){
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(60, 8);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(9);
  tft.println((char) c);
}
