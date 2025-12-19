# RV – Rich Yet?
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Raylib](https://img.shields.io/badge/Raylib-5.0-%23FFFFFF?style=for-the-badge&logo=c&logoColor=black)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)

**A C++ trivia game engine built with Raylib, bridging advanced Data Structures & Algorithms with a polished GUI.**

<img width="1920" height="1080" alt="splash" src="https://github.com/user-attachments/assets/342cb799-c7c2-4303-8193-0505cce4bd8a" />

## 💡 Motivation

This project was conceived to solve a specific engineering challenge: taking abstract Computer Science concepts and implementing them in a tangible, interactive software product.

Too often, Data Structures & Algorithms (DSA) are taught in a vacuum, resulting in command-line tools that demonstrate logic but lack usability. Our goal was to break that mold. We wanted to build a system where **Linked Lists** handled actual gameplay progression, **Hash Maps** managed content networks, and **Sorting Algorithms** powered real-time leaderboards.

**RV – Rich Yet?** is the result—a transition from theoretical code to a deployed graphical application.

## Overview

**RV – Rich Yet?** is a graphical trivia application modeled after the *Who Wants to Be a Millionaire* format. It moves beyond basic arrays and loops, utilizing a custom-built engine to handle state management, file serialization, and dynamic rendering. The game runs on a virtual resolution system, ensuring consistent scaling across different display sizes.

## 👥 The Team

This project was built by a team of three engineers. Click below to connect:

| Engineer | Role | Connect |
| :--- | :--- | :--- |
| **Simal** | Data Structures & Leaderboard | [![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](www.linkedin.com/in/simalhassan) |
| **Ahmed** | Renderer, Audio, & Assets | [![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/ian/ahmedrazakazim) |
| **Laiba** | Game Engine & Logic | [![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/your-laiba-link) |

---

## Key Features

### Gameplay
* **Dynamic GUI:** Custom-rendered UI elements including a prize ladder, lifeline overlays, and a leaderboard.
* **Asset System:** Robust loading pipeline for textures (`.png`/`.jpg`), fonts (`.ttf`), and audio streams.
* **Lifelines:** Four distinct mechanics (50:50, Phone a Friend, Skip, Hint) managed via bitmasking.
* **Persistence:** Automatic saving/loading of player profiles and high scores.
* **Audio Engine:** Context-aware audio system that transitions between looping tracks and one-shot SFX based on game state.

### Technical Implementation
* **State Machine:** Clean separation of concerns using a `GameController` to manage transitions (Splash -> Menu -> Gameplay -> Result).
* **Virtual Resolution:** A rendering target texture allows the game to render at 1920x1080 internally while fitting any window size.
* **Safe Resource Management:** RAII-style handling of Raylib assets (Unloading textures/sounds in destructors).

---

## Architecture & DSA Integration

The core requirement of this project was to integrate **10 distinct Data Structures & Algorithms**, selected not just for complexity, but for their specific utility in solving game logic problems.

| Component | Data Structure / Algorithm | Technical Justification |
| :--- | :--- | :--- |
| **Prize Ladder** | **Singly Linked List** | Enforces the sequential "forward-only" progression of the game. Allows $O(1)$ insertion for potential safety net adjustments. |
| **Leaderboard** | **Merge Sort** | Chosen for stability and guaranteed $O(N \log N)$ performance, avoiding Quick Sort's worst-case degradation on sorted lists. |
| **Category System** | **Hash Table** | `unordered_map` provides $O(1)$ access to specific question difficulty buckets, eliminating linear database scans. |
| **Duplicate Checks** | **Hash Set** | `unordered_set` allows $O(1)$ verification to ensure players don't see the same question twice in a session. |
| **Lifelines** | **Bit Manipulation** | Compressed the boolean state of 4 lifelines into a single integer bitmask for memory efficiency. |
| **Navigation** | **Stack** | LIFO structure tracks menu history to support intuitive "Back" navigation. |
| **Rank Lookup** | **Binary Search** | $O(\log N)$ lookup to determine where a player's score ranks among thousands of entries. |
| **Name Search** | **KMP Algorithm** | Knuth-Morris-Pratt pattern matching used for efficient substring searches within the leaderboard. |
| **RNG** | **Fisher-Yates Shuffle** | Implemented manually to ensure mathematically unbiased randomization of question options. |
| **Event Log** | **Circular Queue** | Fixed-size buffer to manage recent game events without dynamic allocation overhead. |

---

## Technical Challenges & Solutions

* **Asset Pipeline Fragility:** Raylib is strict about file formats. We implemented a robust loading fallback system in `RaylibRenderer` that attempts multiple file extensions (e.g., trying `.jpg` if `.png` fails) to prevent runtime crashes on missing assets.
* **Audio Overlap:** A common issue in FSMs is audio triggering every frame. We implemented a `handleStateAudio` listener that only triggers sound events on state *transitions* (Edge Detection), preventing audio buffer overflow.
* **Text Centering:** Standard centering math failed when custom fonts loaded asynchronously. We wrote a `measureTextSafe` wrapper that validates texture IDs before calculating offsets, preventing text from snapping to the wrong coordinates.

---

## Build Instructions

### Prerequisites
* C++17 Compiler (GCC/MSVC)
* CMake 3.20+
* Raylib 5.0 (Linked via CMake)

### How to Run
1.  **Clone:**
    ```bash
    git clone [https://github.com/yourusername/rv-rich-yet.git](https://github.com/yourusername/rv-rich-yet.git)
    cd rv-rich-yet
    ```
2.  **Build:**
    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build .
    ```
3.  **Run:**
    Ensure the `Graphics/`, `sounds/`, and `fonts/` directories are adjacent to the executable.
    ```bash
    ./wwtbam
    ```

---

**License:** MIT
