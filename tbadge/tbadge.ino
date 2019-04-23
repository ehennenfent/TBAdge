#include "lib.c"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

byte program[MAX_PROGRAM] = {'+', '>', '+', '+', '>', '+', '+', '+', '>', '+', '+', '+', '+',
                             '>', '+', '+', '+', '+', '+', '>', '+', '+', '+', '+', '+', '+'};
byte memory[MEMORY_SIZE];
byte buffer[BUFFER_SIZE];

word mpointer;
word epointer;
byte io_mode;


void render(byte * mem, byte * buf){
  for (word i = 0; i < MEMPIXELS; i++){
    pixels.setPixelColor(mem_coord_transform(i), colors[mem[i]]);
  }

  for (word i = 0; i < BUFPIXELS; i++){
    pixels.setPixelColor(buf_coord_transform(i), colors[buf[i]]);
  }

  pixels.show();
}


void setup() {
  pixels.begin();
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
    render(memory, memory + 32);
    delay(250);
  }

  delay(1500);
}

bool single_step(){
  switch(program[epointer]){
    case '>':
      if (mpointer != BYTE_MAX) mpointer++;
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
    default:
      return false;
  }

  epointer++;
  return true;
}

void reset(){
  memset(memory, 0, MEMORY_SIZE);
  mpointer = 0;
  epointer = 0;
  io_mode = 0;
}