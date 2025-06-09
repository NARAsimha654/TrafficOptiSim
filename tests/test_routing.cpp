#include <iostream>
#include <vector>
#include <cassert>
#include "graph.hpp"   // For creating graph instances for vehicles to use
#include "vehicle.hpp" // The class we are testing

// Helper function to print a path (can be reused or defined locally if not shared)
void print_vehicle_path(const std::string &test_name, const Vehicle &vehicle)
{
    std::cout << "   " << test_name << ": Vehicle " << vehicle.get_id()
              << " (Path: " << vehicle.get_source_node_id() << " -> " << vehicle.get_destination_node_id() << ") = [ ";
    const auto &path = vehicle.get_current_path();
    if (path.empty())
    {
        std::cout << "empty";
    }
    else
    {
        for (size_t i = 0; i < path.size(); ++i)
        {
            std::cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
        }
    }
    std::cout << " ]" << std::endl;
}

void test_vehicle_creation()
{
    std::cout << "Running test_vehicle_creation..." << std::endl;
    Vehicle v1(101, 1, 5);
    assert(v1.get_id() == 101);
    assert(v1.get_source_node_id() == 1);
    assert(v1.get_destination_node_id() == 5);
    assert(v1.get_current_path().empty()); // Path should be empty initially

    Vehicle v2(202, 10, 20);
    assert(v2.get_id() == 202);
    assert(v2.get_source_node_id() == 10);
    assert(v2.get_destination_node_id() == 20);
    assert(v2.get_current_path().empty());

    std::cout << "test_vehicle_creation PASSED." << std::endl;
}

void test_vehicle_plan_route()
{
    std::cout << "Running test_vehicle_plan_route..." << std::endl;

    // Setup a graph (similar to test_graph.cpp for consistency)
    Graph graph;
    // FIXED: Added dummy coordinates to all add_node calls
    for (int i = 1; i <= 6; ++i)
        graph.add_node(i, 0.0, 0.0);
    graph.add_edge(1, 1, 2, 7.0);
    graph.add_edge(2, 1, 3, 9.0);
    graph.add_edge(7, 3, 6, 2.0);
    graph.add_edge(9, 6, 5, 9.0);

    // Test 1: Vehicle plans a valid route
    Vehicle vehicle1(1, 1, 5); // Wants to go from node 1 to node 5
    vehicle1.plan_route(graph);
    std::vector<int> expected_path1 = {1, 3, 6, 5};
    print_vehicle_path("Test 1 (Vehicle 1: 1->5)", vehicle1);
    assert(vehicle1.get_current_path() == expected_path1);

    // Test 2: Vehicle tries to plan a route where no path exists
    // FIXED: Added dummy coordinates
    graph.add_node(10, 0.0, 0.0); // Add a disconnected node
    Vehicle vehicle2(2, 1, 10);   // Wants to go from node 1 to node 10
    vehicle2.plan_route(graph);
    print_vehicle_path("Test 2 (Vehicle 2: 1->10, no path)", vehicle2);
    assert(vehicle2.get_current_path().empty());

    // Test 3: Vehicle plans a route to itself
    Vehicle vehicle3(3, 2, 2);
    vehicle3.plan_route(graph);
    std::vector<int> expected_path3 = {2};
    print_vehicle_path("Test 3 (Vehicle 3: 2->2, path to self)", vehicle3);
    assert(vehicle3.get_current_path() == expected_path3);

    // Test 4: Vehicle source node does not exist in graph
    Vehicle vehicle4(4, 99, 1); // Node 99 does not exist
    vehicle4.plan_route(graph);
    print_vehicle_path("Test 4 (Vehicle 4: 99->1, source non-existent)", vehicle4);
    assert(vehicle4.get_current_path().empty());

    // Test 5: Vehicle destination node does not exist in graph
    Vehicle vehicle5(5, 1, 88); // Node 88 does not exist
    vehicle5.plan_route(graph);
    print_vehicle_path("Test 5 (Vehicle 5: 1->88, dest non-existent)", vehicle5);
    assert(vehicle5.get_current_path().empty());

    std::cout << "test_vehicle_plan_route PASSED." << std::endl;
}

int main()
{
    std::cout << "Starting Vehicle routing tests (test_routing.cpp)..." << std::endl;
    test_vehicle_creation();
    test_vehicle_plan_route();
    std::cout << "All Vehicle routing tests PASSED." << std::endl;
    return 0;
}