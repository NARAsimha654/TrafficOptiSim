#include <iostream>
#include <vector>
#include <cassert>
#include "simulation.hpp" // The class we are testing
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"

void test_simulation_creation_and_setup() {
    std::cout << "Running test_simulation_creation_and_setup..." << std::endl;
    Simulation sim;

    // Test initial state
    assert(sim.get_current_tick() == 0);
    assert(sim.get_vehicles().empty());
    assert(sim.get_intersections().empty());
    // Graph is default constructed, check a basic property like no nodes
    assert(sim.get_graph().get_all_nodes().empty());

    // Test set_graph
    Graph g;
    g.add_node(1);
    g.add_node(2);
    g.add_edge(10, 1, 2, 1.0);
    sim.set_graph(g);
    assert(sim.get_graph().get_all_nodes().size() == 2);
    assert(sim.get_graph().has_edge_between(1,2));

    // Test add_intersection
    std::vector<int> approaches1 = {101, 102}; // Dummy approach IDs
    Intersection intersection1(1, approaches1); // Intersection ID 1
    sim.add_intersection(intersection1);
    assert(sim.get_intersections().size() == 1);
    assert(sim.get_intersection_by_id(1) != nullptr);
    assert(sim.get_intersection_by_id(1)->get_id() == 1);
    assert(sim.get_intersection_by_id(99) == nullptr); // Non-existent

    // Test add_vehicle
    Vehicle vehicle1(1001, 1, 2); // Vehicle ID 1001, from node 1 to 2
    sim.add_vehicle(vehicle1);
    assert(sim.get_vehicles().size() == 1);
    assert(sim.get_vehicle_by_id(1001) != nullptr);
    assert(sim.get_vehicle_by_id(1001)->get_id() == 1001);
    assert(sim.get_vehicle_by_id(9999) == nullptr); // Non-existent

    std::cout << "test_simulation_creation_and_setup PASSED." << std::endl;
}

void test_simulation_tick() {
    std::cout << "Running test_simulation_tick..." << std::endl;
    Simulation sim;

    // Setup graph
    Graph g;
    g.add_node(1); // Intersection node
    sim.set_graph(g);

    // Setup intersection
    std::vector<int> approaches = {10, 20}; // Approach 10, Approach 20 for intersection 1
    Intersection intersection(1, approaches); // Intersection ID 1
    sim.add_intersection(intersection);

    // Initial tick is 0
    assert(sim.get_current_tick() == 0);
    // Initial state of intersection 1, approach 10 should be RED
    // Get a pointer to the intersection managed by the simulation
    Intersection* sim_int1_ptr = sim.get_intersection_by_id(1);
    assert(sim_int1_ptr != nullptr);
    assert(sim_int1_ptr->get_signal_state(approaches[0]) == LightState::RED);

    // Tick 1
    sim.tick();
    assert(sim.get_current_tick() == 1);
    // After 1st sim tick, intersection's update_signal_state is called.
    // Intersection's logic: first approach (10) becomes GREEN.
    assert(sim_int1_ptr->get_signal_state(approaches[0]) == LightState::GREEN);
    assert(sim_int1_ptr->get_signal_state(approaches[1]) == LightState::RED);

    // Tick multiple times to pass GREEN_DURATION for approach 10
    const int green_duration = 15; // From Intersection class
    for (int i = 0; i < green_duration -1; ++i) { // Already ticked once for GREEN
        sim.tick(); // Ticks 2 through 15
        assert(sim_int1_ptr->get_signal_state(approaches[0]) == LightState::GREEN);
    }
    assert(sim.get_current_tick() == green_duration);

    // Next tick should make approach 10 YELLOW
    sim.tick(); // Tick 16
    assert(sim.get_current_tick() == green_duration + 1);
    assert(sim_int1_ptr->get_signal_state(approaches[0]) == LightState::YELLOW);
    assert(sim_int1_ptr->get_signal_state(approaches[1]) == LightState::RED);

    // Tick multiple times to pass YELLOW_DURATION for approach 10
    const int yellow_duration = 3; // From Intersection class
    for (int i = 0; i < yellow_duration - 1; ++i) { // Already ticked once for YELLOW
        sim.tick(); // Ticks 17 through 18 (if green=15, yellow=3)
        assert(sim_int1_ptr->get_signal_state(approaches[0]) == LightState::YELLOW);
    }
    assert(sim.get_current_tick() == green_duration + yellow_duration);

    // Next tick should make approach 10 RED, and approach 20 GREEN
    sim.tick();
    assert(sim.get_current_tick() == green_duration + yellow_duration + 1);
    assert(sim_int1_ptr->get_signal_state(approaches[0]) == LightState::RED);
    assert(sim_int1_ptr->get_signal_state(approaches[1]) == LightState::GREEN);


    std::cout << "test_simulation_tick PASSED." << std::endl;
}


int main() {
    std::cout << "Starting Simulation tests (test_simulation.cpp)..." << std::endl;
    test_simulation_creation_and_setup();
    test_simulation_tick();
    std::cout << "All Simulation tests PASSED." << std::endl;
    return 0;
}
