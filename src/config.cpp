#include "../include/config/config.h"

// Read from environment variables or use defaults
// Set these via:
//   1. Environment vars: set WIFI_SSID=YourSSID before building
//   2. PlatformIO: add to platformio.ini under build_flags
//   3. .env file in project root

#ifndef WIFI_SSID
#define WIFI_SSID "YOUR_WIFI_SSID"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif

#ifndef GITHUB_USERNAME
#define GITHUB_USERNAME "your-github-username"
#endif

#ifndef GITHUB_TOKEN
#define GITHUB_TOKEN ""
#endif

#ifndef NTP_SERVER
#define NTP_SERVER "pool.ntp.org"
#endif

#ifndef GMT_OFFSET
#define GMT_OFFSET 0
#endif

#ifndef DAYLIGHT_OFFSET
#define DAYLIGHT_OFFSET 0
#endif

// Default Configuration
const Config defaultConfig = {
  // WiFi Settings
  .wifi = {
    .ssid = WIFI_SSID,
    .password = WIFI_PASSWORD
  },

  // NTP Settings
  .ntp = {
    .server = NTP_SERVER,
    .gmtOffset = GMT_OFFSET,
    .daylightOffset = DAYLIGHT_OFFSET
  },

  // GitHub Settings
  .github = {
    .username = GITHUB_USERNAME,
    .token = GITHUB_TOKEN,
    .includePrivate = false,
    .updateInterval = 60
  },

  // Display Settings
  .display = {
    .width = 240,
    .height = 240,
    .frameRate = 30
  }
};