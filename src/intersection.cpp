#include "intersection.hpp"
#include <stdexcept> // For std::out_of_range

// Helper function implementation
std::string light_state_to_string(LightState state) {
    switch (state) {
        case LightState::RED: return "RED";
        case LightState::GREEN: return "GREEN";
        case LightState::YELLOW: return "YELLOW";
        default: return "UNKNOWN";
    }
}

// Default constructor
Intersection::Intersection()
    : id_(-1), current_green_approach_index_(-1), ticks_in_current_state_(0), phase_state_(LightState::RED) {}

// Constructor with ID and approach_ids
Intersection::Intersection(int id, const std::vector<int>& approach_ids)
    : id_(id),
      approach_ids_(approach_ids),
      current_green_approach_index_(-1), // No green initially
      ticks_in_current_state_(0),
      phase_state_(LightState::RED) { // Start with all red or first one green after first update
    if (!approach_ids_.empty()) {
        current_green_approach_index_ = 0; // Default to first approach if any
        // Initialize all signals to RED initially, except possibly the first one after an update
        for (int approach_id : approach_ids_) {
            current_signals_[approach_id] = LightState::RED;
            vehicle_queues_[approach_id] = std::queue<int>(); // Initialize empty queue
        }
        // Let the first call to update_signal_state set the initial green
    }
}

void Intersection::initialize(int id, const std::vector<int>& approach_ids) {
    id_ = id;
    approach_ids_ = approach_ids;
    current_green_approach_index_ = -1;
    ticks_in_current_state_ = 0;
    phase_state_ = LightState::RED;

    if (!approach_ids_.empty()) {
        current_green_approach_index_ = 0;
        for (int approach_id : approach_ids_) {
            current_signals_[approach_id] = LightState::RED;
            vehicle_queues_[approach_id] = std::queue<int>();
        }
    }
}


void Intersection::add_vehicle_to_queue(int vehicle_id, int approach_id) {
    if (vehicle_queues_.count(approach_id)) {
        vehicle_queues_[approach_id].push(vehicle_id);
    } else {
        // Optionally handle error: unknown approach_id
        // For now, assume approach_id is valid and was set up in constructor
    }
}

void Intersection::update_signal_state() {
    if (approach_ids_.empty()) return; // No approaches to manage

    ticks_in_current_state_++;

    int current_green_id = approach_ids_[current_green_approach_index_];

    if (phase_state_ == LightState::GREEN) {
        if (ticks_in_current_state_ >= GREEN_DURATION) {
            // Time to switch from GREEN to YELLOW
            current_signals_[current_green_id] = LightState::YELLOW;
            phase_state_ = LightState::YELLOW;
            ticks_in_current_state_ = 0;
        }
        // Else, stay GREEN
    } else if (phase_state_ == LightState::YELLOW) {
        if (ticks_in_current_state_ >= YELLOW_DURATION) {
            // Time to switch from YELLOW to RED, and pick next GREEN
            current_signals_[current_green_id] = LightState::RED; // Old green becomes red

            current_green_approach_index_ = (current_green_approach_index_ + 1) % approach_ids_.size();
            int next_green_id = approach_ids_[current_green_approach_index_];

            current_signals_[next_green_id] = LightState::GREEN;
            phase_state_ = LightState::GREEN;
            ticks_in_current_state_ = 0;

            // Set all other signals to RED
            for (int approach_id : approach_ids_) {
                if (approach_id != next_green_id) {
                    current_signals_[approach_id] = LightState::RED;
                }
            }
        }
        // Else, stay YELLOW
    } else { // phase_state_ == LightState::RED (initial state or after all-red phase)
        // This case handles the very first call or if we want an all-red startup.
        // Transition to the first designated green light.
        current_signals_[current_green_id] = LightState::GREEN;
        phase_state_ = LightState::GREEN;
        ticks_in_current_state_ = 0;
        // Ensure others are RED
        for (int approach_id : approach_ids_) {
            if (approach_id != current_green_id) {
                current_signals_[approach_id] = LightState::RED;
            }
        }
    }
}


LightState Intersection::get_signal_state(int approach_id) const {
    if (current_signals_.count(approach_id)) {
        return current_signals_.at(approach_id);
    }
    // Return RED by default if approach_id is unknown, or throw error
    // For safety, let's assume RED for unlisted approaches if they could be queried.
    // However, valid approaches should be in approach_ids_ and thus in current_signals_.
    throw std::out_of_range("Queried signal state for unknown approach_id: " + std::to_string(approach_id));
}

int Intersection::get_id() const {
    return id_;
}

const std::queue<int>& Intersection::get_vehicle_queue(int approach_id) const {
    if (vehicle_queues_.count(approach_id)) {
        return vehicle_queues_.at(approach_id);
    }
    throw std::out_of_range("Queried queue for unknown approach_id: " + std::to_string(approach_id));
}

const std::vector<int>& Intersection::get_approach_ids() const {
    return approach_ids_;
}
