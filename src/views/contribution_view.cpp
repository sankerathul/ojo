#include "../../include/views/contribution_view.h"
#include "../../include/config/config.h"

ContributionView::ContributionView(NetworkManager& nm) : networkManager(nm) {
  // Initialize GitHub API with auth token if provided
  if (strlen(defaultConfig.github.token) > 0) {
    githubAPI.setAuthToken(defaultConfig.github.token);
  }
}

void ContributionView::init() {
  // Try to connect to WiFi
  networkManager.connect();
  stateStartTime = millis();
  currentState = LOADING;
}

void ContributionView::updateState() {
  unsigned long currentTime = millis();
  NetworkManager::StatusColor status = networkManager.getConnectionStatus();
  
  switch (currentState) {
    case LOADING:
      if (status == NetworkManager::CONNECTED) {
        currentState = CONNECTED;
        stateStartTime = currentTime;
      }
      break;
      
    case CONNECTED:
      if (currentTime - stateStartTime > 2000) {  // Show checkmark for 2 seconds
        currentState = CALENDAR;
        stateStartTime = currentTime;
        // Fetch contributions when entering calendar mode
        fetchContributions();
      }
      break;
      
    case CALENDAR:
      // Stay in calendar mode
      break;
  }
}

uint16_t ContributionView::getContributionColor(uint8_t level) {
  // GitHub activity levels (0-9+ contributions per day)
  switch(level) {
    case 0: return Colors::GITHUB_NONE;        // No activity
    case 1: 
    case 2: return Colors::GITHUB_LOW;         // 1-2 contributions
    case 3: 
    case 4: return Colors::GITHUB_MEDIUM;      // 3-4 contributions
    case 5: 
    case 6: return Colors::GITHUB_HIGH;        // 5-6 contributions
    case 7: 
    case 8: return Colors::GITHUB_VERY_HIGH;   // 7-8 contributions
    default: return Colors::GITHUB_EXTREME;    // 9+ contributions
  }
}

uint16_t ContributionView::getStatusColor() {
  switch(networkManager.getConnectionStatus()) {
    case NetworkManager::DISCONNECTED: return Colors::STATUS_DISCONNECTED;
    case NetworkManager::CONNECTING: return Colors::STATUS_CONNECTING;
    case NetworkManager::CONNECTED: return Colors::STATUS_CONNECTED;
    case NetworkManager::ERROR: return Colors::STATUS_ERROR;
    default: return Colors::STATUS_DISCONNECTED;
  }
}

String ContributionView::getMonthName(int month) {
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  return months[month - 1];
}

void ContributionView::drawContributionGrid(LGFX_Sprite& canvas) {
  const char* dayLabels[] = {"S", "M", "T", "W", "T", "F", "S"};
  constexpr int cellSize = 21;
  constexpr int padding = 2;
  constexpr int cellSpacing = cellSize + padding;
  
  // Update current date from network
  networkManager.updateTime();
  networkManager.getCurrentDate(currentYear, currentMonth, daysInMonth, daysInMonth, startDayOfWeek);
  
  // Calculate grid dimensions for current month only
  int weeksNeeded = ((startDayOfWeek + daysInMonth - 1) / 7) + 1;
  int gridWidth = 7 * cellSpacing - padding;
  int gridHeight = weeksNeeded * cellSpacing - padding;
  
  // Center the grid
  int gridStartX = (240 - gridWidth) / 2;
  int gridStartY = (240 - gridHeight) / 2 + 10; // Slight upward adjustment
  
  // Title - Current month and year
  String monthTitle = getMonthName(currentMonth) + " " + String(currentYear);
  canvas.setTextSize(1);
  canvas.setTextColor(Colors::TEXT);
  canvas.setCursor((240 - monthTitle.length() * 6) / 2, gridStartY - 25);
  canvas.print(monthTitle.c_str());

  // Day labels (Sun-Sat)
  canvas.setTextSize(1);
  canvas.setTextColor(Colors::SUBTEXT);
  for (int i = 0; i < 7; i++) {
    canvas.setCursor(gridStartX + i * cellSpacing + 8, gridStartY - 12);
    canvas.print(dayLabels[i]);
  }

  // Draw contribution boxes - only for actual days in current month
  int dayIndex = 0;
  
  for (int week = 0; week < weeksNeeded; week++) {
    for (int day = 0; day < 7; day++) {
      // Skip days before month starts
      if (week == 0 && day < startDayOfWeek) continue;
      
      // Stop after all days are drawn
      if (dayIndex >= daysInMonth) break;
      
      uint8_t contribLevel = getContributionLevel(dayIndex + 1);
      uint16_t color = getContributionColor(contribLevel);

      int x = gridStartX + day * cellSpacing;
      int y = gridStartY + week * cellSpacing;

      // Draw filled rectangle
      canvas.fillRect(x, y, cellSize, cellSize, color);
      
      // Draw border
      canvas.drawRect(x, y, cellSize, cellSize, canvas.color565(80, 80, 80));
      
      dayIndex++;
    }
    if (dayIndex >= daysInMonth) break;
  }
}

