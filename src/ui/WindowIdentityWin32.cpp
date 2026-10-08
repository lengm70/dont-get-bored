#include "ui/WindowIcon.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shobjidl.h>

namespace ui {

bool ConfigureApplicationIdentity() {
    return SUCCEEDED(SetCurrentProcessExplicitAppUserModelID(L"Lengm.DontGetBored"));
}

}  // namespace ui
