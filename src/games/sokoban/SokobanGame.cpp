#include "games/sokoban/SokobanGame.h"
#include <algorithm>
#include <cmath>
namespace games::sokoban {
void SokobanGame::Reset(int level){level_=std::clamp(level,0,levelCount-1);phase_=Status::Ready;board_={};history_.clear();
 auto& def=GetLevel(level_);for(int y=0;y<rows;++y)for(int x=0;x<columns;++x){int i=y*columns+x;char c=def.map[y][x];walls_[i]=c=='#';goals_[i]=c=='.'||c=='+'||c=='*';board_.boxes[i]=c=='$'||c=='*';if(c=='@'||c=='+')board_.player={x,y};}
 previousPlayer_=board_.player;boxFrom_=boxTo_=-1;
}
void SokobanGame::Start(){if(phase_==Status::Ready)phase_=Status::Playing;}
void SokobanGame::TogglePause(){if(phase_==Status::Playing)phase_=Status::Paused;else if(phase_==Status::Paused)phase_=Status::Playing;}
bool SokobanGame::Free(Position p)const{return p.x>=0&&p.x<columns&&p.y>=0&&p.y<rows&&!walls_[p.y*columns+p.x];}
int SokobanGame::GoalsFilled()const{int n=0;for(int i=0;i<cellCount;++i)if(goals_[i]&&board_.boxes[i])++n;return n;}
bool SokobanGame::Move(int dx,int dy){
 if(phase_!=Status::Playing||std::abs(dx)+std::abs(dy)!=1)return false;
 Position next{board_.player.x+dx,board_.player.y+dy};if(!Free(next))return false;int index=next.y*columns+next.x;
 Position beyond{next.x+dx,next.y+dy};if(board_.boxes[index]&&(!Free(beyond)||board_.boxes[beyond.y*columns+beyond.x]))return false;
 history_.push_back(board_);previousPlayer_=board_.player;boxFrom_=boxTo_=-1;
 if(board_.boxes[index]){boxFrom_=index;boxTo_=beyond.y*columns+beyond.x;board_.boxes[index]=false;board_.boxes[boxTo_]=true;++board_.pushes;}
 board_.player=next;++board_.moves;
 bool solved=true;for(int i=0;i<cellCount;++i)if(goals_[i]&&!board_.boxes[i])solved=false;if(solved)phase_=Status::Won;return true;
}
bool SokobanGame::Undo(){
 if(history_.empty()||(phase_!=Status::Playing&&phase_!=Status::Won))return false;
 previousPlayer_=board_.player;State previous=history_.back();history_.pop_back();boxFrom_=boxTo_=-1;
 for(int i=0;i<cellCount;++i){if(board_.boxes[i]&&!previous.boxes[i])boxFrom_=i;if(!board_.boxes[i]&&previous.boxes[i])boxTo_=i;}
 board_=previous;phase_=Status::Playing;return true;
}
}
