#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <time.h>
#include "../config/config.h"

class NetworkManager {
private:
  const Config& config;
  bool wifiConnected = false;
  unsigned long lastTimeUpdate = 0;
  
  // Current date/time
  int currentYear = 2026;
  int currentMonth = 3;  // March
  int currentDay = 4;
  int daysInMonth = 31;
  int startDayOfWeek = 0;  // 0=Sunday

public:
  NetworkManager(const Config& cfg);
  
  bool connect();
  void disconnect();
  bool isConnected();
  
  void updateTime();
  bool getCurrentDate(int& year, int& month, int& day, int& daysInMonth, int& startDay);
  
  // Status colors (green shades)
  enum StatusColor {
    DISCONNECTED = 0x0000,  // Black
    CONNECTING = 0x0200,    // Dark green
    CONNECTED = 0x07E0,     // Bright green
    ERROR = 0xF800         // Red
  };
  
  StatusColor getConnectionStatus();
};

#endif