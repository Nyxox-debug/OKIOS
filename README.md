# OKIOS
Greek οἶκος → home / habitat

## Description
This project is a reinforcement learning–driven artificial life simulation where a population of identical agents ("creatures") learns to survive and thrive in a shared environment.

Rather than assigning predefined roles, each agent begins with the same capabilities and no prior knowledge. Through interaction with the environment and other agents, distinct behavioral patterns naturally emerge. Over time, agents specialize into functional roles such as exploration, resource gathering, defense, and coordination—without any explicit programming of these roles.

The system is designed to explore how complex group dynamics and structured behavior can arise from simple rules, local perception, and reward-driven learning. Cooperation, specialization, and collective intelligence are not enforced, but instead emerge as adaptive strategies under environmental pressure.

At its core, the simulation investigates a fundamental question:
> How does organized structure emerge from decentralized, self-interested agents?

## Requirements

- CMake 3.16+
- A C++ compiler with C++17 support (GCC or Clang)
- OpenGL 3.3+
- Git (for fetching GLFW automatically)

> **Platform:** Linux and macOS only — the provided CMake preset uses `Unix Makefiles`.
> Windows users would need to modify `CMakePresets.json` to use a different generator
> (e.g. `"Ninja"` or `"Visual Studio 17 2022"`).

## How to Run

**1. Clone the repository**
```bash
git clone https://github.com/Nyxox-debug/OKIOS 
cd OKIOS
```

**2. Configure and build using the preset**
```bash
cmake --preset engine
cmake --build build
```

**3. Run the simulation**
```bash
./build/engine
```

## Controls

| Key | Action |
|-----|--------|
| `W A S D` | Move camera |
| Mouse | Look around |
| `ESC` | Exit |

## Project Structure

```
OKIOS/
├── src/          # Source files
├── include/      # Header files
├── res/
│   └── shaders/  # GLSL shader files
└── vendors/      # glad, glm
```

> WIP
