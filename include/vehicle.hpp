#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <vector>
#include <string> // For potential string state representation
#include "graph.hpp" // Needs graph to plan routes

enum class VehicleState {
    NOT_STARTED,            // Initial state before path is planned or journey started
    EN_ROUTE,               // Moving along an edge
    WAITING_AT_INTERSECTION, // Waiting at an intersection (e.g., at a red light or in queue)
    ARRIVED                 // Reached destination
};

// Helper to convert VehicleState to string
std::string vehicle_state_to_string(VehicleState state);

class Vehicle {
public:
    Vehicle(int id, int source_node_id, int destination_node_id);

    void plan_route(const Graph& graph);
    // Call this after plan_route to initialize movement-related state
    void start_journey(const Graph& graph);

    // Accessors
    int get_id() const;
    int get_source_node_id() const;
    int get_destination_node_id() const;
    const std::vector<int>& get_current_path() const;
    VehicleState get_state() const;
    int get_current_node_id() const; // Last passed intersection or current if waiting
    int get_next_node_id() const;    // Next intersection in the path
    int get_current_edge_progress_ticks() const;
    int get_current_edge_total_ticks() const;

    // Mutators (to be called by Simulation class)
    void set_state(VehicleState new_state);
    void set_current_node_id(int node_id);
    void set_next_node_id(int node_id); // Typically set when starting a new edge
    void set_current_edge_ticks(int progress, int total);
    void increment_edge_progress_ticks();


private:
    int id_;
    int source_node_id_;
    int destination_node_id_;
    std::vector<int> current_path_;

    VehicleState state_;
    int current_node_id_; // Represents the start node of the current edge, or current intersection if waiting.
    int next_node_id_;    // Represents the end node of the current edge.
    int current_edge_progress_ticks_; // Ticks spent on the current edge.
    int current_edge_total_ticks_;    // Total ticks required for the current edge.
};

#endif // VEHICLE_HPP
