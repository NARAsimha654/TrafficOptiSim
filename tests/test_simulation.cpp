#include <iostream>
#include <vector>
#include <cassert>
#include <map> // For checking vehicle map directly
#include "simulation.hpp"
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"

void test_simulation_creation_and_setup()
{
    std::cout << "Running test_simulation_creation_and_setup..." << std::endl;
    Simulation sim;
    assert(sim.get_current_tick() == 0);
    assert(sim.get_vehicles().empty());

    Graph g;
    // FIXED: Added dummy coordinates
    g.add_node(1, 0.0, 0.0);
    g.add_node(2, 0.0, 0.0);
    g.add_edge(10, 1, 2, 1.0);
    sim.set_graph(g);
    assert(sim.get_graph().get_all_nodes().size() == 2);

    Intersection intersection1(1, {10});
    sim.add_intersection(intersection1);
    assert(sim.get_intersections().size() == 1);

    Vehicle vehicle1(1001, 1, 2);
    sim.add_vehicle(vehicle1);
    assert(sim.get_vehicles().size() == 1);
    std::cout << "test_simulation_creation_and_setup PASSED." << std::endl;
}

void test_single_vehicle_full_journey()
{
    std::cout << "Running test_single_vehicle_full_journey..." << std::endl;
    Simulation sim;
    Graph g;
    // FIXED: Added dummy coordinates
    g.add_node(1, 100, 100);
    g.add_node(2, 200, 100);
    g.add_node(3, 300, 100);
    g.add_edge(12, 1, 2, 3);
    g.add_edge(23, 2, 3, 4);
    sim.set_graph(g);

    Intersection int1(1, {12});
    Intersection int2(2, {23});
    sim.add_intersection(int1);
    sim.add_intersection(int2);

    Vehicle car(1, 1, 3);
    car.plan_route(sim.get_graph());
    sim.add_vehicle(car);

    int total_ticks = 0;
    Vehicle *p_car = sim.get_vehicle_by_id(1);
    assert(p_car != nullptr);

    // Tick 1: Starts and moves 1 step.
    sim.tick();
    total_ticks++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE);
    assert(p_car->get_current_edge_progress_ticks() == 1);

    // Ticks 2, 3: Finishes edge, arrives at intersection.
    sim.tick();
    total_ticks++;
    sim.tick();
    total_ticks++;
    assert(p_car->get_state() == VehicleState::WAITING_AT_INTERSECTION);
    assert(p_car->get_current_node_id() == 2);

    // Tick 4: Departs intersection, moves 1 step.
    sim.tick();
    total_ticks++;
    assert(p_car->get_state() == VehicleState::EN_ROUTE);
    assert(p_car->get_current_edge_progress_ticks() == 1);

    // Ticks 5, 6, 7: Finishes edge, arrives at destination.
    sim.tick();
    total_ticks++;
    sim.tick();
    total_ticks++;
    sim.tick();
    total_ticks++;
    assert(p_car->get_state() == VehicleState::ARRIVED);

    // Tick 8: Gets despawned.
    sim.tick();
    total_ticks++;
    assert(sim.get_vehicle_by_id(1) == nullptr);

    std::cout << "test_single_vehicle_full_journey PASSED (in " << total_ticks << " sim ticks)." << std::endl;
}

void test_vehicle_spawning_and_despawning()
{
    std::cout << "Running test_vehicle_spawning_and_despawning..." << std::endl;
    // Test Spawning
    Simulation sim_spawn;
    Graph g_spawn;
    // FIXED: Added dummy coordinates
    g_spawn.add_node(1, 0, 0);
    g_spawn.add_node(2, 0, 0);
    g_spawn.add_node(3, 0, 0);
    g_spawn.add_edge(12, 1, 2, 2);
    g_spawn.add_edge(23, 2, 3, 3);
    g_spawn.add_edge(21, 2, 1, 2);
    g_spawn.add_edge(32, 3, 2, 3);
    sim_spawn.set_graph(g_spawn);
    sim_spawn.add_intersection(Intersection(2, {21, 23}));

    for (int i = 0; i < 20; ++i)
        sim_spawn.tick();
    assert(!sim_spawn.get_vehicles().empty());

    // Test Despawning
    Simulation sim_despawn;
    Graph g_despawn;
    // FIXED: Added dummy coordinates
    g_despawn.add_node(10, 0, 0);
    g_despawn.add_node(20, 0, 0);
    g_despawn.add_edge(1020, 10, 20, 2);
    sim_despawn.set_graph(g_despawn);
    sim_despawn.add_intersection(Intersection(10, {1020}));

    Vehicle v_despawn(100, 10, 20);
    v_despawn.plan_route(sim_despawn.get_graph());
    sim_despawn.add_vehicle(v_despawn);

    // Tick 1: Starts and moves 1 step.
    sim_despawn.tick();
    Vehicle *p_vd = sim_despawn.get_vehicle_by_id(100);
    assert(p_vd != nullptr && p_vd->get_current_edge_progress_ticks() == 1);

    // Tick 2: Arrives and is despawned.
    sim_despawn.tick();
    assert(sim_despawn.get_vehicle_by_id(100) == nullptr);

    std::cout << "test_vehicle_spawning_and_despawning PASSED." << std::endl;
}

int main()
{
    std::cout << "Starting Simulation tests (test_simulation.cpp)..." << std::endl;
    test_simulation_creation_and_setup();
    test_single_vehicle_full_journey();
    test_vehicle_spawning_and_despawning();
    std::cout << "All Simulation tests PASSED." << std::endl;
    return 0;
}