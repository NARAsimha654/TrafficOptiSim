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
    - *(Vehicle movement and interaction with traffic signals are planned for future development.)*

### 3. Traffic Signal Controller (`intersection.hpp`/`intersection.cpp`)
- **Representation**: Each intersection is managed by an `Intersection` object.
    - **ID**: Matches a node ID in the road network graph.
    - **Approaches**: Manages multiple incoming directions or lanes, identified by unique `approach_id`s (e.g., corresponding to incoming edge IDs).
    - **Traffic Signals**: For each approach, it maintains the current state of the traffic light (`RED`, `GREEN`, `YELLOW`).
    - **Vehicle Queues**: For each approach, it maintains a queue of vehicles (by ID) waiting at a red or yellow light.
- **Behavior**:
    - **Signal Cycling**: The `update_signal_state()` method implements the logic for traffic light phase changes. Currently, it uses a fixed-time, round-robin approach:
        - Each approach gets a GREEN light for a defined `GREEN_DURATION`.
        - This is followed by a `YELLOW_DURATION`.
        - Then the light turns RED, and the next approach in sequence gets a GREEN light.
    - **Queue Management**: The `add_vehicle_to_queue()` method allows vehicles to be added to the waiting queue of a specific approach. *(Dequeuing logic will be part of vehicle movement.)*

### 4. Simulation Core (`simulation.hpp`/`simulation.cpp`)
- **Orchestration**: The `Simulation` class is the central coordinator of the simulation.
    - **State**: It holds the `Graph` representing the road network, and collections of all active `Vehicle` and `Intersection` objects.
    - **Time**: It maintains the current simulation time via a `current_tick_` counter.
- **Behavior**:
    - **Setup**: Allows setting the graph and adding intersections and vehicles.
    - **`tick()` method**: This is the main driver of the simulation. In each call to `tick()`:
        - The `current_tick_` is incremented.
        - It iterates through all `Intersection` objects and calls their `update_signal_state()` method, causing traffic lights to cycle.
        - *(Future enhancements will include updating vehicle positions, handling vehicle-intersection interactions, and managing vehicle generation/despawning within the `tick()` method.)*

### 5. Multithreading
*(Details on multithreading for vehicle generation, signal management, etc., will be provided as these features are implemented.)*

### 6. Visualization Module (Optional)
*(Details on the optional SFML/OpenGL based visualization will be provided if this module is developed.)*


## How to Run

### Dependencies
- C++17 or newer (GCC or Clang recommended)
- GNU Make (for building using the provided Makefile)
- *(Optional: SFML if/when visualization is added)*

### Building the Project
To build the main simulation executable and all test suites, navigate to the root directory of the project (`TrafficOptiSim/`) and run:
```bash
make all
```
Alternatively, you can build only the main simulation executable:
```bash
make traffic_sim
```
Or build and run all tests:
```bash
make run_tests
```

### Running the Simulation
After building, you can run the basic console-based simulation:
```bash
./bin/traffic_sim
```
**Expected Output:**
The program will print:
- Initial setup information (graph details, intersections created, vehicles with their planned paths).
- Output for each simulation tick (default is 60 ticks):
    - The current tick number.
    - For each intersection: its ID, and the signal state (RED, GREEN, YELLOW) and current vehicle queue size for each of its approaches.
You will observe the traffic light states cycling at each intersection over time. Currently, vehicle movement is not simulated, so queue sizes will remain 0 unless manually manipulated in code.

### Running Tests
To execute all compiled test suites:
```bash
make run_tests
```
This will run tests for the `Graph`, `Vehicle`, `Intersection`, and `Simulation` components, reporting PASS/FAIL status.

## Simulation Details
*(More details about the simulation's mechanics, data inputs, and outputs will be provided here as the simulation evolves.)*

## Tech Stack
- **Core Language**: C++ (C++17)
- **Build System**: GNU Make
- **Testing**: Basic `assert` statements in C++ test files.
- **Libraries**:
    - Standard Template Library (STL): For data structures (vectors, maps, queues), algorithms, etc.
- *(Optional: SFML for visualization)*

## Demo GIF / Screenshots
*(To be added once visualization or significant simulation output is available.)*

## Future Enhancements
*(A list of potential future improvements and stretch goals, including those mentioned by the user like logging, GUI map creation, REST APIs, and dynamic weather modifiers.)*
