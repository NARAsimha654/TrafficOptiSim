#include "vehicle.hpp"
// No other specific includes needed for this basic version beyond what vehicle.hpp pulls in.

Vehicle::Vehicle(int id, int source_node_id, int destination_node_id)
    : id_(id),
      source_node_id_(source_node_id),
      destination_node_id_(destination_node_id),
      current_path_() { // Initialize path as empty
    // Path will be planned by calling plan_route separately
}

void Vehicle::plan_route(const Graph& graph) {
    // Use the graph's find_shortest_path method
    current_path_ = graph.find_shortest_path(source_node_id_, destination_node_id_);
    // If no path is found, current_path_ will be empty, which is the desired behavior.
}

int Vehicle::get_id() const {
    return id_;
}

int Vehicle::get_source_node_id() const {
    return source_node_id_;
}

int Vehicle::get_destination_node_id() const {
    return destination_node_id_;
}

const std::vector<int>& Vehicle::get_current_path() const {
    return current_path_;
}
