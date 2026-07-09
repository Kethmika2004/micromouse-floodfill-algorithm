# ROBOFEST 2026 Micromouse Solver — Modified Flood Fill

Targets the [`mms`](https://github.com/mackorone/mms) simulator (mackorone/mms).
Tested end-to-end against a mock protocol server (`fake_mms.py`, included) on
both an open maze and a maze with an internal wall barrier — both solved
cleanly with no crashes and no deadlocks.

## What it does

1. **Search passes** (`NUM_SEARCH_ROUND_TRIPS`, default 2): drives start→goal→start
   repeatedly, sensing walls at every cell and recomputing the flood field
   live. Each extra round trip tends to reveal shortcuts the previous pass
   didn't take, because the flood values shift as more walls become known.
2. **Speed run**: once search passes are done, recomputes the flood field on
   the fully-known map, extracts the true shortest path, compresses it into
   long straight segments, and drives it with batched `moveForward(n)` calls
   instead of stopping every cell — this both scores better under `mms`'s
   effective-distance rule and mirrors how the real robot should run
   (accelerate through a straight instead of stop-start every 18cm).

## Build

```bash
g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse
```

## Run in `mms`

1. Open the simulator, click **Edit Algorithm**.
2. Set **Directory** to this folder.
3. **Build Command:** `g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse`
4. **Run Command:** `./mouse`
5. Load a maze (try [micromouseonline/mazefiles](https://github.com/micromouseonline/mazefiles)
   for real contest mazes, not just the built-in ones) and hit run.

## Sanity-check without the GUI

```bash
g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse
python3 fake_mms.py
```

`fake_mms.py` is a minimal stand-in for the simulator's stdin/stdout protocol.
It's not a replacement for testing in the real `mms` GUI (no visual, no real
maze files), but it's much faster for catching plumbing bugs — infinite
loops, protocol desyncs, off-by-one wall bugs — before you ever open the GUI.
Edit the wall-generation block at the top to try different obstacle layouts.

## Tuning

- `NUM_SEARCH_ROUND_TRIPS` in `main.cpp` — more passes = better-known map =
  better final path, at the cost of more search time. Given your 8-minute
  Trial Time (elimination round, section 2.4.2) or 12-minute Competition
  Time (final round, section 2.5.2), 2–3 is a reasonable starting point;
  tune upward if your mouse is fast enough that search time is cheap
  relative to run time.
- `VISUALIZE` — turns on/off drawing the flood field as cell text at the
  end. Kept off during search on purpose (see below).

## A bug worth knowing about, and how it was caught

The first version of this code called the flood-field visualization
(drawing every cell's flood value as text) on *every single navigation
step*, not just once at the end. In `mms` that's 256 `setText` calls per
cell moved — harmless to correctness, but it would visibly lag the
simulator and, on real hardware, would be the equivalent of stalling your
control loop every step to update a debug display. Caught this with the
mock server precisely because it showed up as "far more protocol messages
than moves made." Worth keeping in mind if you extend this: anything you
do every navigation step should be `O(1)`-ish, not `O(maze size)`.

## Diagonal mode (`ENABLE_DIAGONAL_SPEEDRUN`)

Currently **off** by default, and deliberately not implemented as a silent
"just works" flag. `mms` genuinely supports diagonal solving —
`turnRight45`/`turnLeft45`/`moveForwardHalf`/`wallFront(N)` with a half-step
lookahead all exist in the API — but the exact half-step counting
convention for cutting a corner (how many half-steps a 45°-in / diagonal /
45°-out sequence should cover to land squarely in the next cell without
clipping the lattice post) isn't something to guess at blindly; it needs to
be confirmed visually in the simulator, the same way you'd tune it on real
hardware. Guessing wrong here and shipping it as "done" would be worse than
not having it — a mouse that clips corners in competition is worse than one
that just does clean 90° turns.

The recommended path:
1. Get the cardinal (90°-turn) version above winning comfortably in `mms`
   first — that's the actual backbone of the algorithm and is what your
   Trial Time score is going to be won or lost on.
2. Once that's solid, we build the diagonal path-smoother (staircase
   pattern → diagonal run detection lives cleanly on top of
   `FloodFill::compressPath`) and verify the half-step geometry against the
   simulator's visual output cell by cell before trusting it.
3. Only then does it move to the real MPU6050/PID arc-turn implementation
   discussed separately — diagonal execution on real hardware is a motion-
   control problem (turn-while-moving velocity profile), not something the
   path planner alone solves.

## Files

| File | Purpose |
|---|---|
| `api.h` / `api.cpp` | stdin/stdout protocol wrapper for the mms Mouse API |
| `floodfill.h` / `floodfill.cpp` | Wall map, BFS flood computation, path extraction, path compression |
| `main.cpp` | Multi-pass search strategy + speed run orchestration |
| `fake_mms.py` | Mock protocol server for fast offline testing |