void ContributionView::drawLoadingScreen(LGFX_Sprite& canvas, int frame) {
  canvas.fillScreen(Colors::BG);
  
  // Update loading animation
  if (millis() - lastAnimationUpdate > 200) {
    loadingFrame = (loadingFrame + 1) % 4;
    lastAnimationUpdate = millis();
  }
  
  // Draw "OJO" title
  canvas.setTextSize(3);
  canvas.setTextColor(Colors::TEXT);
  canvas.setCursor(100, 80);
  canvas.print("OJO");
  
  // Draw loading text
  canvas.setTextSize(1);
  canvas.setTextColor(Colors::SUBTEXT);
  canvas.setCursor(85, 120);
  canvas.print("Connecting...");
  
  // Draw animated WiFi icon in center
  const int centerX = 120;
  const int centerY = 160;
  
  uint16_t iconColor = Colors::STATUS_CONNECTING;
  
  // Draw WiFi signal bars with animation
  for (int i = 0; i < 4; i++) {
    if (i <= loadingFrame) {
      int radius = 6 + i * 3;
      int startAngle = 45 + i * 10;
      int endAngle = 135 - i * 10;
      canvas.drawArc(centerX, centerY + 8, radius, radius - 2, startAngle, endAngle, iconColor);
    }
  }
  
  // Small circle at intersection
  canvas.fillCircle(centerX, centerY + 2, 1, iconColor);
}

void ContributionView::drawConnectedScreen(LGFX_Sprite& canvas) {
  canvas.fillScreen(Colors::BG);
  
  // Draw "OJO" title
  canvas.setTextSize(3);
  canvas.setTextColor(Colors::TEXT);
  canvas.setCursor(100, 80);
  canvas.print("OJO");
  
  // Draw connected text
  canvas.setTextSize(1);
  canvas.setTextColor(Colors::SUBTEXT);
  canvas.setCursor(95, 120);
  canvas.print("Connected!");
  
  // Draw checkmark
  drawCheckmark(canvas);
}

void ContributionView::drawCheckmark(LGFX_Sprite& canvas) {
  const int centerX = 120;
  const int centerY = 160;
  
  // Draw checkmark symbol
  canvas.drawLine(centerX - 8, centerY, centerX - 3, centerY + 5, Colors::STATUS_CONNECTED);
  canvas.drawLine(centerX - 3, centerY + 5, centerX + 8, centerY - 3, Colors::STATUS_CONNECTED);
  
  // Thicker lines
  canvas.drawLine(centerX - 8, centerY + 1, centerX - 3, centerY + 6, Colors::STATUS_CONNECTED);
  canvas.drawLine(centerX - 3, centerY + 6, centerX + 8, centerY - 2, Colors::STATUS_CONNECTED);
}

void ContributionView::drawWiFiIcon(LGFX_Sprite& canvas, int frame) {
  // WiFi icon position (top-right corner)
  const int centerX = 220;
  const int centerY = 15;
  
  NetworkManager::StatusColor status = networkManager.getConnectionStatus();
  
  // Don't show icon if disconnected
  if (status == NetworkManager::DISCONNECTED) return;
  
  uint16_t iconColor = getStatusColor();
  
  // Update animation frame for connecting state
  if (status == NetworkManager::CONNECTING) {
    if (millis() - lastWifiIconUpdate > 300) {  // Flash every 300ms
      wifiIconFrame = (wifiIconFrame + 1) % 4;
      lastWifiIconUpdate = millis();
    }
  } else {
    wifiIconFrame = 3;  // Full signal when connected
  }
  
  // Draw WiFi signal bars (simple representation)
  // Base arc
  canvas.drawArc(centerX, centerY + 8, 8, 6, 45, 135, iconColor);
  
  // Signal bars based on connection strength/frame
  for (int i = 0; i < wifiIconFrame; i++) {
    int radius = 4 + i * 2;
    int startAngle = 45 + i * 15;
    int endAngle = 135 - i * 15;
    canvas.drawArc(centerX, centerY + 8, radius, radius - 2, startAngle, endAngle, iconColor);
  }
  
  // Small circle at intersection
  canvas.fillCircle(centerX, centerY + 2, 1, iconColor);
}

void ContributionView::render(LGFX_Sprite& canvas, int frame) {
  // Update state machine
  updateState();
  
  // Render based on current state
  switch (currentState) {
    case LOADING:
      drawLoadingScreen(canvas, frame);
      break;
      
    case CONNECTED:
      drawConnectedScreen(canvas);
      break;
      
    case CALENDAR:
      canvas.fillScreen(Colors::BG);
      drawContributionGrid(canvas);
      drawWiFiIcon(canvas, frame);
      break;
  }
  
  canvas.pushSprite(0, 0);
}

void ContributionView::fetchContributions() {
  Serial.println("ContributionView: Fetching contributions for " + String(defaultConfig.github.username));

  if (githubAPI.fetchContributions(defaultConfig.github.username, currentYear, currentMonth, contributions)) {
    hasFetchedContributions = true;
    Serial.println("ContributionView: Successfully fetched " + String(contributions.size()) + " contribution days");
  } else {
    Serial.println("ContributionView: Failed to fetch contributions, using sample data");
    hasFetchedContributions = false;
    // Could fall back to sample data here if needed
  }
}

uint8_t ContributionView::getContributionLevel(int day) {
  if (hasFetchedContributions && day >= 1 && day <= contributions.size()) {
    // Find the contribution for this day
    String targetDate = String(currentYear) + "-" +
                       (currentMonth < 10 ? "0" : "") + String(currentMonth) + "-" +
                       (day < 10 ? "0" : "") + String(day);

    for (const auto& contrib : contributions) {
      if (contrib.date == targetDate) {
        return contrib.count;
      }
    }
  }

  // Fallback to sample data if no real data available
  static uint8_t sampleData[31] = {
    1, 2, 4, 2, 1, 3, 5,      // Week 1
    4, 2, 3, 4, 5, 2, 1,      // Week 2
    4, 5, 3, 2, 4, 5, 4,      // Week 3
    3, 2, 5, 5, 4, 1, 3,      // Week 4
    0, 0, 0                    // Week 5
  };

  return (day >= 1 && day <= 31) ? sampleData[day - 1] : 0;
}
