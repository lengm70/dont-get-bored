#pragma once

#include "raylib.h"

namespace ui::palette {

// Shared violet theme for menus, controls, cards and game boards.
inline constexpr Color text{247, 240, 255, 255};
inline constexpr Color muted{190, 174, 212, 255};
inline constexpr Color accent{194, 151, 255, 255};
inline constexpr Color gradientTop{10, 7, 24, 255};
inline constexpr Color gradientBottom{27, 15, 46, 255};
inline constexpr Color panel{29, 20, 47, 244};
inline constexpr Color border{109, 80, 143, 255};
inline constexpr Color control{49, 33, 73, 255};
inline constexpr Color focused{67, 45, 94, 255};
inline constexpr Color pressed{89, 53, 123, 255};
inline constexpr Color card{39, 26, 58, 255};
inline constexpr Color cardFocused{52, 35, 76, 255};
inline constexpr Color cardPressed{68, 43, 94, 255};
inline constexpr Color cardBorder{84, 62, 110, 255};
inline constexpr Color shadow{10, 5, 22, 110};
inline constexpr Color board{14, 10, 26, 255};
inline constexpr Color grid{47, 36, 65, 255};
inline constexpr Color overlay{13, 8, 24, 225};

}  // namespace ui::palette
