#pragma once
#include "raylib.h"
namespace ui {
struct ControlMotion { float hover=0,pressed=0,pulse=0; };
void BeginUiAnimations(float seconds);
ControlMotion GetControlMotion(Rectangle bounds,const char* label,bool activated=false);
int AnimatedButton(Rectangle bounds,const char* text);
int AnimatedComboBox(Rectangle bounds,const char* text,int* active);
int AnimatedCheckBox(Rectangle bounds,const char* text,bool* checked);
int AnimatedSlider(Rectangle bounds,const char* left,const char* right,float* value,float min,float max);
int AnimatedValueBox(Rectangle bounds,const char* text,int* value,int min,int max,bool editing);
int AnimatedSpinner(Rectangle bounds,const char* text,int* value,int min,int max,bool editing);
}  // namespace ui
