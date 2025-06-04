#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <vector>
#include "graph.hpp" // Needs graph to plan routes

class Vehicle {
public:
    // Constructor
    Vehicle(int id, int source_node_id, int destination_node_id);

    // Plans the route for the vehicle using the provided graph
    void plan_route(const Graph& graph);

    // Accessors
    int get_id() const;
    int get_source_node_id() const;
    int get_destination_node_id() const;
    const std::vector<int>& get_current_path() const;

    // Could add methods later for movement, current position on path, etc.

private:
    int id_;
    int source_node_id_;
    int destination_node_id_;
    std::vector<int> current_path_; // Sequence of node IDs representing the path
};

#endif // VEHICLE_HPP
