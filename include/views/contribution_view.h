#ifndef CONTRIBUTION_VIEW_H
#define CONTRIBUTION_VIEW_H

#include "../core/view.h"
#include "../core/display.h"
#include "../network/network.h"
#include "../github/github_api.h"
#include <vector>

class ContributionView : public View {
private:
  NetworkManager& networkManager;
  GitHubAPI githubAPI;
  
  // View states
  enum ViewState {
    LOADING,      // Initial loading/connecting state
    CONNECTED,    // Just connected, show checkmark
    CALENDAR      // Normal calendar display
  };
  
  ViewState currentState = LOADING;
  unsigned long stateStartTime = 0;
  unsigned long lastAnimationUpdate = 0;
  int loadingFrame = 0;
  
  // Dynamic month data
  int currentYear = 2026;
  int currentMonth = 3;
  int daysInMonth = 31;
  int startDayOfWeek = 0;
  
  // WiFi icon animation
  int wifiIconFrame = 0;
  unsigned long lastWifiIconUpdate = 0;
  
  // Real contribution data from GitHub API
  std::vector<ContributionDay> contributions;
  unsigned long lastGitHubUpdate = 0;
  bool hasFetchedContributions = false;

  uint16_t getContributionColor(uint8_t level);
  uint16_t getStatusColor();
  void drawContributionGrid(LGFX_Sprite& canvas);
  void drawLoadingScreen(LGFX_Sprite& canvas, int frame);
  void drawConnectedScreen(LGFX_Sprite& canvas);
  void drawWiFiIcon(LGFX_Sprite& canvas, int frame);
  void drawCheckmark(LGFX_Sprite& canvas);
  String getMonthName(int month);
  void fetchContributions();
  uint8_t getContributionLevel(int day);
  void updateState();

public:
  ContributionView(NetworkManager& nm);
  void init() override;
  void render(LGFX_Sprite& canvas, int frame) override;
};

#endif
