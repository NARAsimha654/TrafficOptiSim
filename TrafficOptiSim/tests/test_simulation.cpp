#include <iostream>
#include <vector>
#include <cassert>
#include <map> // For checking vehicle map directly
#include "simulation.hpp"
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"

void test_simulation_creation_and_setup() {
    std::cout << "Running test_simulation_creation_and_setup..." << std::endl;
    Simulation sim;
    assert(sim.get_current_tick() == 0);
    assert(sim.get_vehicles().empty());
    assert(sim.get_intersections().empty());
    assert(sim.get_graph().get_all_nodes().empty());

    Graph g;
    g.add_node(1); g.add_node(2);
    g.add_edge(10, 1, 2, 1.0);
    sim.set_graph(g);
    assert(sim.get_graph().get_all_nodes().size() == 2);

    Intersection intersection1(1, {10}); // Node 1, outgoing edge 10
    sim.add_intersection(intersection1);
    assert(sim.get_intersections().size() == 1);
    assert(sim.get_intersection_by_id(1) != nullptr);

    Vehicle vehicle1(1001, 1, 2);
    sim.add_vehicle(vehicle1); // Vehicle starts in NOT_STARTED
    assert(sim.get_vehicles().size() == 1);
    assert(sim.get_vehicle_by_id(1001) != nullptr);
    std::cout << "test_simulation_creation_and_setup PASSED." << std::endl;
}

void test_single_vehicle_full_journey() {
    std::cout << "Running test_single_vehicle_full_journey..." << std::endl;
    Simulation sim;
    Graph g;
    g.add_node(1); g.add_node(2); g.add_node(3);
    g.add_edge(12, 1, 2, 3); // Edge 1->2, ID 12, weight 3 ticks
    g.add_edge(23, 2, 3, 4); // Edge 2->3, ID 23, weight 4 ticks
    sim.set_graph(g);

    Intersection int1(1, {12});
    Intersection int2(2, {23});
    sim.add_intersection(int1);
    sim.add_intersection(int2);

    Vehicle car(1, 1, 3);
    car.plan_route(sim.get_graph());
    assert(!car.get_current_path().empty());
    sim.add_vehicle(car);

    int total_ticks_to_observe = 0;
    Vehicle* p_car = sim.get_vehicle_by_id(1);
    assert(p_car != nullptr);

    // Tick 1: car starts journey (NOT_STARTED -> EN_ROUTE), progress 0/3 on edge 12. Int1(12) GREEN.
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE);
    assert(p_car->get_current_node_id() == 1 && p_car->get_next_node_id() == 2);
    assert(p_car->get_current_edge_total_ticks() == 3 && p_car->get_current_edge_progress_ticks() == 0);
    assert(sim.get_intersection_by_id(1)->get_signal_state(12) == LightState::GREEN);

    // Tick 2: car EN_ROUTE, progress 1/3 on edge 12
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE && p_car->get_current_edge_progress_ticks() == 1);

    // Tick 3: car EN_ROUTE, progress 2/3 on edge 12
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE && p_car->get_current_edge_progress_ticks() == 2);

    // Tick 4: car EN_ROUTE, progress 3/3. Arrives at node 2. State WAITING_AT_INTERSECTION for edge 23.
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::WAITING_AT_INTERSECTION);
    assert(p_car->get_current_node_id() == 2 && p_car->get_next_node_id() == 3);
    assert(!sim.get_intersection_by_id(2)->get_vehicle_queue(23).empty() && sim.get_intersection_by_id(2)->get_vehicle_queue(23).front() == car.get_id());

    // Tick 5: car WAITING. Int2(edge 23) is GREEN (became GREEN at tick 1). Car departs.
    // New state EN_ROUTE for edge 23 (2->3), progress 0/4.
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE);
    assert(p_car->get_current_node_id() == 2 && p_car->get_next_node_id() == 3);
    assert(p_car->get_current_edge_total_ticks() == 4 && p_car->get_current_edge_progress_ticks() == 0);
    assert(sim.get_intersection_by_id(2)->get_vehicle_queue(23).empty());

    // Tick 6: car EN_ROUTE, progress 1/4 on edge 23
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE && p_car->get_current_edge_progress_ticks() == 1);

    // Tick 7: car EN_ROUTE, progress 2/4 on edge 23
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE && p_car->get_current_edge_progress_ticks() == 2);

    // Tick 8: car EN_ROUTE, progress 3/4 on edge 23
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE && p_car->get_current_edge_progress_ticks() == 3);

    // Tick 9: car EN_ROUTE, progress 4/4. Arrives at node 3 (destination). State ARRIVED.
    sim.tick(); total_ticks_to_observe++;
    assert(p_car->get_current_node_id() == 3 && p_car->get_state() == VehicleState::ARRIVED);

    std::cout << "test_single_vehicle_full_journey PASSED (in " << total_ticks_to_observe << " sim ticks)." << std::endl;
}

