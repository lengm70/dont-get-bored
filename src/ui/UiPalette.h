#pragma once

#include "raylib.h"

namespace ui::palette {

// Semantic UI colors. Gameplay piece/symbol colors remain independent.
struct Colors {
    Color text{247, 240, 255, 255};
    Color muted{190, 174, 212, 255};
    Color accent{194, 151, 255, 255};
    Color gradientTop{10, 7, 24, 255};
    Color gradientBottom{27, 15, 46, 255};
    Color panel{29, 20, 47, 244};
    Color border{109, 80, 143, 255};
    Color control{49, 33, 73, 255};
    Color focused{67, 45, 94, 255};
    Color pressed{89, 53, 123, 255};
    Color card{39, 26, 58, 255};
    Color cardFocused{52, 35, 76, 255};
    Color cardPressed{68, 43, 94, 255};
    Color cardBorder{84, 62, 110, 255};
    Color shadow{10, 5, 22, 110};
    Color board{14, 10, 26, 255};
    Color grid{47, 36, 65, 255};
    Color overlay{13, 8, 24, 225};
};

inline Colors current{};
inline unsigned revision = 0;
inline const Color& text = current.text;
inline const Color& muted = current.muted;
inline const Color& accent = current.accent;
inline const Color& gradientTop = current.gradientTop;
inline const Color& gradientBottom = current.gradientBottom;
inline const Color& panel = current.panel;
inline const Color& border = current.border;
inline const Color& control = current.control;
inline const Color& focused = current.focused;
inline const Color& pressed = current.pressed;
inline const Color& card = current.card;
inline const Color& cardFocused = current.cardFocused;
inline const Color& cardPressed = current.cardPressed;
inline const Color& cardBorder = current.cardBorder;
inline const Color& shadow = current.shadow;
inline const Color& board = current.board;
inline const Color& grid = current.grid;
inline const Color& overlay = current.overlay;

}  // namespace ui::palette
