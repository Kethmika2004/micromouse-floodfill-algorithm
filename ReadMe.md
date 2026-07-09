# 🤖 ROBOFEST 2026 – Micromouse Solver

### Modified Flood Fill Algorithm for Autonomous Maze Navigation

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-mms%20Simulator-success)
![Algorithm](https://img.shields.io/badge/Algorithm-Modified%20Flood%20Fill-orange)
![Status](https://img.shields.io/badge/Status-Competition%20Ready-brightgreen)

An optimized **Modified Flood Fill** implementation developed for **ROBOFEST 2026 Micromouse**. The solver is designed for the **mackorone/mms** simulator and features multi-pass maze exploration, dynamic flood-field recomputation, shortest-path extraction, and an optimized speed run strategy.

---

# 📖 Overview

This project implements a competition-oriented **Flood Fill Algorithm** for autonomous maze navigation. Unlike a traditional implementation that simply computes a shortest path once, this solver continuously updates its internal maze representation as new walls are discovered and recomputes the flood values in real time.

After exploration is complete, the solver performs an optimized speed run using the fully discovered maze, compressing straight paths to minimize unnecessary stops and improve traversal efficiency.

The project was designed specifically for **ROBOFEST 2026** but can also serve as a learning resource for robotics, path planning, and autonomous navigation.

---

# ✨ Features

* ✅ Modified Flood Fill Algorithm
* ✅ Dynamic wall discovery
* ✅ Real-time flood-field recomputation
* ✅ Multi-pass maze exploration
* ✅ Automatic shortest-path extraction
* ✅ Straight-path compression
* ✅ Optimized speed run
* ✅ Compatible with the `mms` simulator
* ✅ Offline testing with a mock protocol server
* ✅ Modular C++17 implementation

---

# 🧠 Algorithm Overview

The solver operates in two major phases.

## 1️⃣ Search Phase

The mouse repeatedly explores the maze.

For every movement:

* Detect surrounding walls
* Update the internal maze representation
* Recompute flood values
* Move toward the neighbor with the minimum flood value

Each search pass improves the quality of the discovered map.

By default, the solver performs **two complete round trips** between the start and the goal.

---

## 2️⃣ Speed Run

After exploration is complete:

* The maze is treated as fully known.
* Flood values are recomputed.
* The shortest path is extracted.
* Consecutive straight cells are compressed.
* The mouse executes long `moveForward(n)` commands instead of stopping at every cell.

This significantly improves traversal efficiency and better reflects how a real Micromouse robot should operate.

---

# 🏗️ System Architecture

```text
            Unknown Maze
                 │
                 ▼
        Wall Detection
                 │
                 ▼
      Internal Maze Update
                 │
                 ▼
      Flood Fill Computation
                 │
                 ▼
      Next Cell Selection
                 │
                 ▼
         Continue Search
                 │
                 ▼
      Fully Explored Maze
                 │
                 ▼
      Shortest Path Extraction
                 │
                 ▼
       Path Compression
                 │
                 ▼
          Optimized Speed Run
```

---

# 📂 Repository Structure

```text
.
├── api.cpp
├── api.h
├── floodfill.cpp
├── floodfill.h
├── main.cpp
├── fake_mms.py
└── README.md
```

| File                            | Description                                                                                 |
| ------------------------------- | ------------------------------------------------------------------------------------------- |
| `api.h` / `api.cpp`             | Wrapper for the mms Mouse API communication protocol                                        |
| `floodfill.h` / `floodfill.cpp` | Flood Fill implementation, wall map, BFS computation, path extraction, and path compression |
| `main.cpp`                      | Multi-pass exploration strategy and speed run orchestration                                 |
| `fake_mms.py`                   | Mock protocol server for offline testing                                                    |

---

# 🚀 Getting Started

## Requirements

* C++17
* GCC / Clang
* Python 3 (for offline testing)
* mackorone/mms Simulator

Simulator:

https://github.com/mackorone/mms

---

# 🔨 Build

```bash
g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse
```

---

# ▶️ Running in the mms Simulator

1. Open the **mms Simulator**.
2. Click **Edit Algorithm**.
3. Set the project directory.
4. Build using:

```bash
g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse
```

5. Run using

```bash
./mouse
```

6. Load any maze and press **Run**.

For more realistic competition mazes:

https://github.com/micromouseonline/mazefiles

---

# 🧪 Offline Testing

Compile the solver:

```bash
g++ -std=c++17 -O2 api.cpp floodfill.cpp main.cpp -o mouse
```

Run the mock protocol server:

```bash
python3 fake_mms.py
```

The mock server simulates the communication protocol used by the `mms` simulator.

Although it does not provide visualization, it is extremely useful for detecting:

* Infinite loops
* Protocol synchronization issues
* Incorrect wall detection
* Off-by-one errors
* Navigation bugs

before testing inside the graphical simulator.

---

# ⚙️ Configuration

Several parameters can be adjusted depending on the competition strategy.

### `NUM_SEARCH_ROUND_TRIPS`

Located in `main.cpp`.

Controls the number of complete exploration passes.

Higher values:

* Better maze knowledge
* Better final path
* Longer search time

Recommended values:

* **2** for fast competitions
* **3** for more complete exploration

---

### `VISUALIZE`

Enables visualization of the flood values.

Disabled during search because rendering every flood value on every movement introduces unnecessary overhead.

Recommended:

```text
OFF during exploration
ON for debugging
```

---

### `ENABLE_DIAGONAL_SPEEDRUN`

Currently disabled.

The simulator supports diagonal movement through:

* `turnLeft45()`
* `turnRight45()`
* `moveForwardHalf()`

However, proper half-step geometry must be verified visually before enabling diagonal execution.

Future work will integrate:

* Diagonal path smoothing
* Arc turns
* Motion profiling
* PID-controlled diagonal movement

---

# 📊 Performance Highlights

* Multi-pass exploration
* Dynamic flood updates
* Efficient BFS implementation
* Optimized shortest-path extraction
* Straight-path compression
* Reduced movement overhead
* Stable protocol communication
* Modular architecture
* Competition-ready implementation

---

# 🐞 Debugging

One important optimization discovered during development involved flood-field visualization.

An early version redrew every flood value after each movement.

Although algorithmically correct, this generated hundreds of unnecessary protocol messages and significantly slowed execution.

The issue was detected using the mock protocol server and resolved by limiting visualization to debugging sessions only.

This serves as an important reminder that expensive operations should never occur inside the robot's main navigation loop.

---

# 🔮 Future Improvements

* A* Search
* Dijkstra Comparison
* Bidirectional Search
* Diagonal Speed Run
* Motion Profiling
* PID-controlled Arc Turns
* Hardware Integration
* Sensor Noise Handling
* Performance Benchmarking
* Maze Visualization
* SLAM-inspired Mapping

---

# 📚 References

* Mackorone Micromouse Simulator

  https://github.com/mackorone/mms

* Micromouse Online Maze Files

  https://github.com/micromouseonline/mazefiles

* IEEE Micromouse Competition Resources

* Flood Fill Algorithm

* Breadth-First Search (BFS)

---

# 👨‍💻 Authors

**Yasandu Kethmika**

Computer Science & Engineering Undergraduate
University of Moratuwa

GitHub: https://github.com/Kethmika2004

---

# 📄 License

This project is released under the **MIT License**.

Feel free to use, modify, and extend this project for educational, research, or competition purposes.

---

⭐ **If you found this project useful, consider giving the repository a star!**
