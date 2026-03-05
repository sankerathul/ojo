#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Configuration
struct WiFiConfig {
  const char* ssid;
  const char* password;
};

// NTP Configuration
struct NTPConfig {
  const char* server;
  long gmtOffset;
  int daylightOffset;
};

// GitHub Configuration
struct GitHubConfig {
  const char* username;
  const char* token;  // Optional: for private repos or higher rate limits
  bool includePrivate;  // Include private repository contributions
  int updateInterval;   // Minutes between GitHub API calls
};

// Display Configuration
struct DisplayConfig {
  int width;
  int height;
  int frameRate;  // FPS
};

// Main Configuration Structure
struct Config {
  WiFiConfig wifi;
  NTPConfig ntp;
  GitHubConfig github;
  DisplayConfig display;
};

// Default Configuration
extern const Config defaultConfig;

#endif