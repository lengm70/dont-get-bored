#include <cmath>
#include <iostream>
#include <stdexcept>
#include "ui/Motion.h"
#include "games/twentyone/TwentyOnePresentation.h"
namespace {
void Require(bool value,const char* message) {if(!value) throw std::runtime_error(message);}
}
int main() {
    try {
        float slow=0,fast=0;
        for(int frame=0;frame<60;++frame) slow=ui::motion::Smooth(slow,1,12,1.f/60);
        for(int frame=0;frame<240;++frame) fast=ui::motion::Smooth(fast,1,12,1.f/240);
        Require(std::abs(slow-fast)<.0001f,"Animation convergence is independent of frame rate");
        Require(ui::motion::Smooth(0,1,12,-1)==0,"Invalid time does not reverse animation");
        Require(std::abs(ui::motion::AngleDelta(3.1f,-3.1f))<.1f,"Camera wraps by shortest path");
        ui::motion::Orbit camera;camera.Focus(0,4);camera.Focus(1,4);
        Require(camera.Moving(),"New seat starts camera turn");
        camera.Update(1.f/60); Require(std::abs(camera.Angle())>.01f && std::abs(camera.Angle())<ui::motion::pi/2,"Camera moves continuously rather than jumping");
        for(int frame=0;frame<120;++frame) camera.Update(1.f/60);
        Require(!camera.Moving(),"Camera turn settles");
        games::twentyone::MatchOptions options;options.seats=4;options.ai.fill(false);
        games::twentyone::TwentyOneMatch match(42); match.Start(options);
        games::twentyone::Presentation presentation;presentation.Observe(match,0);
        Require(presentation.Busy(),"Dealing prevents premature input");
        for(int frame=0;frame<60;++frame) presentation.Observe(match,1.f/60);
        Require(!presentation.Busy(),"Input resumes after dealing");
        match.Act(games::twentyone::MatchAction::Hold);presentation.Observe(match,1.f/60);
        Require(presentation.orbit.Moving() && presentation.focused==0,"Old foreground fades during camera turn");
        for(int frame=0;frame<120;++frame) presentation.Observe(match,1.f/60);
        Require(presentation.focused==1 && !presentation.Busy(),"Turn reaches next seat and enables input");
        while(match.Phase()==games::twentyone::MatchPhase::Playing) match.Act(games::twentyone::MatchAction::Hold);
        presentation.Observe(match,0);Require(presentation.result==0,"Result entrance starts at zero");
        presentation.Observe(match,.1f);Require(presentation.result>0 && presentation.result<1,"Result entrance progresses gradually");
        std::cout<<"Motion and presentation tests passed\n";
    } catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 1;}
}
