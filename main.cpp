// ROBOFEST 2026 Micromouse solver — Modified Flood Fill
//
// Strategy:
//   1. Search pass(es): flood-fill navigate from start -> goal -> start,
//      sensing and recording walls at every cell visited. Each additional
//      pass tends to discover shortcuts the previous pass's path didn't
//      reveal, since the flood values change as more walls are learned.
//   2. Speed run: recompute the flood field on the fully-known map, extract
//      the true shortest path, compress it into long straight segments
//      (batched moveForward calls instead of cell-by-cell stepping), and
//      execute it as fast as possible.
//
// Build:  g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse
// Run (inside mms "Edit Algorithm"): ./mouse

#include "api.h"
#include "floodfill.h"
#include <iostream>

namespace {

// ---- Tunables ----
constexpr int NUM_SEARCH_ROUND_TRIPS = 2; // start->goal->start, repeated this many times before the speed run
constexpr bool VISUALIZE = true;          // draw flood values / path in the simulator UI
constexpr bool ENABLE_DIAGONAL_SPEEDRUN = false; // see README before enabling

// ---- Mouse pose, tracked locally (mms does not report position back to us) ----
struct Pose {
    int x = 0, y = 0;
    Direction dir = NORTH;
};

Direction turnLeftOf(Direction d)  { return static_cast<Direction>((d + 3) % 4); }
Direction turnRightOf(Direction d) { return static_cast<Direction>((d + 1) % 4); }

// Sense the three visible sides at the current cell and record any walls found.
void senseWalls(FloodFill& maze, const Pose& pose) {
    struct Check { bool present; Direction absDir; };
    Check checks[3] = {
        { API::wallFront(), pose.dir },
        { API::wallLeft(),  turnLeftOf(pose.dir) },
        { API::wallRight(), turnRightOf(pose.dir) },
    };
    for (const auto& c : checks) {
        if (c.present) {
            maze.addWall(pose.x, pose.y, c.absDir);
            API::setWall(pose.x, pose.y, "nesw"[c.absDir]); // visual feedback in the sim
        }
    }
}

// Rotate in place to face `target`, choosing the shorter way round.
void turnToFace(Pose& pose, Direction target) {
    int diff = (target - pose.dir + 4) % 4;
    switch (diff) {
        case 0: break;
        case 1: API::turnRight(); break;
        case 2: API::turnRight(); API::turnRight(); break;
        case 3: API::turnLeft(); break;
    }
    pose.dir = target;
}

void step(Pose& pose, Direction moveDir) {
    static constexpr int DX[4] = {0, 1, 0, -1};
    static constexpr int DY[4] = {1, 0, -1, 0};
    pose.x += DX[moveDir];
    pose.y += DY[moveDir];
}

void showFlood(const FloodFill& maze) {
    if (!VISUALIZE) return;
    for (int y = 0; y < MAZE_SIZE; ++y) {
        for (int x = 0; x < MAZE_SIZE; ++x) {
            uint16_t v = maze.floodValue(x, y);
            if (v != UNREACHABLE) {
                API::setText(x, y, std::to_string(v));
            }
        }
    }
}

// Cell-by-cell flood-fill navigation from wherever `pose` currently is to
// any cell in `targets`. Senses walls and recomputes flood every step, so
// it reacts correctly even if earlier assumptions turn out to be wrong.
void navigateTo(FloodFill& maze, Pose& pose, const std::vector<Cell>& targets) {
    auto isTarget = [&](Cell c) {
        for (const auto& t : targets) if (t == c) return true;
        return false;
    };

    while (!isTarget({pose.x, pose.y})) {
        senseWalls(maze, pose);
        maze.computeFlood(targets);

        Cell next{};
        Direction moveDir;
        if (!maze.bestNeighbor(pose.x, pose.y, next, moveDir)) {
            // Shouldn't happen on a valid, fully-enclosed competition maze.
            // If it does, something upstream is wrong (bad wall data) -
            // stop rather than loop forever.
            std::cerr << "No open neighbor found - aborting navigation." << std::endl;
            return;
        }

        turnToFace(pose, moveDir);
        if (!API::moveForward(1)) {
            // Crash - re-sense, the wall map was wrong somewhere; the next
            // loop iteration will pick this up since we didn't move.
            API::setColor(pose.x, pose.y, 'r');
            continue;
        }
        step(pose, moveDir);
    }
    senseWalls(maze, pose); // sense the goal cell too, useful for later passes
}

// Execute the compressed path as a fast run: batch moveForward calls
// instead of stepping cell-by-cell. This is what actually gets scored as
// the competition run, and it's what should map onto a real trapezoidal
// speed profile on the physical mouse (accelerate through the whole
// segment instead of stopping every 18cm).
void executeSpeedRun(Pose& pose, const std::vector<Segment>& segments) {
    for (const auto& seg : segments) {
        turnToFace(pose, seg.dir);
        if (!API::moveForward(seg.cells)) {
            std::cerr << "Unexpected crash during speed run at ("
                      << pose.x << "," << pose.y << ")" << std::endl;
            return;
        }
        for (int i = 0; i < seg.cells; ++i) step(pose, seg.dir);
    }
}

} // namespace

int main() {
    FloodFill maze;
    Pose pose; // starts at (0,0) facing NORTH, per section 2.3.6

    const std::vector<Cell> goal = centerGoalCells();
    const std::vector<Cell> start = {{0, 0}};

    API::setColor(0, 0, 'g');
    for (const auto& g : goal) API::setColor(g.x, g.y, 'b');

    // ---- Search passes ----
    for (int pass = 0; pass < NUM_SEARCH_ROUND_TRIPS; ++pass) {
        navigateTo(maze, pose, goal);
        navigateTo(maze, pose, start);
    }
    // Guarantee we're standing on goal-adjacent knowledge for the final
    // solve even if NUM_SEARCH_ROUND_TRIPS ends with pose already at start.
    navigateTo(maze, pose, goal);
    navigateTo(maze, pose, start);

    // ---- Speed run ----
    maze.computeFlood(goal);
    showFlood(maze); // visualize final knowledge once, not every step
    std::vector<Cell> path = maze.extractPath({pose.x, pose.y}, goal);
    std::vector<Segment> segments = FloodFill::compressPath(path);

    if (ENABLE_DIAGONAL_SPEEDRUN) {
        // See README.md "Diagonal mode" section before enabling this -
        // half-step geometry needs to be visually confirmed in the
        // simulator on your maze before you trust it in competition.
        std::cerr << "Diagonal speed run not enabled in this build; "
                     "running cardinal speed run instead." << std::endl;
    }

    executeSpeedRun(pose, segments);

    return 0;
}