# Roadmap to v0.9

Stop after v0.9.0. The v1.0 visual redesign and large architectural changes are outside this run.

| Version | Deliverable |
|---|---|
| 0.4.0 | Tank battle: solo/local co-op, three waves, destructible/solid walls, enemy tanks, base defence, persistent best. |
| 0.5.0 | Sokoban: 20 verified-solvable puzzles, undo, restart, unlocked stages and best move records. |
| 0.6.0 | Rhythm: three original synthesized tracks, four lanes, timing grades, combos, accuracy and saved ratings. |
| 0.7.0 | Space race: three circuits, three laps, acceleration/braking/drift, checkpoints and persistent best times. |
| 0.8.0 | Collection progress dashboard and clear per-game goals, reading existing and new records. |
| 0.9.0 | Pre-1.0 integration fixes: help/pause consistency, focus safety, record compatibility and portable-package regression checks. |

Each completed version is tested, committed to private main, synchronized to public main, tagged, and published as a stable Release on both repositories. Each release includes the game EXE, complete Windows ZIP, source ZIP and SHA-256 checksums. Never publish personal configuration, imported media, agent state or credentials.

## Implementation status at v0.9

Versions 0.4.0–0.8.0 have been individually published to both repositories. The v0.9.0 implementation and verification are complete; tools/release.py performs its final publication. Development stops after that release. No v1.0 changes are authorized by this roadmap.
