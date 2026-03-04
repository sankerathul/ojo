#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <math.h>

// ── Display config ────────────────────────────────────────
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_GC9A01  _panel;
  lgfx::Bus_SPI       _bus;
  lgfx::Light_PWM     _light;

public:
  LGFX() {
    // SPI bus
    auto cfg = _bus.config();
    cfg.spi_host = SPI2_HOST;
    cfg.spi_mode = 0;
    cfg.freq_write = 80000000;   // 80MHz — much faster than Arduino_GFX default
    cfg.freq_read  = 16000000;
    cfg.spi_3wire  = false;
    cfg.use_lock   = true;
    cfg.dma_channel = SPI_DMA_CH_AUTO;
    cfg.pin_sclk =  4;
    cfg.pin_mosi =  5;
    cfg.pin_miso = -1;
    cfg.pin_dc   =  9;
    _bus.config(cfg);
    _panel.setBus(&_bus);

    // Panel
    auto pcfg = _panel.config();
    pcfg.pin_cs   = 2;
    pcfg.pin_rst  =  10;  // adjust if your RST is on a different pin
    pcfg.pin_busy = -1;
    pcfg.memory_width  = 240;
    pcfg.memory_height = 240;
    pcfg.panel_width   = 240;
    pcfg.panel_height  = 240;
    pcfg.readable      = false;
    pcfg.invert        = true;
    pcfg.rgb_order     = false;
    pcfg.dlen_16bit    = false;
    pcfg.bus_shared    = false;
    _panel.config(pcfg);
    setPanel(&_panel);
  }
};

LGFX display;
LGFX_Sprite canvas(&display);   // full-screen off-screen buffer

// ── Palette ───────────────────────────────────────────────
#define BG        canvas.color565(4,   6,  20)
#define IRIS_OUT  canvas.color565(0,  80, 180)
#define IRIS_MID  canvas.color565(0, 160, 220)
#define IRIS_IN   canvas.color565(20, 220, 200)
#define PUPIL_COL canvas.color565(2,   4,  14)
#define SCLERA    canvas.color565(220, 235, 255)
#define SHINE1    canvas.color565(255, 255, 255)
#define SHINE2    canvas.color565(180, 230, 255)
#define GOLD      canvas.color565(255, 200,  40)
#define RING1     canvas.color565(0,  200, 255)
#define RING2     canvas.color565(0,   80, 140)
#define TEXT_COL  canvas.color565(255, 255, 255)
#define SUB_COL   canvas.color565(120, 180, 220)
#define TICK_COL  canvas.color565(0,  220, 180)
#define DIM_COL   canvas.color565(30,  50,  80)

const int CX = 120, CY = 120;

// ── Helpers ───────────────────────────────────────────────
void fillEllipse(int cx, int cy, int rx, int ry, uint16_t color) {
  for (int y = -ry; y <= ry; y++) {
    float xw = rx * sqrtf(1.0f - ((float)y/ry)*((float)y/ry));
    canvas.drawFastHLine(cx-(int)xw, cy+y, 2*(int)xw+1, color);
  }
}

void drawTickRing(int frame) {
  for (int i = 0; i < 60; i++) {
    float rad = (i * 6) * M_PI / 180.0f;
    bool lit = ((i + frame) % 20) < 10;
    uint16_t col = lit ? TICK_COL : DIM_COL;
    canvas.drawLine(
      CX + (int)(108 * cosf(rad)), CY + (int)(108 * sinf(rad)),
      CX + (int)(114 * cosf(rad)), CY + (int)(114 * sinf(rad)),
      col
    );
  }
}

