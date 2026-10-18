#include "games/tank/TankScreen.h"
#include <cmath>
namespace games::tank {
void UpdateFromInput(TankGame& g,float dt,app::ProgressRecords& records){
    if(IsKeyPressed(KEY_R)){g.Reset(g.Cooperative());return;}
    if(g.State()==Status::Ready&&IsKeyPressed(KEY_TAB)){g.Reset(!g.Cooperative());return;}
    if(g.State()==Status::Ready&&IsKeyPressed(KEY_SPACE)){g.Start();return;}
    if(IsKeyPressed(KEY_P)){g.TogglePause();return;}
    std::array<Input,2> in{};const int keys[2][4]={{KEY_W,KEY_D,KEY_S,KEY_A},{KEY_UP,KEY_RIGHT,KEY_DOWN,KEY_LEFT}};
    for(int p=0;p<(g.Cooperative()?2:1);++p){
        for(int i=0;i<4;++i)if(IsKeyDown(keys[p][i])||(!g.Cooperative()&&IsKeyDown(keys[1][i]))){in[p].direction=i;break;}
        in[p].fire=p==0?(IsKeyDown(KEY_F)||IsKeyDown(KEY_SPACE)):IsKeyDown(KEY_ENTER);
    }
    g.Update(dt,in);const char* key=g.Cooperative()?"tank.coop.best":"tank.best";
    records.Best(key,g.Score());if(g.State()==Status::Won)records.Best("tank.clear",1);
}
void DrawScreen(const TankGame& g,Font font,app::Language lang,const app::ProgressRecords& records){
    using namespace arcade;auto l=ui::CurrentLayout();bool zh=Chinese(lang);
    std::string stats=std::string(zh?"波次 ":"Wave ")+std::to_string(g.Wave())+"/3   "+(zh?"基地 ":"Base ")+std::to_string(g.BaseHealth())+"   "+(zh?"分数 ":"Score ")+std::to_string(g.Score())+"   "+(zh?"最佳 ":"Best ")+std::to_string(records.Get(g.Cooperative()?"tank.coop.best":"tank.best"));
    Header(font,zh?"坦克大战":"Tank Battle",stats,l);
    std::string hp="P1 HP "+std::to_string(g.Players()[0].health)+(g.Cooperative()?"   P2 HP "+std::to_string(g.Players()[1].health):"");Text(font,hp,121,15,cyan,l);
    auto boardRect=l.Rect(board);DrawRectangleRec(boardRect,ui::palette::board);
    BeginScissorMode(int(boardRect.x),int(boardRect.y),int(boardRect.width),int(boardRect.height));
    for(int i=0;i<rules::count;++i)if(g.Walls()[i]){
        Rectangle r=l.Rect({board.x+(i%rules::columns)*rules::cell+1,board.y+(i/rules::columns)*rules::cell+1,28,28});Color c=g.Walls()[i]==1?gold:ui::palette::accent;
        DrawRectangleRec(r,Fade(c,.18f));DrawRectangleLinesEx(r,l.scale,Fade(c,.7f));
    }
    auto drawTank=[&](const Tank& t,Color c){if(t.health<=0)return;auto center=Point(t.x,t.y,l);
        Rectangle body{center.x-10*l.scale,center.y-10*l.scale,20*l.scale,20*l.scale};DrawRectangleRec(body,Fade(c,.35f));DrawRectangleLinesEx(body,l.scale,t.flash>0?WHITE:c);
        const int dx[]{0,1,0,-1},dy[]{-1,0,1,0};DrawLineEx(center,Point(t.x+dx[t.direction]*18,t.y+dy[t.direction]*18,l),4*l.scale,c);
        DrawCircleV(center,4*l.scale,c);
    };
    drawTank(g.Players()[0],cyan);drawTank(g.Players()[1],pink);for(const auto& e:g.Enemies())drawTank(e,gold);
    DrawPoly(Point(300,330,l),6,14*l.scale,30,Fade(cyan,.3f));DrawPolyLinesEx(Point(300,330,l),6,14*l.scale,30,2*l.scale,cyan);
    for(const auto& b:g.Bullets())DrawCircleV(Point(b.x,b.y,l),3*l.scale,b.enemy?gold:cyan);
    for(const auto& hit:g.Impacts())DrawPolyLinesEx(Point(hit.x,hit.y,l),6,(4+hit.age*40)*l.scale,hit.age*90,l.scale,Fade(gold,1-hit.age/.4f));
    Overlay(static_cast<arcade::Status>(g.State()),font,lang,l,g.State()==Status::Ready?(g.Cooperative()?(zh?"双人合作 / Tab 切换单人":"Co-op / Tab switches to solo"):(zh?"单人 / Tab 切换合作模式":"Solo / Tab switches to co-op")):stats);
    EndScissorMode();DrawRectangleLinesEx(boardRect,l.scale,ui::palette::accent);
    Footer(font,g.Cooperative()?"P1: WASD + F / Space   P2: Arrows + Enter":(zh?"WASD / 方向键移动   F / 空格开火":"WASD / Arrows move   F / Space fire"),lang,l);
}
}
