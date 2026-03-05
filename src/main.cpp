#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "core/display.h"
#include "core/view.h"
#include "config/config.h"
#include "network/network.h"
#include "views/contribution_view.h"

// Global display and sprite
LGFX display;
LGFX_Sprite canvas(&display);

// Configuration
const Config& config = defaultConfig;

// Network manager
NetworkManager networkManager(config);

// Current active view
View* currentView = nullptr;

// ── View Manager ──────────────────────────────────────────
class ViewManager {
private:
  ContributionView contributionView;
  
public:
  ViewManager(NetworkManager& nm) : contributionView(nm) {}
  
  void init() {
    // Initialize first view (contribution)
    setView(&contributionView);
  }
  
  void setView(View* view) {
    if (currentView) {
      currentView->onDeactivate();
    }
    currentView = view;
    if (currentView) {
      currentView->init();
      currentView->onActivate();
    }
  }
  
  void update(int frame) {
    if (currentView) {
      currentView->render(canvas, frame);
    }
  }
};

ViewManager viewManager(networkManager);

// ── State ─────────────────────────────────────────────────
int  frame       = 0;
unsigned long lastFrameTime = 0;

void setup() {
  display.init();
  display.setRotation(0);
  display.fillScreen(TFT_BLACK);

  // Create full-screen 16-bit sprite
  canvas.setColorDepth(16);
  canvas.createSprite(240, 240);

  // Initialize view manager
  viewManager.init();
}

void loop() {
  unsigned long now = millis();
  if (now - lastFrameTime < 33) return;  // 30fps
  lastFrameTime = now;

  viewManager.update(frame++);
}