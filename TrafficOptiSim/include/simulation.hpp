#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <map>
#include <vector>
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"

class Simulation {
public:
    Simulation();

    // Setup methods
    void set_graph(const Graph& graph);
    // Note: We store copies of vehicles and intersections.
    // Consider using smart pointers if complex ownership or polymorphism is needed later.
    void add_vehicle(const Vehicle& vehicle);
    void add_intersection(const Intersection& intersection);

    // Core simulation step
    void tick();

    // Accessors
    int get_current_tick() const;
    const Graph& get_graph() const;
    const std::map<int, Vehicle>& get_vehicles() const;
    const std::map<int, Intersection>& get_intersections() const;
    // Mutable accessors might be needed for internal operations or testing
    Vehicle* get_vehicle_by_id(int vehicle_id); // Returns nullptr if not found
    Intersection* get_intersection_by_id(int intersection_id); // Returns nullptr if not found


private:
    Graph graph_;
    std::map<int, Vehicle> vehicles_; // Key: vehicle_id
    std::map<int, Intersection> intersections_; // Key: intersection_id (node_id from graph)
    int current_tick_;
};

#endif // SIMULATION_HPP
