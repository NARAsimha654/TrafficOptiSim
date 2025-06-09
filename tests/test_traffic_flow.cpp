#include <iostream>
#include <vector>
#include <string>
#include <cassert> // For assert()
#include <fstream> // For dummy csv file
#include <cstdio>  // For std::remove

// Include headers from the main project
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"
#include "simulation.hpp"
#include "utils.hpp"

// Basic test runner state
int tests_run = 0;
int tests_passed = 0;

// Helper macro for assertions in tests
#define RUN_TEST(test_function)                                      \
    tests_run++;                                                     \
    std::cout << "Running test: " #test_function "..." << std::endl; \
    if (test_function())                                             \
    {                                                                \
        tests_passed++;                                              \
        std::cout << #test_function << " PASSED." << std::endl;      \
    }                                                                \
    else                                                             \
    {                                                                \
        std::cout << #test_function << " FAILED." << std::endl;      \
    }                                                                \
    std::cout << "----------------------------------------" << std::endl;

// --- Test Functions ---

bool test_graph_creation_and_pathfinding()
{
    Graph g;
    // FIXED: Added dummy coordinates to all add_node calls
    g.add_node(1, 0.0, 0.0);
    g.add_node(2, 0.0, 0.0);
    g.add_node(3, 0.0, 0.0);
    g.add_node(4, 0.0, 0.0);

    g.add_edge(12, 1, 2, 10);
    g.add_edge(23, 2, 3, 5);
    g.add_edge(13, 1, 3, 20);
    g.add_edge(34, 3, 4, 8);

    std::vector<int> path = g.find_shortest_path(1, 4);
    std::vector<int> expected_path = {1, 2, 3, 4};

    assert(path.size() == expected_path.size());
    for (size_t i = 0; i < path.size(); ++i)
    {
        assert(path[i] == expected_path[i]);
    }

    std::vector<int> path_to_self = g.find_shortest_path(1, 1);
    assert(path_to_self.size() == 1 && path_to_self[0] == 1);

    g.add_node(5, 0.0, 0.0); // Isolated node
    std::vector<int> no_path = g.find_shortest_path(1, 5);
    assert(no_path.empty());

    return true;
}

bool test_vehicle_creation_and_route_planning()
{
    Graph g;
    // FIXED: Added dummy coordinates
    g.add_node(1, 0.0, 0.0);
    g.add_node(2, 0.0, 0.0);
    g.add_node(3, 0.0, 0.0);
    g.add_edge(12, 1, 2, 10);
    g.add_edge(23, 2, 3, 5);

    Vehicle v(101, 1, 3);
    v.plan_route(g);

    const auto &path = v.get_current_path();
    std::vector<int> expected_path = {1, 2, 3};
    assert(path.size() == expected_path.size());
    for (size_t i = 0; i < path.size(); ++i)
    {
        assert(path[i] == expected_path[i]);
    }
    assert(v.get_state() == VehicleState::NOT_STARTED);
    return true;
}

bool test_intersection_signal_logic()
{
    std::vector<int> approaches = {10, 20};
    Intersection intersection(1, approaches);

    intersection.update_signal_state();
    assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN);
    assert(intersection.get_signal_state(approaches[1]) == LightState::RED);

    for (int i = 0; i < Intersection::GREEN_DURATION - 1; ++i)
    {
        intersection.update_signal_state();
    }
    assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN);

    intersection.update_signal_state();
    assert(intersection.get_signal_state(approaches[0]) == LightState::YELLOW);

    for (int i = 0; i < Intersection::YELLOW_DURATION - 1; ++i)
    {
        intersection.update_signal_state();
    }
    assert(intersection.get_signal_state(approaches[0]) == LightState::YELLOW);

    intersection.update_signal_state();
    assert(intersection.get_signal_state(approaches[0]) == LightState::RED);
    assert(intersection.get_signal_state(approaches[1]) == LightState::GREEN);

    return true;
}