void drawEye(int blink) {
  int eyeRX = 62;
  int eyeCY = CY - 18;
  int eyeRY = (blink == 0) ? 34 : max(2, 34 - blink * 4);

  fillEllipse(CX, eyeCY, eyeRX, eyeRY, SCLERA);

  if (blink < 8) {
    int irisR = min(26, eyeRY - 2);
    canvas.fillCircle(CX, eyeCY, irisR,      IRIS_OUT);
    canvas.fillCircle(CX, eyeCY, irisR - 6,  IRIS_MID);
    canvas.fillCircle(CX, eyeCY, irisR - 12, IRIS_IN);

    for (int deg = 0; deg < 360; deg += 18) {
      float rad = deg * M_PI / 180.0f;
      canvas.drawLine(
        CX + (int)(8*(cosf(rad))),          eyeCY + (int)(8*sinf(rad)),
        CX + (int)((irisR-2)*cosf(rad)),    eyeCY + (int)((irisR-2)*sinf(rad)),
        IRIS_OUT
      );
    }
    canvas.fillCircle(CX, eyeCY, 10, PUPIL_COL);
    canvas.fillCircle(CX - 7, eyeCY - 7, 5, SHINE1);
    canvas.fillCircle(CX + 8, eyeCY - 4, 2, SHINE2);
  }

  // Eyelid outline
  for (int t = 0; t < 3; t++)
    for (int deg = 0; deg < 360; deg++) {
      float rad = deg * M_PI / 180.0f;
      canvas.drawPixel(
        CX    + (int)((eyeRX-t) * cosf(rad)),
        eyeCY + (int)((eyeRY-t) * sinf(rad)),
        canvas.color565(10, 30, 60)
      );
    }

  // Eyelashes
  if (blink < 5) {
    for (int deg = 200; deg <= 340; deg += 20) {
      float rad = deg * M_PI / 180.0f;
      canvas.drawLine(
        CX + (int)(eyeRX       * cosf(rad)), eyeCY + (int)(eyeRY       * sinf(rad)),
        CX + (int)((eyeRX + 7) * cosf(rad)), eyeCY + (int)((eyeRY + 7) * sinf(rad)),
        canvas.color565(10, 20, 50)
      );
    }
  }
}

void drawScene(int frame, int blink) {
  // 1. Draw everything to the sprite (RAM)
  canvas.fillScreen(BG);

  // Outer rings
  for (int r = 117; r <= 119; r++) canvas.drawCircle(CX, CY, r, RING1);
  for (int r = 104; r <= 105; r++) canvas.drawCircle(CX, CY, r, RING2);

  drawTickRing(frame);

  // Accent dots
  canvas.fillCircle(CX,      CY - 97, 3, GOLD);
  canvas.fillCircle(CX,      CY + 97, 3, GOLD);
  canvas.fillCircle(CX - 97, CY,      3, GOLD);
  canvas.fillCircle(CX + 97, CY,      3, GOLD);

  drawEye(blink);

  // OJO title
  canvas.setTextSize(4);
  canvas.setTextColor(TEXT_COL);
  canvas.setCursor(CX - 36, CY + 28);
  canvas.print("OJO");

  // Gold underline
  canvas.drawFastHLine(CX - 38, CY + 58, 76, GOLD);
  canvas.drawFastHLine(CX - 38, CY + 59, 76, GOLD);

  // Coming soon
  canvas.setTextSize(1);
  canvas.setTextColor(SUB_COL);
  canvas.setCursor(CX - 33, CY + 70);
  canvas.print("COMING SOON");

  // 2. Push entire sprite to display in one SPI burst — no flicker
  canvas.pushSprite(0, 0);
}

// ── State ─────────────────────────────────────────────────
int  frame       = 0;
int  blinkStep   = 0;
bool blinking    = false;
unsigned long lastBlinkTime = 0;
unsigned long lastFrameTime = 0;

void setup() {
  display.init();
  display.setRotation(0);
  display.fillScreen(TFT_BLACK);

  // Create full-screen 16-bit sprite (240x240 = 115KB — fits in ESP32C3 RAM)
  canvas.setColorDepth(16);
  canvas.createSprite(240, 240);

  drawScene(0, 0);
}

void loop() {
  unsigned long now = millis();
  if (now - lastFrameTime < 33) return;  // 30fps
  lastFrameTime = now;

  if (!blinking && (now - lastBlinkTime > 4000)) {
    blinking  = true;
    blinkStep = 0;
  }
  if (blinking) {
    blinkStep++;
    if (blinkStep > 18) {
      blinking      = false;
      blinkStep     = 0;
      lastBlinkTime = now;
    }
  }

  int blink = 0;
  if (blinking)
    blink = (blinkStep <= 9) ? blinkStep : (18 - blinkStep);

  drawScene(frame++, blink);
}