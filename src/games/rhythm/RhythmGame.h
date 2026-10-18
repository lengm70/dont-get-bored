#pragma once
#include "games/rhythm/RhythmCharts.h"
namespace games::rhythm {
enum class Status{Ready,Playing,Paused,Won};
class RhythmGame{
public:
 RhythmGame(){Reset();}void Reset(int track=0);void Start();void TogglePause();void Advance(double songSeconds);int Hit(int lane);
 Status State()const{return state_;}int TrackIndex()const{return track_;}double Time()const{return time_;}
 const auto& Notes()const{return notes_;}int Score()const{return score_;}int Combo()const{return combo_;}int MaxCombo()const{return maxCombo_;}
 int Perfects()const{return perfects_;}int Goods()const{return goods_;}int Misses()const{return misses_;}int Ghosts()const{return ghosts_;}
 float Accuracy()const;const char* Grade()const;
private:
 Status state_=Status::Ready;std::vector<Note> notes_;int track_=0,score_=0,combo_=0,maxCombo_=0,perfects_=0,goods_=0,misses_=0,ghosts_=0;double time_=0;
};
}