void test_vehicle_spawning_and_despawning() {
    std::cout << "Running test_vehicle_spawning_and_despawning..." << std::endl;
    // Test Spawning
    Simulation sim_spawn;
    Graph g_spawn;
    g_spawn.add_node(1); g_spawn.add_node(2); g_spawn.add_node(3);
    g_spawn.add_edge(12, 1, 2, 2); // Edge 1->2
    g_spawn.add_edge(23, 2, 3, 3); // Edge 2->3
    // Add reverse edges if needed for more diverse spawning, or more nodes
    g_spawn.add_edge(21, 2, 1, 2);
    g_spawn.add_edge(32, 3, 2, 3);
    sim_spawn.set_graph(g_spawn);
    sim_spawn.add_intersection(Intersection(1, {12})); // Controls outgoing from 1
    sim_spawn.add_intersection(Intersection(2, {21, 23})); // Controls outgoing from 2
    sim_spawn.add_intersection(Intersection(3, {32})); // Controls outgoing from 3

    int spawn_interval = 20;
    assert(sim_spawn.get_vehicles().empty());
    for(int i=0; i < spawn_interval; ++i) {
        sim_spawn.tick();
    }
    assert(!sim_spawn.get_vehicles().empty());

    // Test Despawning
    Simulation sim_despawn;
    Graph g_despawn; // Use a simple graph for despawn test
    g_despawn.add_node(10); g_despawn.add_node(20);
    g_despawn.add_edge(1020, 10, 20, 2); // Edge 10->20, weight 2
    sim_despawn.set_graph(g_despawn);
    sim_despawn.add_intersection(Intersection(10, {1020})); // Intersection at node 10 controls edge 1020

    Vehicle v_despawn(100, 10, 20);
    v_despawn.plan_route(sim_despawn.get_graph());
    assert(!v_despawn.get_current_path().empty());
    sim_despawn.add_vehicle(v_despawn);
    assert(sim_despawn.get_vehicles().count(100));

    // Tick 1: NOT_STARTED -> EN_ROUTE (prog 0/2). Int 10 (edge 1020) GREEN.
    sim_despawn.tick();
    Vehicle* p_vd = sim_despawn.get_vehicle_by_id(100);
    assert(p_vd != nullptr && p_vd->get_state() == VehicleState::EN_ROUTE && p_vd->get_current_edge_progress_ticks() == 0);

    // Tick 2: EN_ROUTE (prog 1/2)
    sim_despawn.tick();
    assert(p_vd != nullptr && p_vd->get_state() == VehicleState::EN_ROUTE && p_vd->get_current_edge_progress_ticks() == 1);

    // Tick 3: EN_ROUTE (prog 2/2) -> ARRIVED. Vehicle is despawned at the end of this tick.
    sim_despawn.tick();
    assert(sim_despawn.get_vehicle_by_id(100) == nullptr); // Assert it's gone

    std::cout << "test_vehicle_spawning_and_despawning PASSED." << std::endl;
}

int main() {
    std::cout << "Starting Simulation tests (test_simulation.cpp)..." << std::endl;
    test_simulation_creation_and_setup();
    test_single_vehicle_full_journey();
    test_vehicle_spawning_and_despawning();
    std::cout << "All Simulation tests PASSED." << std::endl;
    return 0;
}
