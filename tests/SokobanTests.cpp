#include "games/sokoban/SokobanGame.h"
#include <iostream>
#include <stdexcept>
using namespace games::sokoban;
void Check(bool b,const char*m){if(!b)throw std::runtime_error(m);}
int main(){try{
 for(int level=0;level<levelCount;++level){SokobanGame g;g.Reset(level);Check(!g.Move(1,0),"Ready blocked");g.Start();int steps=0;
  for(char c:std::string(GetLevel(level).solution)){int dx=c=='R'?1:c=='L'?-1:0,dy=c=='D'?1:c=='U'?-1:0;if(g.Phase()==Status::Won)break;Check(g.Move(dx,dy),"Known inverse-pull solution must be legal");++steps;}
  Check(g.Phase()==Status::Won,"Every shipped level must solve");Check(g.Undo()&&g.Phase()==Status::Playing,"Undo from win");Check(g.Board().moves==steps-1,"Undo restores move count");
 }
 SokobanGame g;g.Start();auto initial=g.Board();Check(!g.Move(1,1),"No diagonal push");g.TogglePause();Check(!g.Move(1,0),"Pause blocked");g.TogglePause();Check(g.Move(1,0)&&g.Undo()&&g.Board().boxes==initial.boxes&&g.Board().player==initial.player,"Undo restores all state");std::cout<<"20 puzzle solutions verified\n";
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
