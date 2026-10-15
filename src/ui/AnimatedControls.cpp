#include "ui/AnimatedControls.h"
#include <array>
#include <cmath>
#include <string>
#include <unordered_map>
#include "raygui.h"
#include "ui/Motion.h"
#include "ui/UiPalette.h"
#include "ui/BackgroundImage.h"
#include "ui/AdaptiveTheme.h"
namespace ui {
namespace {
struct State { ControlMotion motion; unsigned frame=0,updated=0; Vector2 click{}; };
std::unordered_map<std::string,State> states;
unsigned frame=1;float dt=1.f/60;
std::string Key(Rectangle bounds,const char* label) {
    return std::to_string(std::lround(bounds.x))+":"+std::to_string(std::lround(bounds.y))+":"+
        std::to_string(std::lround(bounds.width))+":"+std::to_string(std::lround(bounds.height))+":"+(label?label:"");
}
Color Mix(Color a,Color b,float weight) {
    const auto blend=[&](unsigned char x,unsigned char y){return static_cast<unsigned char>(x+(y-x)*weight);};
    return {blend(a.r,b.r),blend(a.g,b.g),blend(a.b,b.b),blend(a.a,b.a)};
}
void Feedback(Rectangle bounds,const char* label,bool activated) {
    const auto effect=GetControlMotion(bounds,label,activated);
    if(GuiGetState()==STATE_DISABLED || GuiIsLocked()) return;
    const float scale=std::max(0.5f,GuiGetStyle(DEFAULT,TEXT_SIZE)/22.f);
    const float inset=effect.pressed*2*scale;
    Rectangle outline{bounds.x+inset,bounds.y+inset,bounds.width-2*inset,bounds.height-2*inset};
    DrawRectangleLinesEx(outline,scale,Fade(palette::accent,effect.hover*.40f+effect.pulse*.50f));
    if(effect.pulse>0) {
        const auto& state=states.at(Key(bounds,label));
        BeginScissorMode(static_cast<int>(bounds.x),static_cast<int>(bounds.y),static_cast<int>(bounds.width),static_cast<int>(bounds.height));
        DrawCircleLinesV(state.click,(1-effect.pulse)*bounds.width*.7f,Fade(palette::accent,effect.pulse*.6f));
        EndScissorMode();
    }
}
}
void BeginUiAnimations(float seconds) {
    dt=std::isfinite(seconds)&&seconds>0?std::min(seconds,.1f):0; ++frame;
    UpdateBackgroundAnimation(dt); UpdateThemeAnimation(dt);
    if(frame%120==0) for(auto it=states.begin();it!=states.end();) {
        if(frame-it->second.frame>600) it=states.erase(it); else ++it;
    }
}
ControlMotion GetControlMotion(Rectangle bounds,const char* label,bool activated) {
    auto& state=states[Key(bounds,label)]; state.frame=frame;
    const bool enabled=!GuiIsLocked() && GuiGetState()!=STATE_DISABLED;
    const bool hovered=enabled && CheckCollisionPointRec(GetMousePosition(),bounds);
    if(state.updated!=frame) {
        state.updated=frame;
        state.motion.hover=motion::Smooth(state.motion.hover,hovered?1.f:0.f,animationConfig::hoverRate,dt);
        state.motion.pressed=motion::Smooth(state.motion.pressed,hovered&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)?1.f:0.f,animationConfig::pressRate,dt);
        state.motion.pulse=std::max(0.f,state.motion.pulse-dt*animationConfig::pulseDecay);
    }
    if(activated || (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        state.motion.pulse=1; state.click=hovered?GetMousePosition():Vector2{bounds.x+bounds.width/2,bounds.y+bounds.height/2};
    }
    return state.motion;
}
int AnimatedButton(Rectangle bounds,const char* text) {
    const auto effect=GetControlMotion(bounds,text);
    constexpr std::array<int,6> properties{BORDER_COLOR_NORMAL,BASE_COLOR_NORMAL,BORDER_COLOR_FOCUSED,BASE_COLOR_FOCUSED,BORDER_COLOR_PRESSED,BASE_COLOR_PRESSED};
    std::array<int,6> saved{};
    for(std::size_t index=0;index<properties.size();++index) saved[index]=GuiGetStyle(BUTTON,properties[index]);
    const Color border=Mix(GetColor(saved[0]),GetColor(saved[2]),effect.hover);
    const Color base=Mix(Mix(GetColor(saved[1]),GetColor(saved[3]),effect.hover),GetColor(saved[5]),effect.pressed);
    for(int state=0;state<3;++state) {GuiSetStyle(BUTTON,properties[state*2],ColorToInt(border));GuiSetStyle(BUTTON,properties[state*2+1],ColorToInt(base));}
    const int result=GuiButton(bounds,text);
    for(std::size_t index=0;index<properties.size();++index) GuiSetStyle(BUTTON,properties[index],saved[index]);
    Feedback(bounds,text,result!=0); return result;
}
int AnimatedComboBox(Rectangle b,const char* t,int* v) { const int r=GuiComboBox(b,t,v);Feedback(b,t,r!=0);return r; }
int AnimatedCheckBox(Rectangle b,const char* t,bool* v) { const int r=GuiCheckBox(b,t,v);Feedback(b,t,r!=0);return r; }
int AnimatedSlider(Rectangle b,const char* l,const char* r,float* v,float min,float max) { const int result=GuiSlider(b,l,r,v,min,max);Feedback(b,l,result!=0);return result; }
int AnimatedValueBox(Rectangle b,const char* t,int* v,int min,int max,bool edit) { const int r=GuiValueBox(b,t,v,min,max,edit);Feedback(b,t,r!=0);return r; }
int AnimatedSpinner(Rectangle b,const char* t,int* v,int min,int max,bool edit) { const int r=GuiSpinner(b,t,v,min,max,edit);Feedback(b,t,r!=0);return r; }
}  // namespace ui
