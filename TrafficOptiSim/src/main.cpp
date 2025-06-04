#include <iostream>
#include <vector>
#include <string> // Required for light_state_to_string
#include "simulation.hpp"
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp" // Also brings in light_state_to_string declaration

int main() {
    std::cout << "TrafficOptiSim - Basic Simulation Run" << std::endl;
    std::cout << "------------------------------------" << std::endl;

    // 1. Create Graph
    Graph city_graph;
    // Nodes (Intersections)
    city_graph.add_node(1); // Intersection 1
    city_graph.add_node(2); // Intersection 2
    city_graph.add_node(3); // Intersection 3

    // Edges (Roads) - Using edge IDs as approach IDs for simplicity here
    // Edge ID, From Node, To Node, Weight
    city_graph.add_edge(12, 1, 2, 10.0); // Edge from I1 to I2 (approach for I2 is 12)
    city_graph.add_edge(21, 2, 1, 10.0); // Edge from I2 to I1 (approach for I1 is 21)
    city_graph.add_edge(23, 2, 3, 8.0);  // Edge from I2 to I3 (approach for I3 is 23)
    city_graph.add_edge(32, 3, 2, 8.0);  // Edge from I3 to I2 (approach for I2 is 32)
    city_graph.add_edge(13, 1, 3, 15.0); // Edge from I1 to I3 (approach for I3 is 13)
    city_graph.add_edge(31, 3, 1, 15.0); // Edge from I3 to I1 (approach for I1 is 31)

    std::cout << "Graph created with 3 nodes and 6 edges." << std::endl;

    // 2. Create Intersections
    // For Intersection 1 (Node 1), approaches are incoming edges: 21, 31
    Intersection int1(1, {21, 31});
    // For Intersection 2 (Node 2), approaches are incoming edges: 12, 32
    Intersection int2(2, {12, 32});
    // For Intersection 3 (Node 3), approaches are incoming edges: 23, 13
    Intersection int3(3, {23, 13});

    std::cout << "Intersections created." << std::endl;

    // 3. Create Simulation
    Simulation sim;
    sim.set_graph(city_graph);
    sim.add_intersection(int1);
    sim.add_intersection(int2);
    sim.add_intersection(int3);

    std::cout << "Simulation initialized with graph and intersections." << std::endl;

    // 4. Create Vehicles
    Vehicle car1(101, 1, 3); // Car 101 from Intersection 1 to Intersection 3
    Vehicle car2(102, 3, 1); // Car 102 from Intersection 3 to Intersection 1

    // Plan routes for vehicles
    car1.plan_route(sim.get_graph());
    car2.plan_route(sim.get_graph());

    sim.add_vehicle(car1);
    sim.add_vehicle(car2);

    std::cout << "Vehicles created and added to simulation." << std::endl;
    std::cout << "Vehicle 101 (1->3) Path: ";
    for (int node_id : car1.get_current_path()) { std::cout << node_id << " "; }
    std::cout << std::endl;
    std::cout << "Vehicle 102 (3->1) Path: ";
    for (int node_id : car2.get_current_path()) { std::cout << node_id << " "; }
    std::cout << std::endl;


    // 5. Run Simulation Loop
    int simulation_duration_ticks = 60; // Run for 60 ticks
    std::cout << "\n--- Starting Simulation Loop (Duration: " << simulation_duration_ticks << " ticks) ---" << std::endl;

    for (int i = 0; i < simulation_duration_ticks; ++i) {
        sim.tick();
        std::cout << "\nTick: " << sim.get_current_tick() << std::endl;

        // Print Intersection States
        const auto& intersections = sim.get_intersections();
        for (const auto& pair : intersections) {
            const Intersection& intersection = pair.second;
            std::cout << "  Intersection " << intersection.get_id() << ":" << std::endl;
            for (int approach_id : intersection.get_approach_ids()) {
                std::cout << "    Approach " << approach_id << ": "
                          << light_state_to_string(intersection.get_signal_state(approach_id))
                          << " (Queue: " << intersection.get_vehicle_queue(approach_id).size() << ")"
                          << std::endl;
            }
        }
        // Vehicle status (basic - just show they exist, no movement yet)
        // if (i == 0) { // Print only once or periodically
        //     const auto& vehicles = sim.get_vehicles();
        //     std::cout << "  Vehicles in simulation: " << vehicles.size() << std::endl;
        //     for(const auto& v_pair : vehicles) {
        //         std::cout << "    Vehicle " << v_pair.first << " from " << v_pair.second.get_source_node_id() << " to " << v_pair.second.get_destination_node_id() << std::endl;
        //     }
        // }
    }

    std::cout << "\n--- Simulation Loop Finished ---" << std::endl;

    return 0;
}
