#pragma once
#include <algorithm>
#include <cmath>
#include "ui/AnimationConfig.h"
namespace ui::motion {
inline constexpr float pi=3.14159265359f;
inline float Smooth(float value,float target,float rate,float dt) {
    if(!std::isfinite(dt) || dt<=0) return value;
    return value+(target-value)*(1-std::exp(-rate*std::min(dt,0.1f)));
}
inline float Ease(float progress) { const float t=std::clamp(progress,0.f,1.f);return 1-(1-t)*(1-t)*(1-t); }
inline float Advance(float progress,float duration,float dt) {
    return std::clamp(progress+(std::isfinite(dt)&&dt>0?std::min(dt,0.1f)/duration:0),0.f,1.f);
}
inline float AngleDelta(float from,float to) { return std::remainder(to-from,2*pi); }
class Orbit {
public:
    void Focus(int seat,int count) {
        if(seat<0 || count<1) return;
        target_=-seat*2*pi/count;
        if(!initialized_) { angle_=target_; initialized_=true; }
    }
    void Update(float dt) { angle_=Smooth(angle_,angle_+AngleDelta(angle_,target_),animationConfig::orbitRate,dt); }
    float Angle() const { return angle_; }
    bool Moving() const { return std::abs(AngleDelta(angle_,target_))>0.015f; }
private:
    float angle_=0,target_=0; bool initialized_=false;
};
}  // namespace ui::motion
