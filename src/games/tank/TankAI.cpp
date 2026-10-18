#include "games/tank/TankGame.h"
#include <algorithm>
#include <cmath>
namespace games::tank {
void TankGame::EnemyAI(float dt){
    spawnTimer_-=dt;
    if(spawnLeft_>0&&spawnTimer_<=0){
        float x=45+255*(spawnIndex_%3);bool clear=std::none_of(enemies_.begin(),enemies_.end(),[x](const Tank&e){return std::hypot(e.x-x,e.y-45)<35;});
        if(clear){enemies_.push_back({x,45,2,wave_==3?2:1,1.f,.5f,0});--spawnLeft_;++spawnIndex_;spawnTimer_=2.3f;}
    }
    for(auto& e:enemies_){
        e.cooldown=std::max(0.f,e.cooldown-dt);e.flash=std::max(0.f,e.flash-dt);e.think-=dt;
        float tx=300,ty=330;for(const auto& p:players_)if(p.health>0&&std::hypot(e.x-p.x,e.y-p.y)<170){tx=p.x;ty=p.y;break;}
        if(e.think<=0){
            float x=tx-e.x,y=ty-e.y;int preferred=std::abs(x)>std::abs(y)?(x>0?1:3):(y>0?2:0);
            static constexpr int dx[]{0,1,0,-1},dy[]{-1,0,1,0};e.direction=preferred;
            for(int n=0;n<4;++n){int dir=(preferred+(spawnIndex_%2?n:4-n))%4;if(Walkable(e.x+dx[dir]*16,e.y+dy[dir]*16)){e.direction=dir;break;}}
            if(std::abs(x)<18)e.direction=y>0?2:0;else if(std::abs(y)<18)e.direction=x>0?1:3;
            e.think=.8f;
        }
        Fire(e,true);float x=e.x,y=e.y;Move(e,e.direction,rules::enemySpeed,dt);if(x==e.x&&y==e.y)e.think=0;
    }
}
}
