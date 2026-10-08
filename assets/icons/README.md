# Application icon

`app.png` is the original transparent artwork generated using the built-in imagegen tool.
`app.ico` packages the same artwork at 16, 24, 32, 48, 64, 128 and 256 pixels for Windows.

The application sets its window icon after opening the window. Windows builds also embed
the ICO in the executable through `resources/AppIcon.rc.in`, including the named
`GLFW_ICON` resource used when the window is first created. A process AppUserModelID
is assigned before opening the window so the game has its own taskbar group.
Windows builds use the GUI subsystem to avoid an additional console taskbar button.
Keep `assets/icons/app.png`
in the runtime assets folder. Missing runtime artwork produces a warning without blocking startup.

## Generation prompt

Use case: logo-brand. Asset type: a single square desktop application icon for a retro mini-game collection with a purple cosmic pixel-art theme. Create one centered bold game controller silhouette in bright lavender and violet, with a simple dark cross-shaped D-pad on the left and two pale luminous buttons on the right. A small four-point star above the controller subtly conveys the space theme. Crisp intentional pixel-art edges, chunky shapes, a very limited purple / pale-lavender / midnight-indigo palette, clean strong silhouette readable at 16x16 and 32x32. Use a solid dark-indigo rounded-square tile behind the controller, with transparent canvas outside its rounded corners. The icon fills most of the square with safe margins, front-facing composition. Flat polished arcade style, no photographic rendering, no soft blur, no tiny decorative texture, no dense stars, no text, no letters, no wordmark, no watermark, no mockup, no surrounding objects. Output exactly one icon with true alpha transparency outside the tile.
