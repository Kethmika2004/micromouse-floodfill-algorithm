#pragma once
#include <vector>
#include <array>
#include <cstdint>

// ---- Directions ----
// Absolute compass directions, matches mms convention: (0,0) is bottom-left,
// x increases East, y increases North.
enum Direction : uint8_t { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

// Wall bitmasks (per cell)
constexpr uint8_t WALL_N = 1 << NORTH;
constexpr uint8_t WALL_E = 1 << EAST;
constexpr uint8_t WALL_S = 1 << SOUTH;
constexpr uint8_t WALL_W = 1 << WEST;

constexpr int MAZE_SIZE = 16;
constexpr uint16_t UNREACHABLE = 0xFFFF;

struct Cell {
    int x, y;
    bool operator==(const Cell& o) const { return x == o.x && y == o.y; }
};

// A single planned move, expressed as an absolute direction to travel
// and how many full cells to cover in a straight line before the next turn.
struct Segment {
    Direction dir;
    int cells; // number of full cells to move forward in this straight segment
};

class FloodFill {
public:
    FloodFill();

    // ---- Wall knowledge ----
    // Mark a wall as existing between (x,y) and its neighbor in `dir`.
    // Automatically mirrors the wall onto the neighboring cell.
    void addWall(int x, int y, Direction dir);
    bool hasWall(int x, int y, Direction dir) const;
    bool isKnown(int x, int y, Direction dir) const;
    bool inBounds(int x, int y) const;

    // ---- Flood fill computation ----
    // Recomputes flood[][] via multi-source BFS from `targets` outward,
    // respecting only walls that are currently known. Unknown = passable
    // (optimistic search), which is the standard Micromouse convention.
    void computeFlood(const std::vector<Cell>& targets);

    uint16_t floodValue(int x, int y) const { return flood_[y][x]; }

    // Returns true and fills `next` with the best open neighbor (lowest
    // flood value) reachable from (x,y). Returns false if no open neighbor
    // exists (shouldn't happen on a valid maze once fully explored).
    bool bestNeighbor(int x, int y, Cell& next, Direction& moveDir) const;

    // Extract full cell-by-cell path from `start` to any cell in `targets`,
    // strictly by descending the flood gradient over currently known walls.
    // Used to compute the final "speed run" path once the maze is solved.
    std::vector<Cell> extractPath(Cell start, const std::vector<Cell>& targets) const;

    // Compress a cell-by-cell path into straight-line segments + turns,
    // which is what should actually be executed (fewer, longer
    // moveForward calls score much better under mms's effective-distance
    // rule, and matches how a real mouse should run - accelerate through
    // straights rather than stopping every 18cm).
    static std::vector<Segment> compressPath(const std::vector<Cell>& path);

    static Direction directionBetween(Cell a, Cell b);

private:
    // walls_[y][x] bitmask of which sides currently have a *known* wall.
    std::array<std::array<uint8_t, MAZE_SIZE>, MAZE_SIZE> walls_{};
    std::array<std::array<uint16_t, MAZE_SIZE>, MAZE_SIZE> flood_{};
};

// Standard 4-cell center goal for a 16x16 maze (0-indexed).
std::vector<Cell> centerGoalCells();