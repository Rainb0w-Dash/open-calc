#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// === Pin mapping for ESP32-S3 WROOM-1 ===
#define TFT_MOSI 11
#define TFT_MISO 13
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  21   // Your chosen reset pin

// Use the default SPI instance
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(200);

  // Initialize SPI with your custom pins
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  // Initialize display
  tft.begin();

  tft.setRotation(1);   // Landscape
  tft.fillScreen(ILI9341_BLACK);

  drawDOOM();
}

void loop() {
  // Nothing needed here
}

void drawDOOM() {
  const char* text = "DOOM";

  tft.setTextSize(6);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);

  int16_t x1, y1;
  uint16_t w, h;

  // Calculate text bounds so we can center it
  tft.getTextBounds((char*)text, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (tft.width()  - w) / 2;
  int16_t y = (tft.height() - h) / 2;

  tft.setCursor(x, y);
  tft.print(text);
}
