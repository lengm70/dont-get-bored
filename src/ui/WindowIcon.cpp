#include "ui/WindowIcon.h"

#include "raylib.h"
#include "config/UiConfig.h"

namespace ui {

#ifndef _WIN32
bool ConfigureApplicationIdentity() { return true; }
#endif

bool ConfigureWindowIcon() {
    Image icon = LoadImage(config::iconPath);
    if (!IsImageValid(icon)) {
        TraceLog(LOG_WARNING, "Could not load application icon: %s", config::iconPath);
        return false;
    }
    SetWindowIcon(icon);
    UnloadImage(icon);
    return true;
}

}  // namespace ui
