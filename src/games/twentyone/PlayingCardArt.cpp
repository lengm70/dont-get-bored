#include "games/twentyone/PlayingCardArt.h"
#include <algorithm>
#include <array>
#include "games/twentyone/TwentyOneArtConfig.h"
#include "ui/GeometricPanel.h"
#include "ui/UiPalette.h"
namespace games::twentyone {
namespace {
struct Canvas {
    Rectangle bounds; float scale;
    Vector2 P(float x,float y) const { return {bounds.x+x*scale,bounds.y+y*scale}; }
    void Line(float x,float y,float endX,float endY,float width,Color color) const {
        DrawLineEx(P(x,y),P(endX,endY),width*scale,color);
    }
    template<std::size_t N> void Polygon(const std::array<Vector2,N>& points,Color color) const {
        const auto first=P(points[0].x,points[0].y);
        for(std::size_t index=1; index+1<N; ++index) {
            auto a=P(points[index].x,points[index].y), b=P(points[index+1].x,points[index+1].y);
            if((a.x-first.x)*(b.y-first.y)-(a.y-first.y)*(b.x-first.x)>0) std::swap(a,b);
            DrawTriangle(first,a,b,color);
        }
    }
};
Color SuitColor(TwentyOneSuit suit) {
    return suit==TWENTY_ONE_HEARTS || suit==TWENTY_ONE_DIAMONDS ? art::warmSuit : art::coolSuit;
}
void Suit(const Canvas& canvas,TwentyOneSuit suit,float x,float y,float size,bool inverted=false) {
    const float flip=inverted?-1.f:1.f;
    const Color color=SuitColor(suit);
    const auto p=[&](float dx,float dy){return Vector2{x+dx*size,y+dy*size*flip};};
    const auto diamond=[&](float dx,float dy,float radius) {
        const std::array<Vector2,4> vertices{{p(dx,dy-radius),p(dx+radius,dy),p(dx,dy+radius),p(dx-radius,dy)}};
        canvas.Polygon(vertices,color);
    };
    if(suit==TWENTY_ONE_DIAMONDS) diamond(0,0,0.58f);
    else if(suit==TWENTY_ONE_HEARTS) {
        const std::array<Vector2,6> vertices{{p(-.5f,-.18f),p(-.25f,-.48f),p(0,-.24f),p(.25f,-.48f),p(.5f,-.18f),p(0,.58f)}};
        canvas.Polygon(vertices,color);
    } else if(suit==TWENTY_ONE_SPADES) {
        const std::array<Vector2,5> vertices{{p(0,-.62f),p(.5f,.08f),p(.26f,.3f),p(-.26f,.3f),p(-.5f,.08f)}};
        canvas.Polygon(vertices,color);
        const std::array<Vector2,3> stem{{p(0,.12f),p(.2f,.62f),p(-.2f,.62f)}}; canvas.Polygon(stem,color);
    } else {
        diamond(0,-.28f,.28f); diamond(-.3f,.08f,.28f); diamond(.3f,.08f,.28f);
        const std::array<Vector2,3> stem{{p(0,.12f),p(.2f,.65f),p(-.2f,.65f)}}; canvas.Polygon(stem,color);
    }
}
}
void DrawPlayingCard(TwentyOneCard card,Rectangle bounds,Font font) {
    const Canvas canvas{bounds,bounds.width/art::cardWidth};
    const Color tone=SuitColor(card.suit);
    Rectangle shadow=bounds; shadow.x+=3*canvas.scale; shadow.y+=5*canvas.scale;
    ui::DrawGeometricPanel(shadow,art::cornerCut*canvas.scale,Fade(BLACK,.45f),BLANK,0);
    ui::DrawGeometricPanel(bounds,art::cornerCut*canvas.scale,ui::palette::board,Fade(tone,.75f),1.5f*canvas.scale);
    for(float y=art::gridStep;y<art::cardHeight;y+=art::gridStep)
        canvas.Line(9,y,151,y,.5f,Fade(tone,.07f));
    for(float x=art::gridStep;x<art::cardWidth;x+=art::gridStep)
        canvas.Line(x,9,x,215,.5f,Fade(tone,.07f));
    canvas.Line(34,8,83,8,2,tone); canvas.Line(77,216,126,216,2,tone);
    canvas.Line(7,60,7,93,2,Fade(tone,.5f)); canvas.Line(153,131,153,164,2,Fade(tone,.5f));
    const char* rank=card.rank==1?"A":card.rank==11?"J":card.rank==12?"Q":card.rank==13?"K":TextFormat("%d",card.rank);
    DrawTextEx(font,rank,canvas.P(10,12),art::cornerRankSize*canvas.scale,canvas.scale,tone);
    DrawTextPro(font,rank,canvas.P(150,212),{0,0},180,art::cornerRankSize*canvas.scale,canvas.scale,tone);
    Suit(canvas,card.suit,21,49,16); Suit(canvas,card.suit,139,175,16,true);
    if(card.rank>=11) {
        ui::DrawGeometricPanel({bounds.x+35*canvas.scale,bounds.y+65*canvas.scale,90*canvas.scale,94*canvas.scale},
            15*canvas.scale,Fade(tone,.08f),Fade(tone,.45f),canvas.scale);
        // Rank-specific concentric geometry replaces painted court portraits.
        const auto center=canvas.P(80,112); const float radius=(26+(card.rank-11)*4)*canvas.scale;
        DrawPolyLinesEx(center,card.rank-6,radius,45,2*canvas.scale,tone);
        DrawPolyLinesEx(center,4,radius*.65f,0,canvas.scale,Fade(tone,.5f));
        Suit(canvas,card.suit,80,112,30);
    } else if(card.rank==1) Suit(canvas,card.suit,80,112,66);
    else {
        const auto pip=[&](float x,float y){Suit(canvas,card.suit,x,y,25,y>112);};
        if(card.rank<=3) {pip(80,57);pip(80,167);if(card.rank==3)pip(80,112);}
        else {
            pip(51,57);pip(109,57);pip(51,167);pip(109,167);
            if(card.rank==5)pip(80,112);
            if(card.rank>=6 && card.rank<=8){pip(51,112);pip(109,112);}
            if(card.rank==7 || card.rank==8)pip(80,84);
            if(card.rank==8)pip(80,140);
            if(card.rank>=9){pip(51,94);pip(109,94);pip(51,130);pip(109,130);}
            if(card.rank==9)pip(80,112);
            if(card.rank==10){pip(80,75);pip(80,149);}
        }
    }
}
}  // namespace games::twentyone
