#pragma once
#include "raylib.h"
namespace games::rhythm {
class RhythmAudio {
public:
 bool Start(int track,float volume);void Stop();void Pause(bool paused);void Update();double Time()const;bool Valid()const{return valid_;}
private:Music music_{};bool valid_=false;
};
}
