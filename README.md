# OJO - GitHub Contributions Display

A beautiful circular display showing GitHub contribution data on an ESP32-C3 with a 240x240 TFT screen.

## Features

- 📅 **Dynamic Calendar**: Shows current month with proper day alignment
- 🌐 **WiFi Connected**: Fetches real date/time from NTP servers
- 📶 **WiFi Status Icon**: Animated WiFi icon shows connection status
- 🎨 **GitHub Activity Colors**: Green shades represent contribution levels
- 📱 **Modular Architecture**: Easy to add new views/screens
- 🔄 **Real GitHub Data**: Fetches actual contribution data via GitHub GraphQL API

## Setup

### 1. Configuration

Edit `src/config.cpp` to set your credentials:

```cpp
const Config defaultConfig = {
  // WiFi Settings
  .wifi = {
    .ssid = "YOUR_WIFI_NAME",
    .password = "YOUR_WIFI_PASSWORD"
  },
  
  // GitHub Settings
  .github = {
    .username = "YOUR_GITHUB_USERNAME",
    .token = "",  // Optional: GitHub personal access token for higher rate limits
    .includePrivate = false,
    .updateInterval = 60
  },
  
  // NTP Settings
  .ntp = {
    .server = "pool.ntp.org",
    .gmtOffset = 0,
    .daylightOffset = 0
  }
};
```

### 2. Upload to ESP32

```bash
pio run --target upload
```

### 3. Monitor Output

```bash
pio device monitor
```

## GitHub API Integration

OJO fetches real contribution data using GitHub's GraphQL API:

- **Authentication**: Uses personal access token (optional but recommended)
- **Data Source**: Fetches contribution calendar for current month
- **Rate Limits**: 5,000 requests/hour with token, 60 without
- **Privacy**: Only public contributions unless token allows private repos

### Serial Logging

The device outputs detailed logs to the serial monitor:

```
GitHubAPI: Starting contribution fetch for sankerathul 2026-3
GitHubAPI: Sending GraphQL request...
GitHubAPI: Response status: 200
GitHubAPI: Parsed 31 contribution days for Mar 2026
ContributionView: Successfully fetched 31 contribution days
```

## Startup Sequence

OJO follows a beautiful startup sequence:

1. **Loading Screen**: Shows "OJO" with animated WiFi icon while connecting
2. **Connected Screen**: Displays checkmark ✅ when WiFi connection succeeds  
3. **Calendar View**: Loads the GitHub contribution calendar

## WiFi Status Icon

- **Loading**: Animated WiFi bars during connection
- **Connected**: Solid WiFi icon in top-right corner
- **Error**: Red WiFi icon if connection fails

## GitHub Activity Colors

The calendar squares use green shades to represent daily contribution levels:

- 🟢 **Light Green**: No contributions (0)
- 🟢 **Pale Green**: Low activity (1-2)
- 🟢 **Medium Green**: Moderate activity (3-4)
- 🟢 **Dark Green**: High activity (5-6)
- 🟢 **Darker Green**: Very high activity (7-8)
- 🟢 **Deep Green**: Extreme activity (9+)

## Project Structure

```
include/
├── config/
│   └── config.h          # Configuration structures
├── core/
│   ├── display.h         # Color definitions and display config
│   └── view.h            # Base View interface
├── github/
│   └── github_api.h      # GitHub API client interface
├── network/
│   └── network.h         # WiFi and NTP management
└── views/
    └── contribution_view.h  # GitHub contributions display

src/
├── config.cpp            # Default configuration values
├── github_api.cpp        # GitHub API implementation
├── main.cpp              # Entry point and view management
├── network/
│   └── network.cpp       # Network implementation
└── views/
    └── contribution_view.cpp  # Contribution view implementation
```

## Adding New Views

1. Create `include/views/new_view.h`
2. Implement `src/views/new_view.cpp`
3. Add to `ViewManager` in `main.cpp`

## Dependencies

- LovyanGFX: Display driver for GC9A01 TFT
- ArduinoHttpClient: HTTP client for API calls
- ArduinoJson: JSON parsing for GitHub API responses
- ESP32 Arduino Core: WiFi and time functions

## Hardware

- ESP32-C3 DevKitM-1
- 240x240 GC9A01 TFT Display

### Display GPIO
- SCL: 4
- SDA: 5
- DC: 9
- CS: 2
- RST: 10