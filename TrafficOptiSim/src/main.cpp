#include <iostream>
#include <vector>
#include <string>
#include "simulation.hpp"
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"

int main() {
    std::cout << "TrafficOptiSim - Enhanced Simulation Run" << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    // 1. Create Graph
    Graph city_graph;
    if (!city_graph.load_from_file("data/sample_map.txt")) {
        std::cerr << "Failed to load graph from file. Exiting." << std::endl;
        return 1; // Indicate an error
    }
    std::cout << "Graph loaded successfully from data/sample_map.txt" << std::endl;
    // Verify graph size based on file
    // std::cout << "Graph has " << city_graph.get_all_nodes().size() << " nodes and "
    //           << city_graph.get_all_edges().size() << " edges." << std::endl;


    // 2. Create Intersections (approach_ids are OUTGOING edge IDs)
    Intersection int1(1, {12, 13});
    Intersection int2(2, {21, 23, 24});
    Intersection int3(3, {32, 31, 34});
    Intersection int4(4, {43, 42});

    std::cout << "Intersections created." << std::endl;

    // 3. Create Simulation
    Simulation sim;
    sim.set_graph(city_graph);
    sim.add_intersection(int1);
    sim.add_intersection(int2);
    sim.add_intersection(int3);
    sim.add_intersection(int4);

    std::cout << "Simulation initialized." << std::endl;

    // 4. Initial Vehicles (Optional - mostly rely on spawner)
    // Vehicle car1(101, 1, 4);
    // car1.plan_route(sim.get_graph());
    // sim.add_vehicle(car1);
    // std::cout << "Manually added Vehicle 101 (1->4) Path: ";
    // for (int node_id : car1.get_current_path()) { std::cout << node_id << " "; }
    // std::cout << std::endl;

    // 5. Run Simulation Loop
    int simulation_duration_ticks = 300; // Increased duration
    const int spawn_interval_info = 20; // Matches SPAWN_INTERVAL in Simulation class for info message

    std::cout << "\n--- Starting Simulation Loop (Duration: " << simulation_duration_ticks << " ticks) ---" << std::endl;
    std::cout << "(Vehicle Spawn Interval in Sim: Approx every " << spawn_interval_info << " ticks)" << std::endl;

    for (int i = 0; i < simulation_duration_ticks; ++i) {
        int vehicles_at_start_of_tick_processing = sim.get_vehicles().size();

        sim.tick(); // Simulation processes this tick, vehicles might spawn/despawn

        int vehicles_at_end_of_tick_processing = sim.get_vehicles().size();

        std::cout << "\nTick: " << sim.get_current_tick() << " | Active Vehicles: " << vehicles_at_end_of_tick_processing << std::endl;
        if (vehicles_at_end_of_tick_processing > vehicles_at_start_of_tick_processing) {
            std::cout << "  Vehicle(s) spawned this tick." << std::endl;
        } else if (vehicles_at_end_of_tick_processing < vehicles_at_start_of_tick_processing) {
            std::cout << "  Vehicle(s) despawned this tick." << std::endl;
        }

        // Print Intersection States
        const auto& intersections = sim.get_intersections();
        for (const auto& pair : intersections) {
            const Intersection& intersection = pair.second;
            std::cout << "  Intersection " << intersection.get_id() << ":" << std::endl;
            for (int approach_id : intersection.get_approach_ids()) { // approach_id is an outgoing edge ID
                std::cout << "    Outgoing Edge " << approach_id << ": Signal = "
                          << light_state_to_string(intersection.get_signal_state(approach_id))
                          << " (Queue: " << intersection.get_vehicle_queue(approach_id).size() << ")"
                          << std::endl;
            }
        }

        // Print detailed status for a few active vehicles or if count is low
        if (vehicles_at_end_of_tick_processing > 0 && (vehicles_at_end_of_tick_processing < 5 || sim.get_current_tick() % 10 == 0) ) {
            std::cout << "  Active Vehicle States:" << std::endl;
            int print_count = 0;
            for (const auto& v_pair : sim.get_vehicles()) {
                const Vehicle& v = v_pair.second;
                std::cout << "    ID " << v.get_id() << ": State=" << vehicle_state_to_string(v.get_state())
                          << ", AtNode=" << v.get_current_node_id() << ", NextNode=" << v.get_next_node_id()
                          << ", EdgePrg=" << v.get_current_edge_progress_ticks() << "/" << v.get_current_edge_total_ticks()
                          << ", PathDest=" << v.get_destination_node_id();
                // Briefly show path
                // std::cout << " Path: ";
                // for(int p_node : v.get_current_path()) { std::cout << p_node << " ";}
                std::cout << std::endl;
                if (++print_count >= 5 && vehicles_at_end_of_tick_processing > 5) { // Print max 5 if many vehicles
                    std::cout << "    ... (and " << vehicles_at_end_of_tick_processing - print_count << " more vehicles)" << std::endl;
                    break;
                }
            }
        }
    }

    std::cout << "\n--- Simulation Loop Finished ---" << std::endl;
    std::cout << "Final active vehicles: " << sim.get_vehicles().size() << std::endl;

    return 0;
}
