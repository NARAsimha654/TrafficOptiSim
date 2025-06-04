#include "vehicle.hpp"
#include <stdexcept> // For potential errors if path is misused

std::string vehicle_state_to_string(VehicleState state) {
    switch (state) {
        case VehicleState::NOT_STARTED: return "NOT_STARTED";
        case VehicleState::EN_ROUTE: return "EN_ROUTE";
        case VehicleState::WAITING_AT_INTERSECTION: return "WAITING_AT_INTERSECTION";
        case VehicleState::ARRIVED: return "ARRIVED";
        default: return "UNKNOWN_STATE";
    }
}

Vehicle::Vehicle(int id, int source_node_id, int destination_node_id)
    : id_(id),
      source_node_id_(source_node_id),
      destination_node_id_(destination_node_id),
      current_path_(),
      state_(VehicleState::NOT_STARTED),
      current_node_id_(source_node_id_), // Initially at source
      next_node_id_(-1), // Unknown until path is planned and journey started
      current_edge_progress_ticks_(0),
      current_edge_total_ticks_(0) {
}

void Vehicle::plan_route(const Graph& graph) {
    current_path_ = graph.find_shortest_path(source_node_id_, destination_node_id_);
    // After planning, call start_journey to set initial movement vars
}

void Vehicle::start_journey(const Graph& graph) {
    current_edge_progress_ticks_ = 0;
    current_edge_total_ticks_ = 0;

    if (source_node_id_ == destination_node_id_) {
        state_ = VehicleState::ARRIVED;
        current_node_id_ = destination_node_id_;
        next_node_id_ = -1; // No next node
        current_path_.clear(); // Path is trivial
        if (!current_path_.empty() && current_path_.front() == source_node_id_) {
            // if path was planned and only contains source, that's fine
        } else {
             current_path_ = {source_node_id_}; // Path is just the node itself
        }
        return;
    }

    if (!current_path_.empty()) {
        // Ensure path starts with the source node if it's not empty
        // This could happen if plan_route was called, then source_node_id_ was changed, then start_journey was called.
        // Or if path planning itself had an issue.
        // For robustness, we could re-align or error. Here, we assume current_path_[0] is the true start if path exists.
        current_node_id_ = current_path_.front();

        if (current_path_.size() > 1) {
            next_node_id_ = current_path_[1];
            const Edge* edge = graph.get_edge_between(current_node_id_, next_node_id_);
            if (edge) {
                // Using edge weight directly as ticks. Could be scaled or calculated differently.
                current_edge_total_ticks_ = static_cast<int>(edge->weight);
                if (current_edge_total_ticks_ < 1) current_edge_total_ticks_ = 1; // Minimum 1 tick per edge
                state_ = VehicleState::EN_ROUTE;
            } else {
                // Path contains an edge not in graph - problem!
                state_ = VehicleState::NOT_STARTED; // Or an error state
                next_node_id_ = -1;
                // This case should ideally not happen if graph & path are consistent.
            }
        } else { // Path has only one node.
            if (current_node_id_ == destination_node_id_) { // This should be true if path has 1 node and source==dest
                 state_ = VehicleState::ARRIVED;
            } else {
                // Path is just source, but source is not destination -> error or stuck
                // This implies no path was found by plan_route but current_path_ was not empty (e.g. {source_id})
                // which is not typical for how find_shortest_path usually returns empty on failure.
                // However, if find_shortest_path could return a path of {source} if source==dest, this is covered.
                // If source != dest and path has 1 node, it's an invalid path to start a journey on.
                state_ = VehicleState::NOT_STARTED;
            }
            next_node_id_ = -1;
        }
    } else { // Path is empty and source != destination (already handled source == dest)
        state_ = VehicleState::NOT_STARTED; // Cannot start, no path
        current_node_id_ = source_node_id_; // Ensure current_node is at least the source
        next_node_id_ = -1;
    }
}


// Accessors
int Vehicle::get_id() const { return id_; }
int Vehicle::get_source_node_id() const { return source_node_id_; }
int Vehicle::get_destination_node_id() const { return destination_node_id_; }
const std::vector<int>& Vehicle::get_current_path() const { return current_path_; }
VehicleState Vehicle::get_state() const { return state_; }
int Vehicle::get_current_node_id() const { return current_node_id_; }
int Vehicle::get_next_node_id() const { return next_node_id_; }
int Vehicle::get_current_edge_progress_ticks() const { return current_edge_progress_ticks_; }
int Vehicle::get_current_edge_total_ticks() const { return current_edge_total_ticks_; }

// Mutators
void Vehicle::set_state(VehicleState new_state) { state_ = new_state; }
void Vehicle::set_current_node_id(int node_id) { current_node_id_ = node_id; }
void Vehicle::set_next_node_id(int node_id) { next_node_id_ = node_id; }
void Vehicle::set_current_edge_ticks(int progress, int total) {
    current_edge_progress_ticks_ = progress;
    current_edge_total_ticks_ = total;
    if (current_edge_total_ticks_ < 1 && state_ == VehicleState::EN_ROUTE) { // Ensure positive travel time if en_route
        current_edge_total_ticks_ = 1;
    }
}
void Vehicle::increment_edge_progress_ticks() {
    if (state_ == VehicleState::EN_ROUTE) {
        current_edge_progress_ticks_++;
    }
}
