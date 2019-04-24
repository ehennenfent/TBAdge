#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "Adafruit_miniTFTWing.h"
#include "tbas.h"

Adafruit_miniTFTWing ss;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t colors[256];

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

void render(byte * mem, std::deque<byte> &buf){
  for (word i = 0; i < MEMPIXELS; i++){
    pixels.setPixelColor(mem_coord_transform(i), colors[mem[i]]);
  }

  for (word i = 0; i < BUFPIXELS; i++){
    pixels.setPixelColor(buf_coord_transform(i), (i < buf.size()) ? colors[buf[i]] : colors[0]);
  }

  pixels.show();
}

void setup() {
  Serial.begin(9600);
  pixels.begin();

  ss.tftReset();
  ss.setBacklight(0x0); // fully on

  tft.initR(INITR_MINI160x80);
  tft.fillScreen(ST77XX_WHITE);
  tft.setRotation(3);

  // set_draw_method(write_char);
  // reset();

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
    render(get_memory(), get_buf());
    delay(500);
  }

  delay(1500);
}

void write_char(byte c){
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(60, 8);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(9);
  tft.println((char) c);
}
