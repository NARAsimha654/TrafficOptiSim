#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <map>
#include <vector>
#include <queue>  // For std::queue
#include <string> // For approach names/IDs if needed, or just use int

// Define traffic light states
enum class LightState
{
    RED,
    GREEN,
    YELLOW
};

// Converts LightState to string for debugging
std::string light_state_to_string(LightState state);

class Intersection
{
public:
    // --- PUBLIC CONSTANTS (FIXED) ---
    // Moved from private to public to allow access from test files
    static const int GREEN_DURATION = 15; // ticks
    static const int YELLOW_DURATION = 3; // ticks

    // --- CONSTRUCTORS & INITIALIZERS ---
    // Constructor that takes the intersection ID and a list of approach identifiers
    Intersection(int id, const std::vector<int> &approach_ids);
    Intersection(); // Default constructor for map compatibility if needed, then init separately
    void initialize(int id, const std::vector<int> &approach_ids);

    // --- PUBLIC METHODS ---
    // Adds a vehicle (by ID) to the queue of a specific approach
    void add_vehicle_to_queue(int vehicle_id, int approach_id);

    // Updates the traffic signal states based on a simple cycle or logic
    void update_signal_state();

    // Gets the current signal state for a given approach
    LightState get_signal_state(int approach_id) const;

    // Gets the ID of this intersection
    int get_id() const;

    // Gets the current queue for a given approach (const version)
    const std::queue<int> &get_vehicle_queue(int approach_id) const;

    // Gets the list of approach IDs for this intersection
    const std::vector<int> &get_approach_ids() const;

    // Returns vehicle_id or -1 if empty
    int pop_vehicle_from_queue(int approach_id);

private:
    int id_;
    std::map<int, LightState> current_signals_;     // Key: approach_id
    std::map<int, std::queue<int>> vehicle_queues_; // Key: approach_id, Value: queue of vehicle_ids

    std::vector<int> approach_ids_;    // Stores the approaches relevant to this intersection
    int current_green_approach_index_; // Index in approach_ids_ that is currently green (or was last green)
    int ticks_in_current_state_;       // Counter for how long current signal phase has been active

    // Red duration is implied by other phases

    // Internal state for signal cycling
    LightState phase_state_; // Is the current phase GREEN or YELLOW
};

#endif // INTERSECTION_HPP