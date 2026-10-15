#include "games/bomber/BomberVisual.h"
#include <algorithm>
#include <cmath>
#include "games/bomber/BomberConfig.h"
#include "ui/UiPalette.h"
namespace games::bomber {
namespace {
constexpr float pi = 3.14159265f;
Vector2 Center(Point point, const ui::UiLayout& layout) {
    return layout.Point(config::boardX + point.x * config::tileSize,config::boardY + point.y * config::tileSize);
}
Vector2 Center(Cell cell, const ui::UiLayout& layout) { return Center(Point{cell.x + 0.5f,cell.y + 0.5f},layout); }
Rectangle CellRect(Cell cell, const ui::UiLayout& layout, float inset = config::cellInset) {
    return layout.Rect({config::boardX + cell.x * config::tileSize + inset,
        config::boardY + cell.y * config::tileSize + inset,config::tileSize - inset * 2,config::tileSize - inset * 2});
}
void DrawCore(Vector2 center, float radius, Color color, float clock) {
    const float pulse = 1 + 0.08f * std::sin(clock * config::pulseRate);
    DrawPoly(center,6,radius * 1.5f * pulse,30,Fade(color,0.1f));
    DrawPoly(center,6,radius * pulse,30,Fade(color,0.3f));
    DrawPolyLinesEx(center,6,radius * pulse,30,std::max(1.0f,radius * 0.12f),color);
    DrawPoly(center,4,radius * 0.38f,45,WHITE);
}
Color UpgradeColor(Upgrade kind) {
    return kind == Upgrade::Capacity ? config::capacity : kind == Upgrade::Range ? config::range : config::speed;
}
void DrawPickup(const Pickup& item, const BomberGame& game, const ui::UiLayout& layout) {
    if (item.protection > 0) return;
    auto center = Center(item.cell,layout);
    const float unit = config::tileSize * layout.scale;
    center.y += std::sin(game.Clock() * 4 + item.cell.x) * unit * 0.06f;
    const Color color = UpgradeColor(item.kind);
    DrawPoly(center,4,unit * 0.27f,0,Fade(color,0.15f));
    DrawPolyLinesEx(center,4,unit * 0.27f,0,layout.scale,color);
    if (item.kind == Upgrade::Capacity) {
        DrawCircleV({center.x - unit * 0.08f,center.y},unit * 0.04f,color);
        DrawCircleV({center.x + unit * 0.08f,center.y},unit * 0.04f,color);
    } else {
        const float length = unit * 0.13f;
        DrawLineEx({center.x - length,center.y + length},{center.x,center.y - length},layout.scale,color);
        DrawLineEx({center.x,center.y - length},{center.x + length,center.y + length},layout.scale,color);
        if (item.kind == Upgrade::Range) DrawLineEx({center.x,center.y - length},{center.x,center.y + length},layout.scale,color);
    }
}
void DrawActor(const Actor& actor, int index, float clock, const ui::UiLayout& layout) {
    if (!actor.alive) return;
    const auto center = Center(actor.position,layout);
    const float radius = rules::actorRadius * config::tileSize * layout.scale;
    const Color color = !actor.human ? config::enemy : index == 0 ? config::playerOne : config::playerTwo;
    DrawPoly({center.x,center.y + radius * 0.2f},6,radius * 1.15f,30,Fade(BLACK,0.35f));
    DrawPoly(center,6,radius,30,Fade(color,0.22f));
    DrawPolyLinesEx(center,6,radius,30,layout.scale,color);
    Cell facing = Offset(actor.facing);
    if (actor.facing == Direction::None) facing = {0,1};
    const Vector2 eye{center.x + facing.x * radius * 0.35f,center.y + facing.y * radius * 0.35f};
    DrawLineEx({eye.x - facing.y * radius * 0.4f,eye.y + facing.x * radius * 0.4f},
               {eye.x + facing.y * radius * 0.4f,eye.y - facing.x * radius * 0.4f},radius * 0.22f,WHITE);
    if (!actor.human) DrawPolyLinesEx(center,3,radius * 1.3f,clock * 35,layout.scale,Fade(color,0.4f));
    else {
        const int marks = index == 0 ? 1 : 2;
        for (int i = 0; i < marks; ++i)
            DrawCircleV({center.x + (i - (marks - 1) / 2.0f) * radius * 0.5f,center.y + radius * 1.5f},radius * 0.12f,color);
    }
}
} // namespace
void DrawArena(const BomberGame& game, const ui::UiLayout& layout) {
    const float unit = config::tileSize * layout.scale;
    for (int y = 0; y < rules::rows; ++y) for (int x = 0; x < rules::columns; ++x) {
        const Cell cell{x,y}; const Rectangle rect = CellRect(cell,layout);
        const Tile tile = game.Map().At(cell);
        DrawRectangleRec(rect,tile == Tile::Wall ? Fade(config::wall,0.35f) : tile == Tile::Crate ? Fade(config::crate,0.22f) : Fade(ui::palette::grid,0.16f));
        DrawRectangleLinesEx(rect,layout.scale,tile == Tile::Wall ? Fade(config::wall,0.75f) : tile == Tile::Crate ? config::crate : Fade(ui::palette::grid,0.35f));
        if (tile == Tile::Crate) {
            const float cut = unit * 0.18f;
            DrawLineEx({rect.x + cut,rect.y + cut},{rect.x + rect.width - cut,rect.y + rect.height - cut},layout.scale,Fade(config::crate,0.7f));
            DrawLineEx({rect.x + cut,rect.y + rect.height - cut},{rect.x + rect.width - cut,rect.y + cut},layout.scale,Fade(config::crate,0.7f));
        } else if (tile == Tile::Wall) {
            DrawLineEx({rect.x + unit * 0.15f,rect.y + unit * 0.15f},{rect.x + rect.width - unit * 0.15f,rect.y + unit * 0.15f},layout.scale,config::wall);
        }
    }
    for (int i = 0; i < static_cast<int>(game.Bombs().size()); ++i) {
        const auto& bomb = game.Bombs()[i];
        const Color color = bomb.owner == 0 ? config::playerOne : config::playerTwo;
        for (Cell cell : BlastCells(game.Map(),game.Bombs(),i)) {
            DrawRectangleRec(CellRect(cell,layout,5),Fade(color,0.065f));
            DrawRectangleLinesEx(CellRect(cell,layout,5),layout.scale,Fade(color,0.23f));
        }
        const auto center = Center(bomb.cell,layout);
        DrawCore(center,unit * config::bombRadius,color,game.Clock() + i);
        DrawRing(center,unit * 0.33f,unit * 0.36f,-90,-90 + 360 * bomb.remaining / rules::fuseSeconds,32,Fade(color,0.8f));
    }
    for (const auto& pickup : game.Pickups()) DrawPickup(pickup,game,layout);
    for (int i = 0; i < static_cast<int>(game.Actors().size()); ++i) DrawActor(game.Actors()[i],i,game.Clock(),layout);
    for (const auto& fire : game.Flames()) {
        const float strength = std::clamp(fire.remaining / rules::flameSeconds,0.0f,1.0f);
        const auto center = Center(fire.cell,layout);
        DrawRectangleRec(CellRect(fire.cell,layout,1),Fade(config::flame,0.22f * strength));
        DrawLineEx({center.x - unit * 0.47f,center.y},{center.x + unit * 0.47f,center.y},unit * 0.23f,Fade(config::flame,strength));
        DrawLineEx({center.x,center.y - unit * 0.47f},{center.x,center.y + unit * 0.47f},unit * 0.23f,Fade(config::flame,strength));
        DrawPoly(center,4,unit * 0.12f,45,Fade(WHITE,strength));
    }
    for (const auto& debris : game.Fragments()) {
        const float t = 1 - debris.remaining / rules::flameSeconds;
        const auto center = Center(debris.cell,layout);
        const Color color = debris.upgrade == Upgrade::None ? config::flame : UpgradeColor(debris.upgrade);
        if (debris.upgrade != Upgrade::None) DrawPolyLinesEx(center,6,unit * (0.2f + t * 0.6f),30,layout.scale,Fade(color,1 - t));
        for (int i = 0; i < 6; ++i) {
            const float angle = i * pi / 3 + debris.cell.x;
            const Vector2 point{center.x + std::cos(angle) * unit * t * config::fragmentDistance,
                center.y + std::sin(angle) * unit * t * config::fragmentDistance};
            DrawPoly(point,3,unit * 0.07f * (1 - t),t * 180,Fade(color,1 - t));
        }
    }
}
void DrawBomberIcon(Rectangle bounds) {
    const float size = bounds.width;
    const Vector2 center{bounds.x + size / 2,bounds.y + bounds.height / 2};
    DrawLineEx({center.x - size * 0.38f,center.y},{center.x + size * 0.38f,center.y},size * 0.08f,Fade(config::flame,0.6f));
    DrawLineEx({center.x,center.y - size * 0.38f},{center.x,center.y + size * 0.38f},size * 0.08f,Fade(config::flame,0.6f));
    DrawCore(center,size * 0.18f,config::playerOne,0);
    for (float side : {-1.0f,1.0f}) DrawPolyLinesEx({center.x + side * size * 0.3f,center.y + side * size * 0.3f},4,size * 0.12f,45,1.5f,config::crate);
}
} // namespace games::bomber