bool test_simulation_tick_and_vehicle_movement()
{
    Simulation sim;
    Graph g;
    // FIXED: Added dummy coordinates
    g.add_node(1, 100, 100);
    g.add_node(2, 200, 100);
    g.add_node(3, 300, 100);
    g.add_edge(12, 1, 2, 3);
    g.add_edge(23, 2, 3, 2);
    sim.set_graph(g);

    Intersection int2(2, {23});
    sim.add_intersection(int2);

    Vehicle v(1, 1, 3);
    v.plan_route(g);
    sim.add_vehicle(v);

    sim.tick();
    Vehicle *v_ptr = sim.get_vehicle_by_id(1);
    assert(v_ptr != nullptr);
    assert(v_ptr->get_state() == VehicleState::EN_ROUTE);
    assert(v_ptr->get_current_node_id() == 1);
    assert(v_ptr->get_next_node_id() == 2);
    assert(v_ptr->get_current_edge_progress_ticks() == 1);
    assert(v_ptr->get_current_edge_total_ticks() == 3);

    sim.tick();
    sim.tick();
    assert(v_ptr->get_state() == VehicleState::WAITING_AT_INTERSECTION);
    assert(v_ptr->get_current_node_id() == 2);
    assert(v_ptr->get_next_node_id() == 3);

    Intersection *i_ptr = sim.get_intersection_by_id(2);
    assert(i_ptr != nullptr);
    assert(!i_ptr->get_vehicle_queue(23).empty());
    assert(i_ptr->get_vehicle_queue(23).front() == v.get_id());

    sim.tick();
    assert(i_ptr->get_signal_state(23) == LightState::GREEN);
    assert(v_ptr->get_state() == VehicleState::EN_ROUTE);
    assert(v_ptr->get_current_node_id() == 2);
    assert(v_ptr->get_next_node_id() == 3);
    assert(v_ptr->get_current_edge_progress_ticks() == 1);
    assert(v_ptr->get_current_edge_total_ticks() == 2);

    sim.tick();
    assert(v_ptr->get_state() == VehicleState::ARRIVED);
    assert(v_ptr->get_current_node_id() == 3);

    sim.tick();
    assert(sim.get_vehicle_by_id(1) == nullptr);

    return true;
}

bool test_utils_csv_parser()
{
    std::string test_csv_path = "test_temp_data.csv";
    std::ofstream outfile(test_csv_path);
    outfile << "header1,header2,header3\n";
    outfile << "data1,100,val1\n";
    outfile << "data2, 200 , val2 \n";
    outfile << "data3,300,val3";
    outfile.close();

    std::vector<Utils::CsvRow> parsed_data = Utils::parse_csv(test_csv_path);

    assert(parsed_data.size() == 4);
    if (parsed_data.size() == 4)
    {
        assert(parsed_data[1].fields.size() == 3);
        assert(parsed_data[1].fields[1] == "100");
        assert(parsed_data[2].fields[1] == "200");
    }

    std::remove(test_csv_path.c_str());

    int int_val;
    double double_val;
    assert(Utils::string_to_int(" 123 ", int_val) && int_val == 123);
    assert(!Utils::string_to_int("abc", int_val));
    assert(Utils::string_to_double(" 3.14 ", double_val) && double_val == 3.14);
    assert(!Utils::string_to_double("xyz", double_val));

    return true;
}

int main()
{
    std::cout << "--- Starting Traffic Flow Tests ---" << std::endl;
    RUN_TEST(test_graph_creation_and_pathfinding);
    RUN_TEST(test_vehicle_creation_and_route_planning);
    RUN_TEST(test_intersection_signal_logic);
    RUN_TEST(test_simulation_tick_and_vehicle_movement);
    RUN_TEST(test_utils_csv_parser);
    std::cout << "--- Test Summary ---" << std::endl;
    std::cout << "Total tests run: " << tests_run << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
    std::cout << "--------------------" << std::endl;
    return (tests_run == tests_passed) ? 0 : 1;
}