#ifndef DISPLAY_H
#define DISPLAY_H

#include <LovyanGFX.hpp>

// Display configuration and initialization
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
    cfg.freq_write = 80000000;   // 80MHz
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
    pcfg.pin_rst  = 10;
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

// Color palette
namespace Colors {
  inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

  constexpr uint16_t BG = 0x0000;        // Black background
  constexpr uint16_t TEXT = 0xFFFF;      // White text
  constexpr uint16_t SUBTEXT = 0x8410;   // Gray subtext
  
  // WiFi Status colors (green shades)
  constexpr uint16_t STATUS_DISCONNECTED = 0x0000;  // Black
  constexpr uint16_t STATUS_CONNECTING = 0x0200;    // Dark green
  constexpr uint16_t STATUS_CONNECTED = 0x07E0;     // Bright green
  constexpr uint16_t STATUS_ERROR = 0xF800;         // Red
  
  // GitHub Activity colors (green shades - light to dark)
  // Level 0: No contributions
  constexpr uint16_t GITHUB_NONE = 0x2104;    // Very dark gray (almost black)
  
  // Level 1-4: Low to high activity (light green to dark green)
  constexpr uint16_t GITHUB_LOW = 0x07E0;     // Bright light green (1-2 contributions)
  constexpr uint16_t GITHUB_MEDIUM = 0x0600;  // Medium green (3-4 contributions)  
  constexpr uint16_t GITHUB_HIGH = 0x0400;    // Dark green (5-6 contributions)
  constexpr uint16_t GITHUB_VERY_HIGH = 0x0200; // Very dark green (7-8 contributions)
  constexpr uint16_t GITHUB_EXTREME = 0x0100; // Almost black dark green (9+ contributions)
}

#endif
