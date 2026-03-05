#include "../../include/network/network.h"

NetworkManager::NetworkManager(const Config& cfg) : config(cfg) {
}

bool NetworkManager::connect() {
  if (wifiConnected) return true;
  
  WiFi.begin(config.wifi.ssid, config.wifi.password);
  
  // Wait up to 10 seconds for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }
  
  wifiConnected = (WiFi.status() == WL_CONNECTED);
  
  if (wifiConnected) {
    // Configure NTP
    configTime(config.ntp.gmtOffset, config.ntp.daylightOffset, config.ntp.server);
    updateTime();
  }
  
  return wifiConnected;
}

void NetworkManager::disconnect() {
  if (wifiConnected) {
    WiFi.disconnect();
    wifiConnected = false;
  }
}

bool NetworkManager::isConnected() {
  return wifiConnected && WiFi.status() == WL_CONNECTED;
}

void NetworkManager::updateTime() {
  if (!isConnected()) return;
  
  if (millis() - lastTimeUpdate > 60000) {  // Update every minute
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
      currentYear = timeInfo.tm_year + 1900;
      currentMonth = timeInfo.tm_mon + 1;  // tm_mon is 0-11
      currentDay = timeInfo.tm_mday;
      
      // Calculate days in current month
      daysInMonth = 31;  // Default
      if (currentMonth == 4 || currentMonth == 6 || currentMonth == 9 || currentMonth == 11) {
        daysInMonth = 30;
      } else if (currentMonth == 2) {
        // Leap year check
        bool isLeap = (currentYear % 4 == 0 && currentYear % 100 != 0) || (currentYear % 400 == 0);
        daysInMonth = isLeap ? 29 : 28;
      }
      
      // Calculate what day of week the month starts on (0=Sunday)
      struct tm firstDay = timeInfo;
      firstDay.tm_mday = 1;
      firstDay.tm_hour = 0;
      firstDay.tm_min = 0;
      firstDay.tm_sec = 0;
      mktime(&firstDay);
      startDayOfWeek = firstDay.tm_wday;  // 0=Sunday, 1=Monday, etc.
      
      lastTimeUpdate = millis();
    }
  }
}

bool NetworkManager::getCurrentDate(int& year, int& month, int& day, int& daysInMonth, int& startDay) {
  year = currentYear;
  month = currentMonth;
  day = currentDay;
  daysInMonth = this->daysInMonth;
  startDay = startDayOfWeek;
  return true;
}

NetworkManager::StatusColor NetworkManager::getConnectionStatus() {
  if (!wifiConnected) return DISCONNECTED;
  
  wl_status_t status = WiFi.status();
  switch (status) {
    case WL_CONNECTED: return CONNECTED;
    case WL_CONNECT_FAILED:
    case WL_CONNECTION_LOST:
    case WL_DISCONNECTED: return ERROR;
    default: return CONNECTING;
  }
}