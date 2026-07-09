#include "floodfill.h"
#include <queue>
#include <algorithm>

namespace {
constexpr int DX[4] = {0, 1, 0, -1};  // N, E, S, W
constexpr int DY[4] = {1, 0, -1, 0};
Direction OPPOSITE(Direction d) { return static_cast<Direction>((d + 2) % 4); }
}

FloodFill::FloodFill() {
    for (auto& row : walls_) row.fill(0);
    for (auto& row : flood_) row.fill(UNREACHABLE);

    // The four outer boundary walls are guaranteed by the maze spec
    // (section 2.3.1: "the outside wall encloses the entire maze"),
    // so we can seed these as known from the start.
    for (int i = 0; i < MAZE_SIZE; ++i) {
        addWall(i, 0, SOUTH);
        addWall(i, MAZE_SIZE - 1, NORTH);
        addWall(0, i, WEST);
        addWall(MAZE_SIZE - 1, i, EAST);
    }
    // Section 2.3.6: start cell is bound by walls on three sides.
    // We assume start = (0,0) facing NORTH, with walls on S/W already set
    // above; the caller (main) sets the third wall based on which side
    // the actual maze places it on once sensed - left as a runtime addWall.
}

bool FloodFill::inBounds(int x, int y) const {
    return x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE;
}

void FloodFill::addWall(int x, int y, Direction dir) {
    if (!inBounds(x, y)) return;
    walls_[y][x] |= (1 << dir);
    int nx = x + DX[dir];
    int ny = y + DY[dir];
    if (inBounds(nx, ny)) {
        walls_[ny][nx] |= (1 << OPPOSITE(dir));
    }
}

bool FloodFill::hasWall(int x, int y, Direction dir) const {
    if (!inBounds(x, y)) return true; // treat out-of-bounds as walled
    return (walls_[y][x] & (1 << dir)) != 0;
}

bool FloodFill::isKnown(int x, int y, Direction dir) const {
    // In this implementation "known wall present" and "known open" are
    // both represented the same way as mms itself does for scoring: we
    // only ever set a bit once we've actually sensed it. Passability
    // during flood fill is therefore "not hasWall", which is exactly the
    // optimistic-until-proven-blocked rule described in the mms README
    // ("all walls are assumed non-existent" until discovered).
    return hasWall(x, y, dir);
}

void FloodFill::computeFlood(const std::vector<Cell>& targets) {
    for (auto& row : flood_) row.fill(UNREACHABLE);

    std::queue<Cell> q;
    for (const auto& t : targets) {
        flood_[t.y][t.x] = 0;
        q.push(t);
    }

    while (!q.empty()) {
        Cell c = q.front();
        q.pop();
        uint16_t d = flood_[c.y][c.x];

        for (int dir = 0; dir < 4; ++dir) {
            if (hasWall(c.x, c.y, static_cast<Direction>(dir))) continue;
            int nx = c.x + DX[dir];
            int ny = c.y + DY[dir];
            if (!inBounds(nx, ny)) continue;
            if (flood_[ny][nx] > d + 1) {
                flood_[ny][nx] = d + 1;
                q.push({nx, ny});
            }
        }
    }
}

bool FloodFill::bestNeighbor(int x, int y, Cell& next, Direction& moveDir) const {
    uint16_t best = UNREACHABLE;
    bool found = false;
    for (int dir = 0; dir < 4; ++dir) {
        if (hasWall(x, y, static_cast<Direction>(dir))) continue;
        int nx = x + DX[dir];
        int ny = y + DY[dir];
        if (!inBounds(nx, ny)) continue;
        if (flood_[ny][nx] < best) {
            best = flood_[ny][nx];
            next = {nx, ny};
            moveDir = static_cast<Direction>(dir);
            found = true;
        }
    }
    return found;
}

Direction FloodFill::directionBetween(Cell a, Cell b) {
    if (b.x == a.x + 1) return EAST;
    if (b.x == a.x - 1) return WEST;
    if (b.y == a.y + 1) return NORTH;
    return SOUTH;
}

std::vector<Cell> FloodFill::extractPath(Cell start, const std::vector<Cell>& targets) const {
    std::vector<Cell> path;
    Cell cur = start;
    path.push_back(cur);

    auto isTarget = [&](Cell c) {
        for (const auto& t : targets) if (t == c) return true;
        return false;
    };

    int guard = MAZE_SIZE * MAZE_SIZE * 2; // safety against infinite loop on bad map
    while (!isTarget(cur) && guard-- > 0) {
        uint16_t best = UNREACHABLE;
        Cell nextCell = cur;
        bool found = false;
        for (int dir = 0; dir < 4; ++dir) {
            if (hasWall(cur.x, cur.y, static_cast<Direction>(dir))) continue;
            int nx = cur.x + DX[dir];
            int ny = cur.y + DY[dir];
            if (!inBounds(nx, ny)) continue;
            if (flood_[ny][nx] < best) {
                best = flood_[ny][nx];
                nextCell = {nx, ny};
                found = true;
            }
        }
        if (!found) break;
        cur = nextCell;
        path.push_back(cur);
    }
    return path;
}

std::vector<Segment> FloodFill::compressPath(const std::vector<Cell>& path) {
    std::vector<Segment> segments;
    if (path.size() < 2) return segments;

    Direction curDir = directionBetween(path[0], path[1]);
    int count = 1;

    for (size_t i = 1; i + 1 < path.size(); ++i) {
        Direction d = directionBetween(path[i], path[i + 1]);
        if (d == curDir) {
            ++count;
        } else {
            segments.push_back({curDir, count});
            curDir = d;
            count = 1;
        }
    }
    segments.push_back({curDir, count});
    return segments;
}

std::vector<Cell> centerGoalCells() {
    int c1 = MAZE_SIZE / 2 - 1; // 7
    int c2 = MAZE_SIZE / 2;     // 8
    return {{c1, c1}, {c1, c2}, {c2, c1}, {c2, c2}};
}