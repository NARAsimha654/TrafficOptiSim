#include <iostream>
#include <vector>
#include <string>
#include <chrono> // For std::chrono for sleep
#include <thread> // For std::this_thread::sleep_for

#include "simulation.hpp"
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"
#include "optimizer.hpp"       // New: Include optimizer
#include "utils.hpp"           // New: Include utils
#include "visualizer.hpp"      // New: Include visualizer

int main() {
    std::cout << "TrafficOptiSim - Enhanced Simulation Run with Visualizer & Optimizer" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    // 1. Create Graph
    Graph city_graph;
    if (!city_graph.load_from_file("data/sample_map.txt")) {
        std::cerr << "Failed to load graph from file. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Graph loaded successfully from data/sample_map.txt" << std::endl;

    // 2. Create Intersections (as defined in sample_map.txt and current main)
    // Node 1: Intersection with outgoing edges 12, 13
    // Node 2: Intersection with outgoing edges 21, 23, 24
    // Node 3: Intersection with outgoing edges 31, 32, 34
    // Node 4: Intersection with outgoing edges 42, 43
    // (Assuming edges are named <from_node><to_node> for simplicity in example)
    std::map<int, Intersection> intersections_map;
    const auto& all_nodes = city_graph.get_all_nodes();
    for(const auto& node_pair : all_nodes) {
        int node_id = node_pair.first;
        std::vector<int> outgoing_edge_ids;
        for(const auto& edge_pair : city_graph.get_edges_from_node(node_id)) {
            outgoing_edge_ids.push_back(edge_pair.id);
        }
        if (!outgoing_edge_ids.empty()) {
            Intersection intersection;
            intersection.initialize(node_id, outgoing_edge_ids);
            intersections_map[node_id] = intersection;
        }
    }
    std::cout << intersections_map.size() << " intersections created based on graph nodes with outgoing edges." << std::endl;


    // 3. Create Simulation
    Simulation sim;
    sim.set_graph(city_graph);
    for(const auto& int_pair : intersections_map) {
        sim.add_intersection(int_pair.second);
    }
    std::cout << "Simulation initialized with graph and intersections." << std::endl;

    // 4. Initialize Optimizer (Optional Usage Example)
    TrafficOptimizer optimizer;
    std::vector<Utils::CsvRow> raw_traffic_data = Utils::parse_csv("data/traffic_density.csv");
    std::vector<TrafficDataPoint> traffic_data_points;
    // Convert CsvRow to TrafficDataPoint (skipping header)
    if (!raw_traffic_data.empty()) {
        for (size_t i = 1; i < raw_traffic_data.size(); ++i) {
            const auto& row = raw_traffic_data[i];
            if (row.fields.size() == 5) { // timestamp,edge_id,density,average_speed,vehicles_passed
                TrafficDataPoint tdp;
                bool all_valid = true;
                all_valid &= Utils::string_to_int(row.fields[0], tdp.timestamp);
                all_valid &= Utils::string_to_int(row.fields[1], tdp.edge_id);
                all_valid &= Utils::string_to_double(row.fields[2], tdp.density);
                all_valid &= Utils::string_to_double(row.fields[3], tdp.average_speed);
                all_valid &= Utils::string_to_int(row.fields[4], tdp.vehicles_passed);
                if (all_valid) {
                    traffic_data_points.push_back(tdp);
                } else {
                    std::cerr << "Warning: Could not parse traffic data row: " << i+1 << std::endl;
                }
            }
        }
    }
    optimizer.load_traffic_data(traffic_data_points);
    std::cout << "Optimizer loaded " << optimizer.get_traffic_data().size() << " traffic data points." << std::endl;

    // 5. Initialize Visualizer
    Visualization::TextVisualizer visualizer;
    std::cout << "Text visualizer initialized." << std::endl;

    // 6. Run Simulation Loop
    int simulation_duration_ticks = 100; // Shorter duration for quick runs with visualization
    const int spawn_interval_info = 20; // Matches SPAWN_INTERVAL in Simulation class for info message
    bool enable_visualization = true; // Set to false to disable visualizer and run faster

    std::cout << "\n--- Starting Simulation Loop (Duration: " << simulation_duration_ticks << " ticks) ---" << std::endl;
    if (enable_visualization) {
        std::cout << "(Text visualization is ON. May cause rapid scrolling.)" << std::endl;
    } else {
        std::cout << "(Text visualization is OFF for faster execution.)" << std::endl;
    }
    std::cout << "(Vehicle Spawn Interval in Sim: Approx every " << spawn_interval_info << " ticks)" << std::endl;


    for (int i = 0; i < simulation_duration_ticks; ++i) {
        sim.tick();

        if (enable_visualization) {
            // Pass current simulation state to the visualizer
            visualizer.display_state(
                sim.get_current_tick(),
                sim.get_graph(),
                sim.get_vehicles(),
                sim.get_intersections()
            );
            // Add a small delay to make the text visualization readable
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 200ms delay
        } else {
            // Minimal output if visualizer is off
            if ( (i+1) % 10 == 0 || i == simulation_duration_ticks -1) { // Print every 10 ticks or on last tick
                 std::cout << "Tick: " << sim.get_current_tick()
                           << " | Active Vehicles: " << sim.get_vehicles().size() << std::endl;
            }
        }

        // Example of using optimizer (e.g., analyze every N ticks)
        if ((sim.get_current_tick() % 50 == 0) && !sim.get_intersections().empty() ) { // Every 50 ticks
            optimizer.analyze_current_conditions(sim.get_graph(), sim.get_intersections());
            // Example: Get suggestions for the first intersection (if any)
            int first_intersection_id = sim.get_intersections().begin()->first;
            std::map<int, int> suggested_timings = optimizer.suggest_new_signal_timings(first_intersection_id);
            if (!suggested_timings.empty()) {
                std::cout << "Optimizer suggested new timings for intersection " << first_intersection_id << " (not applied in this demo)." << std::endl;
            }
        }
    }

    std::cout << "\n--- Simulation Loop Finished ---" << std::endl;
    std::cout << "Final tick: " << sim.get_current_tick() << std::endl;
    std::cout << "Final active vehicles: " << sim.get_vehicles().size() << std::endl;

    return 0;
}
