#ifndef VIEW_H
#define VIEW_H

#include <LovyanGFX.hpp>

// Base class for all OJO views
class View {
public:
  virtual ~View() = default;

  // Called once when view is first displayed
  virtual void init() {}

  // Called every frame to render the view
  virtual void render(LGFX_Sprite& canvas, int frame) = 0;

  // Called when view is activated
  virtual void onActivate() {}

  // Called when view is deactivated
  virtual void onDeactivate() {}
};

#endif
