# v0.9 pre-1.0 verification

The current scope ends at v0.9.0. No v1.0 redesign, engine replacement, account system, networking or additional game is included.

## Functional gates

- 12 game entries and illustrations; Space starts games without an accidental gameplay action.
- F1 opens per-game controls and blocks gameplay; Esc closes help before leaving the game.
- P pauses every game at the application boundary, including AI turns and audio-clock rhythm play.
- Losing focus pauses gameplay; returning focus does not resume automatically. P resumes; R can restart while paused.
- Existing settings/highscores/Breakout saves retain their schema; new records use atomic config/progress.ini replacement.
- All 20 Sokoban maps replay a verified legal solution; undo includes winning moves.
- Each rhythm chart reaches a perfect result with exact timestamps; real audio playback/pause/resume was checked on all 3 original tracks.
- Each race circuit completes 3 ordered-checkpoint laps under ordinary control inputs; FPS consistency and pause are tested.
- Tank solo/co-op records are separate; collection goals import existing records read-only.
- Every published version is tested, built in Release, scanned for credentials/agent state, packaged with default assets and runtime DLLs, then smoke-launched without an MSYS2 PATH.

## Test boundaries

Windows x64 is the packaged platform. This run does not claim Linux/macOS verification, a complete human playthrough of every game, multiplayer keyboard rollover guarantees, or exhaustive usability testing. The test suite covers rules, state transitions, adapters, compatibility, assets and the shared session policy. Visual snapshots cover the four added games, goals page and help on 720p/1080p/1440p/2160p offscreen canvases. Window-size queries are supplied by the render harness so the physical monitor does not clamp the 4K layout test.

## Next work (requires a new request)

Plan the v1.0 visual/interaction redesign, playtest with people, tune difficulty and sound balance, and decide the final content freeze. Do not automatically begin these changes.
