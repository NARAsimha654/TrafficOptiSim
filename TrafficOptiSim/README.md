# TrafficOptiSim

## Overview
TrafficOptiSim is a C++-based smart traffic signal simulation and optimization system designed for urban environments. It mimics real-world intersection behavior using simulated sensor data and applies queue theory and pathfinding algorithms (like Dijkstra's or A*) to optimize signal timings for improved traffic flow. The system is multithreaded, prioritizing performance and scalability.

The primary objectives of TrafficOptiSim are to:
- Simulate a smart intersection network.
- Optimize traffic signal timings based on real-time (simulated) traffic density.
- Use Dijkstra’s or A* to suggest best routes for vehicles.
- Visualize traffic movement and signals (optional, using SFML/OpenGL).
- Demonstrate advanced C++ concepts like STL, multithreading, file I/O, and algorithmic problem-solving.

## System Architecture
The system is composed of several key components that interact to simulate and optimize traffic flow:

### 1. Road Network Model (`graph.hpp`/`graph.cpp`)
- **Representation**: The urban environment's roads and intersections are modeled as a directed graph.
    - **Nodes**: Represent intersections. Each node has a unique ID.
    - **Edges**: Represent roads connecting two intersections. Each edge has properties like:
        - A unique ID.
        - Source and destination node IDs.
        - A `weight`, which typically signifies the travel time or traffic delay on that road segment.
- **Pathfinding**: The graph component is responsible for calculating the fastest (least weight) path between two intersections.
    - **Algorithm**: Currently, Dijkstra's algorithm is implemented in the `find_shortest_path` method to compute these paths. This allows vehicles to determine their optimal route based on current (or static) road weights.

### 2. Vehicle Simulation (`vehicle.hpp`/`vehicle.cpp`)
- **Representation**: Each vehicle in the simulation is an object with the following characteristics:
    - A unique ID.
    - A source intersection (node ID) where it originates.
    - A destination intersection (node ID) it aims to reach.
- **Behavior**:
    - **Route Planning**: Upon creation or when needed, a vehicle plans its route by querying the `Graph` component (using `find_shortest_path`) to get the sequence of intersections it needs to traverse. This path is stored internally by the vehicle.
    - *(Further behaviors like movement, reaction to traffic signals, and interaction with queues will be added in subsequent development stages.)*

### 3. Traffic Signal Controller (`intersection.hpp`/`intersection.cpp`)
*(To be detailed in future development. This component will manage traffic light states and timings at intersections.)*

### 4. Simulation Core (`simulation.cpp`)
*(To be detailed in future development. This component will manage the overall simulation loop, time progression, and state updates.)*

### 5. Multithreading
*(Details on multithreading for vehicle generation, signal management, etc., will be provided as these features are implemented.)*

### 6. Visualization Module (Optional)
*(Details on the optional SFML/OpenGL based visualization will be provided if this module is developed.)*


## How to Run
*(Instructions on dependencies, building, and running the simulation will be added here.)*

### Dependencies
- C++17 or newer
- Make (for building using the provided Makefile)
- *(SFML if visualization is included)*

### Building
```bash
# Clone the repository (assuming it's hosted on Git)
# git clone <repository_url>
# cd TrafficOptiSim

# Build the project (e.g., test executables)
make
```

### Running Tests
```bash
make run_tests
```

## Simulation Details
*(More details about the simulation's mechanics, data inputs, and outputs will be provided here.)*

## Tech Stack
- **Core Language**: C++ (C++17)
- **Build System**: GNU Make
- **Libraries**:
    - Standard Template Library (STL): For data structures (vectors, maps, queues), algorithms, etc.
- *(Optional: SFML for visualization)*

## Demo GIF / Screenshots
*(To be added once visualization or significant simulation output is available.)*

## Future Enhancements
*(A list of potential future improvements and stretch goals.)*
