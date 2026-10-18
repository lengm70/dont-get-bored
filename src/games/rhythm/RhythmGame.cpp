#include "games/rhythm/RhythmGame.h"
#include <algorithm>
#include <cmath>
namespace games::rhythm {
void RhythmGame::Reset(int track){track_=std::clamp(track,0,trackCount-1);notes_=MakeChart(track_);state_=Status::Ready;score_=combo_=maxCombo_=perfects_=goods_=misses_=ghosts_=0;time_=0;}
void RhythmGame::Start(){if(state_==Status::Ready)state_=Status::Playing;}
void RhythmGame::TogglePause(){if(state_==Status::Playing)state_=Status::Paused;else if(state_==Status::Paused)state_=Status::Playing;}
void RhythmGame::Advance(double time){if(state_!=Status::Playing||!std::isfinite(time)||time<time_)return;time_=time;
 for(auto& n:notes_)if(n.result<0&&time_-n.time>goodWindow+.00001){n.result=0;++misses_;combo_=0;}
 if(time_>notes_.back().time+.6)state_=Status::Won;
}
int RhythmGame::Hit(int lane){if(state_!=Status::Playing||lane<0||lane>=4)return -1;int found=-1;double distance=goodWindow+.00001;
 for(int i=0;i<int(notes_.size());++i){const auto& n=notes_[i];double d=std::abs(n.time-time_);if(n.result<0&&n.lane==lane&&d<distance){distance=d;found=i;}}
 if(found<0){++ghosts_;combo_=0;score_=std::max(0,score_-25);return 0;}
 int points=distance<=perfectWindow+.00001?100:60;notes_[found].result=points;if(points==100)++perfects_;else ++goods_;
 ++combo_;maxCombo_=std::max(maxCombo_,combo_);score_+=points*(1+std::min(3,combo_/20));return points;
}
float RhythmGame::Accuracy()const{double denominator=notes_.size()*100.0+ghosts_*30.0;return denominator?float((perfects_*100.0+goods_*60.0)/denominator*100):0;}
const char* RhythmGame::Grade()const{float a=Accuracy();return a>=97?"S":a>=90?"A":a>=80?"B":a>=65?"C":"D";}
}
