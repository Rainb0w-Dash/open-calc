#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// === Pin mapping for ESP32-S3 WROOM-1 ===
#define TFT_MOSI 11
#define TFT_MISO 13
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  21

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// --- 3D Wireframe Structures & Data ---
struct Point3D { float x, y, z; };
struct Point2D { int16_t x, y; };

const Point3D cubeNodes[8] = {
  {-1.0, -1.0, -1.0}, { 1.0, -1.0, -1.0},
  { 1.0,  1.0, -1.0}, {-1.0,  1.0, -1.0},
  {-1.0, -1.0,  1.0}, { 1.0, -1.0,  1.0},
  { 1.0,  1.0,  1.0}, {-1.0,  1.0,  1.0}
};

const uint8_t cubeEdges[12][2] = {
  {0, 1}, {1, 2}, {2, 3}, {3, 0},
  {4, 5}, {5, 6}, {6, 7}, {7, 4},
  {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

// Function Declarations
void testColorBars();
void testShapes();
void testTextSizes();
void test3DRender(uint32_t durationMs);
void testBouncingBox();

void setup() {
  Serial.begin(115200);
  delay(200);

  // Initialize Hardware SPI
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
  tft.begin();
  tft.setRotation(1); // 320x240 Landscape
}

void loop() {
  // Segment 1: Color Bar Test
  testColorBars();
  delay(2000);

  // Segment 2: Shapes & Alignment
  testShapes();
  delay(2000);

  // Segment 3: Text Scaling
  testTextSizes();
  delay(2000);

  // Segment 4: Real-time 3D Wireframe Render (Runs for 6 Seconds)
  test3DRender(6000);

  // Segment 5: Frame Refresh / Motion Test
  testBouncingBox();
  delay(1000);
}

// ==================== TEST SEGMENTS ====================

// --- Segment 1: Color Bars ---
void testColorBars() {
  tft.fillScreen(ILI9341_BLACK);

  uint16_t colors[] = {
    ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE,
    ILI9341_YELLOW, ILI9341_CYAN, ILI9341_MAGENTA, ILI9341_WHITE
  };

  int barWidth = tft.width() / 7;

  for (int i = 0; i < 7; i++) {
    tft.fillRect(i * barWidth, 0, barWidth, tft.height() - 30, colors[i]);
  }

  tft.fillRect(0, tft.height() - 30, tft.width(), 30, ILI9341_NAVY);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, tft.height() - 22);
  tft.print("1/5 COLOR TEST");
}

// --- Segment 2: Shapes & Geometry ---
void testShapes() {
  tft.fillScreen(ILI9341_BLACK);

  tft.drawRect(0, 0, tft.width(), tft.height(), ILI9341_WHITE);
  tft.drawLine(0, 0, tft.width(), tft.height(), ILI9341_DARKGREY);
  tft.drawLine(0, tft.height(), tft.width(), 0, ILI9341_DARKGREY);

  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;
  for (int16_t r = 10; r < 100; r += 15) {
    tft.drawCircle(cx, cy, r, ILI9341_CYAN);
  }

  tft.fillRect(10, 10, 30, 30, ILI9341_RED);
  tft.fillRect(tft.width() - 40, 10, 30, 30, ILI9341_GREEN);
  tft.fillRect(10, tft.height() - 40, 30, 30, ILI9341_BLUE);
  tft.fillRect(tft.width() - 40, tft.height() - 40, 30, 30, ILI9341_YELLOW);
}

// --- Segment 3: Typography ---
void testTextSizes() {
  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1);
  tft.println("Size 1: ESP32-S3 TFT Test");

  tft.setCursor(10, 30);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.println("Size 2: ILI9341");

  tft.setCursor(10, 60);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(3);
  tft.println("Size 3: 320x240");

  tft.setCursor(10, 110);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(4);
  tft.println("Size 4: DOOM");

  tft.setCursor(10, 170);
  tft.setTextColor(ILI9341_MAGENTA);
  tft.setTextSize(5);
  tft.println("READY!");
}

// --- Segment 4: 3D Wireframe Renderer ---
void test3DRender(uint32_t durationMs) {
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("4/5 3D WIREFRAME");

  Point2D projected[8];
  Point2D oldProjected[8] = {0};

  float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
  const int16_t centerX = tft.width() / 2;
  const int16_t centerY = (tft.height() / 2) + 10;
  const float scale = 100.0;
  const float distance = 3.2;

  uint32_t startTime = millis();

  while (millis() - startTime < durationMs) {
    float radX = angleX * DEG_TO_RAD;
    float radY = angleY * DEG_TO_RAD;
    float radZ = angleZ * DEG_TO_RAD;

    float cx = cos(radX), sx = sin(radX);
    float cy = cos(radY), sy = sin(radY);
    float cz = cos(radZ), sz = sin(radZ);

    for (int i = 0; i < 8; i++) {
      oldProjected[i] = projected[i];

      float x = cubeNodes[i].x;
      float y = cubeNodes[i].y;
      float z = cubeNodes[i].z;

      float y1 = y * cx - z * sx;
      float z1 = y * sx + z * cx;

      float x2 = x * cy + z1 * sy;
      float z2 = -x * sy + z1 * cy;

      float x3 = x2 * cz - y1 * sz;
      float y3 = x2 * sz + y1 * cz;

      projected[i].x = (int16_t)(centerX + (x3 * scale) / (z2 + distance));
      projected[i].y = (int16_t)(centerY + (y3 * scale) / (z2 + distance));
    }

    // Erase old lines
    for (int i = 0; i < 12; i++) {
      uint8_t p1 = cubeEdges[i][0];
      uint8_t p2 = cubeEdges[i][1];
      tft.drawLine(oldProjected[p1].x, oldProjected[p1].y,
                   oldProjected[p2].x, oldProjected[p2].y, ILI9341_BLACK);
    }

    // Draw new lines
    for (int i = 0; i < 12; i++) {
      uint8_t p1 = cubeEdges[i][0];
      uint8_t p2 = cubeEdges[i][1];
      tft.drawLine(projected[p1].x, projected[p1].y,
                   projected[p2].x, projected[p2].y, ILI9341_CYAN);
    }

    angleX += 2.5;
    angleY += 3.5;
    angleZ += 1.0;

    delay(12); // ~60 FPS
  }
}

// --- Segment 5: Dynamic Bouncing Box ---
void testBouncingBox() {
  tft.fillScreen(ILI9341_BLACK);

  int boxSize = 25;
  int x = 10, y = 10;
  int dx = 4, dy = 3;

  uint32_t startTime = millis();

  while (millis() - startTime < 4000) {
    tft.fillRect(x, y, boxSize, boxSize, ILI9341_BLACK);

    x += dx;
    y += dy;

    if (x <= 0 || x + boxSize >= tft.width()) dx = -dx;
    if (y <= 0 || y + boxSize >= tft.height()) dy = -dy;

    tft.fillRect(x, y, boxSize, boxSize, ILI9341_RED);
    tft.drawRect(x, y, boxSize, boxSize, ILI9341_WHITE);

    delay(10);
  }
}
