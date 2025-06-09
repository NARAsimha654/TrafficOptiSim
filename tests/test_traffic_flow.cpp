#include <iostream>
#include <vector>
#include <string>
#include <cassert> // For assert()

// Include headers from the main project
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"
#include "simulation.hpp"
#include "utils.hpp" // For parsing test data if needed, or other utilities

// Basic test runner state
int tests_run = 0;
int tests_passed = 0;

// Helper macro for assertions in tests
#define RUN_TEST(test_function) \
    tests_run++; \
    std::cout << "Running test: " #test_function "..." << std::endl; \
    if (test_function()) { \
        tests_passed++; \
        std::cout << #test_function << " PASSED." << std::endl; \
    } else { \
        std::cout << #test_function << " FAILED." << std::endl; \
    } \
    std::cout << "----------------------------------------" << std::endl;

// --- Test Functions ---

bool test_graph_creation_and_pathfinding() {
    Graph g;
    // Nodes
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);

    // Edges (id, from, to, weight)
    g.add_edge(12, 1, 2, 10); // Edge from 1 to 2, weight 10
    g.add_edge(23, 2, 3, 5);  // Edge from 2 to 3, weight 5
    g.add_edge(13, 1, 3, 20); // Edge from 1 to 3, weight 20 (longer path)
    g.add_edge(34, 3, 4, 8);  // Edge from 3 to 4, weight 8

    std::vector<int> path = g.find_shortest_path(1, 4);
    std::vector<int> expected_path = {1, 2, 3, 4}; // Expected path 1 -> 2 -> 3 -> 4

    assert(path.size() == expected_path.size());
    for (size_t i = 0; i < path.size(); ++i) {
        assert(path[i] == expected_path[i]);
    }

    // Test path to self
    std::vector<int> path_to_self = g.find_shortest_path(1,1);
    assert(path_to_self.size() == 1 && path_to_self[0] == 1);

    // Test non-existent path
    g.add_node(5); // Isolated node
    std::vector<int> no_path = g.find_shortest_path(1,5);
    assert(no_path.empty());

    return true; // Return true if all asserts pass
}

bool test_vehicle_creation_and_route_planning() {
    Graph g;
    g.add_node(1); g.add_node(2); g.add_node(3);
    g.add_edge(12, 1, 2, 10);
    g.add_edge(23, 2, 3, 5);

    Vehicle v(101, 1, 3);
    v.plan_route(g);

    const auto& path = v.get_current_path();
    std::vector<int> expected_path = {1, 2, 3};
    assert(path.size() == expected_path.size());
    for (size_t i = 0; i < path.size(); ++i) {
        assert(path[i] == expected_path[i]);
    }
    assert(v.get_state() == VehicleState::NOT_STARTED);
    return true;
}

bool test_intersection_signal_logic() {
    std::vector<int> approaches = {10, 20}; // Assume these are outgoing edge IDs
    Intersection intersection(1, approaches);

    // Initial state check (after first update_signal_state call implicitly done by simulation or if manually called)
    // For simplicity, we'll call update_signal_state manually to cycle through states.
    // The Intersection constructor sets up initial RED, first update_signal_state makes first approach GREEN.

    intersection.update_signal_state(); // Tick 1: Approach 10 should be GREEN
    assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN);
    assert(intersection.get_signal_state(approaches[1]) == LightState::RED);

    // Cycle through GREEN_DURATION for first approach
    for (int i = 0; i < Intersection::GREEN_DURATION -1; ++i) { // -1 because one update already happened
        intersection.update_signal_state();
    }
    assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN); // Still green

    intersection.update_signal_state(); // Switch to YELLOW
    assert(intersection.get_signal_state(approaches[0]) == LightState::YELLOW);

    // Cycle through YELLOW_DURATION
    for (int i = 0; i < Intersection::YELLOW_DURATION -1; ++i) {
        intersection.update_signal_state();
    }
    assert(intersection.get_signal_state(approaches[0]) == LightState::YELLOW); // Still yellow

    intersection.update_signal_state(); // Switch to RED, next approach GREEN
    assert(intersection.get_signal_state(approaches[0]) == LightState::RED);
    assert(intersection.get_signal_state(approaches[1]) == LightState::GREEN);

    return true;
}

