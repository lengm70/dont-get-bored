#include "games/rhythm/RhythmAudio.h"
#include "games/rhythm/RhythmCharts.h"
#include <algorithm>
namespace games::rhythm {
bool RhythmAudio::Start(int track,float volume){Stop();if(!IsAudioDeviceReady())return false;music_=LoadMusicStream(GetTrack(track).path);valid_=IsMusicValid(music_);if(!valid_)return false;music_.looping=false;SetMusicVolume(music_,std::clamp(volume,0.f,1.f));PlayMusicStream(music_);return true;}
void RhythmAudio::Stop(){if(valid_){StopMusicStream(music_);UnloadMusicStream(music_);valid_=false;}}
void RhythmAudio::Pause(bool value){if(!valid_)return;if(value)PauseMusicStream(music_);else ResumeMusicStream(music_);}
void RhythmAudio::Update(){if(valid_)UpdateMusicStream(music_);}
double RhythmAudio::Time()const{return valid_?GetMusicTimePlayed(music_):0;}
}
