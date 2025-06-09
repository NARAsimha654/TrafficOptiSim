# TrafficOptiSim

## Overview
TrafficOptiSim is a C++-based smart traffic signal simulation and optimization system designed for urban environments. It mimics real-world intersection behavior and can use simulated or historical traffic data to optimize signal timings for improved traffic flow. The system employs pathfinding algorithms (like Dijkstra's) to allow vehicles to find optimal routes.

The primary objectives of TrafficOptiSim are to:
- Simulate a smart intersection network with vehicle movement.
- Optimize traffic signal timings based on traffic density (using the `TrafficOptimizer` component).
- Utilize utility functions for common tasks like data parsing (`Utils` component).
- Provide a text-based visualization of the simulation state (`TextVisualizer` component).
- Demonstrate advanced C++ concepts like STL, file I/O, and algorithmic problem-solving.

## System Architecture
The system is composed of several key components that interact to simulate and optimize traffic flow:

### 1. Road Network Model (`graph.hpp`/`graph.cpp`)
- **Representation**: The urban environment's roads and intersections are modeled as a directed graph.
    - **Nodes**: Represent intersections.
    - **Edges**: Represent roads connecting two intersections, with a `weight` (e.g., travel time).
- **Pathfinding**: Implements Dijkstra's algorithm (`find_shortest_path`) for vehicles to find optimal routes.

### 2. Vehicle Simulation (`vehicle.hpp`/`vehicle.cpp`)
- **Representation**: Each vehicle has an ID, source, destination, and planned path.
- **Behavior**:
    - **Route Planning**: Uses the `Graph` to plan its path.
    - **State Machine**: Moves through states: `NOT_STARTED`, `EN_ROUTE`, `WAITING_AT_INTERSECTION`, `ARRIVED`.
    - **Movement**: Progresses along edges based on edge weights (travel time in ticks).

### 3. Traffic Signal Controller (`intersection.hpp`/`intersection.cpp`)
- **Representation**: Manages individual intersections with traffic lights and vehicle queues for each approach (outgoing edge).
- **Behavior**:
    - **Signal Cycling**: Uses fixed-time cycles for `GREEN`, `YELLOW`, `RED` states for each controlled approach.
    - **Queue Management**: Vehicles queue up at red/yellow lights and are dequeued when the light turns green.

### 4. Simulation Core (`simulation.hpp`/`simulation.cpp`)
- **Orchestration**: The `Simulation` class coordinates the graph, vehicles, and intersections.
- **Time**: Manages simulation time via a `current_tick_` counter.
- **`tick()` method**: Advances the simulation by one time step:
    - Updates all intersection signals.
    - Spawns new vehicles periodically.
    - Updates all vehicle states and positions:
        - Moves vehicles along edges.
        - Handles vehicle arrival at intersections (queuing or proceeding).
        - Handles vehicle arrival at destinations (despawning).

### 5. Traffic Optimizer (`optimizer.hpp`/`optimizer.cpp`)
- **Purpose**: Designed to analyze traffic conditions and suggest optimizations, such as adjusting signal timings.
- **`TrafficOptimizer` Class**:
    - **Data Loading**: Can load traffic data (e.g., from `traffic_density.csv`) using `load_traffic_data()`. Data points are stored as `TrafficDataPoint` structs (timestamp, edge_id, density, average_speed, vehicles_passed).
    - **Analysis**: The `analyze_current_conditions()` method can be used to process current simulation state (placeholder implementation).
    - **Suggestions**: `suggest_new_signal_timings()` can propose new signal timings for an intersection (placeholder implementation).
- **`traffic_density.csv`**: Located in the `data/` directory, this CSV file provides sample historical or simulated traffic data. The format is: `timestamp,edge_id,density,average_speed,vehicles_passed`. This data can be used by the `TrafficOptimizer`.

### 6. Utilities (`utils.hpp`/`utils.cpp`)
- **Purpose**: Provides common helper functions used across the project.
- **`Utils` Namespace**:
    - `parse_csv()`: Parses data from a CSV file into `std::vector<CsvRow>`.
    - `trim_whitespace()`: Trims whitespace from strings.
    - `string_to_int()`, `string_to_double()`: Safe string-to-number conversion functions.

### 7. Visualization (`visualization/visualizer.hpp`/`visualization/visualizer.cpp`)
- **Purpose**: Provides a way to view the simulation state.
- **`TextVisualizer` Class**:
    - **Console Output**: Renders a text-based representation of the simulation in the console.
    - **`display_state()`**: Shows the current tick, intersection states (signals and queue sizes for outgoing edges), and active vehicle details (ID, state, location, progress).

## How to Run

### Dependencies
- C++17 or newer (GCC or Clang recommended)
- GNU Make (for building using the provided Makefile)

### Building the Project
To build the main simulation executable (`traffic_sim`) and the comprehensive test suite (`test_traffic_flow`), navigate to the root directory of the project and run:
```bash
make all
```
This command compiles all source files, including the new optimizer, utils, and visualizer components, and links them into the respective executables.

To build only the main simulation executable:
```bash
make traffic_sim
```

To build only the new test suite:
```bash
make $(BIN_DIR)/test_traffic_flow
```
(Note: The Makefile also supports building older, individual test files if needed, but `test_traffic_flow` is the most current.)

### Running the Simulation
After building, you can run the main simulation:
```bash
./bin/traffic_sim
```
**Expected Output:**
The program will:
1.  Print initialization messages (graph loading, intersection creation, optimizer data loading).
2.  Start the simulation loop. For each tick, the `TextVisualizer` will print:
    - The current simulation tick.
    - Status of each intersection:
        - ID of the intersection.
        - For each outgoing edge (approach): its ID, current signal state (RED, GREEN, YELLOW), and the number of vehicles in its queue.
    - Status of active vehicles:
        - A table listing vehicle ID, current state (e.g., EN_ROUTE, WAITING_AT_INTERSECTION), current node, next node, progress on the current edge, and destination node.
3.  The simulation runs for a predefined number of ticks (e.g., 100 in the current `main.cpp`), with a short delay between ticks to make the visualization readable.
4.  Vehicle spawning, movement, queuing at intersections, and arrival at destinations can be observed.

You can disable the visualizer output and the delay by modifying the `enable_visualization` flag in `src/main.cpp` and recompiling if you want the simulation to run faster for data collection or non-visual testing.

### Running Tests
The primary test suite is `test_traffic_flow`. To compile and run all tests (including older ones if still configured in Makefile, and the new comprehensive one):
```bash
make run_tests
```
This command will execute `bin/test_traffic_flow` (and any other test executables defined in the `run_tests` target in the Makefile).
The `test_traffic_flow` executable includes tests for:
- Graph creation and pathfinding.
- Vehicle creation, route planning, and state progression.
- Intersection signal logic and queue management.
- Core simulation mechanics (vehicle movement through intersections).
- Utility functions like CSV parsing.

Test results (PASS/FAIL) will be printed to the console.

## Simulation Details
- **Traffic Data**: The `data/traffic_density.csv` file provides a simple example of how traffic data can be fed into the system. The `TrafficOptimizer` can use this data.
- **Visualization**: The `TextVisualizer` provides a basic way to monitor the simulation. For more advanced graphics, a library like SFML or OpenGL could be integrated in the future.

## Tech Stack
- **Core Language**: C++ (C++17)
- **Build System**: GNU Make
- **Testing**: Custom test runner with `assert` statements in C++ (`tests/test_traffic_flow.cpp`).
- **Libraries**:
    - Standard Template Library (STL): For data structures, algorithms, etc.

## Demo GIF / Screenshots
*(To be added: A screenshot of the text visualizer output would be appropriate here.)*

## Future Enhancements
- Advanced traffic optimization algorithms in `TrafficOptimizer`.
- Graphical visualization (e.g., using SFML).
- Logging simulation events to a file.
- GUI for map creation or simulation control.
- REST APIs for external interaction.
- Dynamic event modifiers (e.g., weather, road closures).