bool test_simulation_tick_and_vehicle_movement() {
    Simulation sim;
    Graph g;
    g.add_node(1); g.add_node(2); g.add_node(3);
    g.add_edge(12, 1, 2, 3); // Edge 12: node 1 to 2, travel time 3 ticks
    g.add_edge(23, 2, 3, 2); // Edge 23: node 2 to 3, travel time 2 ticks
    sim.set_graph(g);

    // Intersection at node 2, controls movement from edge 12 to edge 23.
    // The approach ID for an intersection is the *outgoing* edge ID.
    Intersection int2(2, {23}); // Intersection at node 2, with outgoing edge 23.
    sim.add_intersection(int2);

    // Make sure int2's signal for edge 23 is initially Green for this test
    // We need to tick the simulation a bit for the intersection signals to cycle or set it up.
    // For this test, let's assume the simulation's intersection update will eventually make it green.
    // Or, more robustly, control the intersection state directly if possible, or run enough ticks.
    // The intersection starts with the first approach (23) becoming green after the first update.

    Vehicle v(1, 1, 3); // Vehicle from node 1 to node 3
    v.plan_route(g); // Path will be 1 -> 2 -> 3
    sim.add_vehicle(v);

    // Tick 1: Vehicle should start its journey on edge 12.
    sim.tick(); // Intersection updates; Vehicle changes from NOT_STARTED to EN_ROUTE
    Vehicle* v_ptr = sim.get_vehicle_by_id(1);
    assert(v_ptr != nullptr);
    assert(v_ptr->get_state() == VehicleState::EN_ROUTE);
    assert(v_ptr->get_current_node_id() == 1);
    assert(v_ptr->get_next_node_id() == 2);
    assert(v_ptr->get_current_edge_progress_ticks() == 1);
    assert(v_ptr->get_current_edge_total_ticks() == 3);

    // Tick 2 & 3: Vehicle continues on edge 12
    sim.tick(); // Progress = 2
    sim.tick(); // Progress = 3. Vehicle arrives at node 2 (intersection)
    assert(v_ptr->get_state() == VehicleState::WAITING_AT_INTERSECTION);
    assert(v_ptr->get_current_node_id() == 2); // Arrived at intersection node 2
    assert(v_ptr->get_next_node_id() == 3);   // Wants to go to node 3 next

    // Check if vehicle is in queue for edge 23 at intersection 2
    Intersection* i_ptr = sim.get_intersection_by_id(2);
    assert(i_ptr != nullptr);
    assert(!i_ptr->get_vehicle_queue(23).empty());
    assert(i_ptr->get_vehicle_queue(23).front() == v.get_id());

    // Tick 4: Intersection 2 signal for edge 23 should be GREEN (assuming default cycle)
    // Vehicle should depart from intersection 2 towards node 3 on edge 23
    sim.tick();
    // Note: Intersection signal update happens at the START of the tick.
    // Vehicle state update (reacting to green light) happens AFTER signal update.
    assert(i_ptr->get_signal_state(23) == LightState::GREEN); // Assuming it's green by now
    assert(v_ptr->get_state() == VehicleState::EN_ROUTE);
    assert(v_ptr->get_current_node_id() == 2); // Departed from 2
    assert(v_ptr->get_next_node_id() == 3);   // Heading to 3
    assert(v_ptr->get_current_edge_progress_ticks() == 1);
    assert(v_ptr->get_current_edge_total_ticks() == 2); // Travel time for edge 23

    // Tick 5: Vehicle continues on edge 23
    sim.tick(); // Progress = 2. Vehicle arrives at node 3 (destination)
    assert(v_ptr->get_state() == VehicleState::ARRIVED);
    assert(v_ptr->get_current_node_id() == 3);

    // Tick 6: Vehicle should be despawned
    sim.tick();
    assert(sim.get_vehicle_by_id(1) == nullptr);

    return true;
}

bool test_utils_csv_parser() {
    // Create a dummy CSV file for testing
    std::string test_csv_path = "test_temp_data.csv";
    std::ofstream outfile(test_csv_path);
    outfile << "header1,header2,header3\n";
    outfile << "data1,100,val1\n";
    outfile << "data2, 200 , val2 \n"; // With spaces
    outfile << "data3,300,val3";        // No newline at end
    outfile.close();

    std::vector<Utils::CsvRow> parsed_data = Utils::parse_csv(test_csv_path);

    assert(parsed_data.size() == 4); // 1 header + 3 data rows
    if (parsed_data.size() == 4) {
        assert(parsed_data[0].fields.size() == 3);
        assert(parsed_data[0].fields[0] == "header1");

        assert(parsed_data[1].fields.size() == 3);
        assert(parsed_data[1].fields[0] == "data1");
        assert(parsed_data[1].fields[1] == "100");

        assert(parsed_data[2].fields.size() == 3);
        assert(parsed_data[2].fields[1] == "200"); // Check trimming
        assert(parsed_data[2].fields[2] == "val2");

        assert(parsed_data[3].fields.size() == 3);
        assert(parsed_data[3].fields[0] == "data3");
    }

    // Clean up dummy file
    std::remove(test_csv_path.c_str());

    // Test string to int/double
    int int_val;
    double double_val;
    assert(Utils::string_to_int("  123  ", int_val) && int_val == 123);
    assert(!Utils::string_to_int("abc", int_val));
    assert(Utils::string_to_double("  3.14  ", double_val) && double_val == 3.14);
    assert(!Utils::string_to_double("xyz", double_val));

    return true;
}


// --- Main Test Execution ---
int main() {
    std::cout << "--- Starting Traffic Flow Tests ---" << std::endl;

    RUN_TEST(test_graph_creation_and_pathfinding);
    RUN_TEST(test_vehicle_creation_and_route_planning);
    RUN_TEST(test_intersection_signal_logic);
    // test_simulation_tick_and_vehicle_movement is a bit more complex due to timing
    // It might need more controlled setup or specific helper functions in Simulation/Intersection for testing.
    // For now, it provides a good scenario.
    RUN_TEST(test_simulation_tick_and_vehicle_movement);
    RUN_TEST(test_utils_csv_parser);


    std::cout << "--- Test Summary ---" << std::endl;
    std::cout << "Total tests run: " << tests_run << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
    std::cout << "--------------------" << std::endl;

    return (tests_run == tests_passed) ? 0 : 1; // Return 0 on success, 1 on failure
}
