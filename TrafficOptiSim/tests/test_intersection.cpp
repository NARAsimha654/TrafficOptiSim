#include <iostream>
#include <vector>
#include <string>
#include <stdexcept> // For std::out_of_range
#include <cassert>
#include "intersection.hpp" // The class we are testing

// Helper to print intersection state (optional, for debugging)
void print_intersection_state(const Intersection& intersection) {
    std::cout << "  Intersection ID: " << intersection.get_id() << std::endl;
    const auto& approaches = intersection.get_approach_ids();
    if (approaches.empty()) {
        std::cout << "    No approaches defined." << std::endl;
        return;
    }
    for (int approach_id : approaches) {
        std::cout << "    Approach " << approach_id << ": Signal = "
                  << light_state_to_string(intersection.get_signal_state(approach_id))
                  << ", Queue size = " << intersection.get_vehicle_queue(approach_id).size()
                  << std::endl;
    }
}


void test_intersection_creation_and_initial_state() {
    std::cout << "Running test_intersection_creation_and_initial_state..." << std::endl;

    std::vector<int> approaches = {10, 20, 30}; // e.g., North, East, South approaches
    Intersection intersection(1, approaches);

    assert(intersection.get_id() == 1);
    assert(intersection.get_approach_ids().size() == 3);
    assert(intersection.get_approach_ids() == approaches);

    // Initial signal states should be RED for all configured approaches before first update
    for (int approach_id : approaches) {
        assert(intersection.get_signal_state(approach_id) == LightState::RED);
        assert(intersection.get_vehicle_queue(approach_id).empty());
    }

    // Test default constructor and initialize
    Intersection intersection_default;
    assert(intersection_default.get_id() == -1); // Default ID
    intersection_default.initialize(2, {100, 200});
    assert(intersection_default.get_id() == 2);
    assert(intersection_default.get_approach_ids().size() == 2);
    assert(intersection_default.get_signal_state(100) == LightState::RED);
    assert(intersection_default.get_signal_state(200) == LightState::RED);


    // Test querying non-existent approach
    bool thrown = false;
    try {
        intersection.get_signal_state(99); // 99 is not a configured approach
    } catch (const std::out_of_range& e) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        intersection.get_vehicle_queue(99);
    } catch (const std::out_of_range& e) {
        thrown = true;
    }
    assert(thrown);

    std::cout << "test_intersection_creation_and_initial_state PASSED." << std::endl;
}

void test_vehicle_queuing() {
    std::cout << "Running test_vehicle_queuing..." << std::endl;
    std::vector<int> approaches = {10, 20};
    Intersection intersection(1, approaches);

    intersection.add_vehicle_to_queue(101, 10); // Vehicle 101 to approach 10
    intersection.add_vehicle_to_queue(102, 10); // Vehicle 102 to approach 10
    intersection.add_vehicle_to_queue(201, 20); // Vehicle 201 to approach 20

    assert(intersection.get_vehicle_queue(10).size() == 2);
    assert(intersection.get_vehicle_queue(20).size() == 1);

    // Check order if we could dequeue (not implemented in Intersection directly yet)
    // For now, just size is enough. If we had a pop_vehicle_from_queue:
    // assert(intersection.pop_vehicle_from_queue(10) == 101);
    // assert(intersection.get_vehicle_queue(10).size() == 1);

    // Adding to a non-existent approach (should ideally be handled, current impl might silently fail or error if map access is strict)
    // Current Intersection::add_vehicle_to_queue uses .count, so it should silently not add.
    intersection.add_vehicle_to_queue(999, 99); // Approach 99 doesn't exist
    bool thrown = false;
    try {
         intersection.get_vehicle_queue(99); // This will throw
    } catch (const std::out_of_range& e) {
        thrown = true;
    }
    assert(thrown); // Confirms approach 99 was not implicitly created.


    std::cout << "test_vehicle_queuing PASSED." << std::endl;
}

void test_signal_cycling() {
    std::cout << "Running test_signal_cycling..." << std::endl;
    std::vector<int> approaches = {10, 20}; // Approach 10, Approach 20
    Intersection intersection(1, approaches);

    // Constants from Intersection class (consider making them accessible for tests or use known values)
    const int green_duration = 15;
    const int yellow_duration = 3;

    // Initial state: all RED (as asserted in creation test)
    // print_intersection_state(intersection);

    // --- Tick 1: First update_signal_state call ---
    intersection.update_signal_state(); // Approach 10 should become GREEN
    // std::cout << "After 1st update_signal_state:" << std::endl;
    // print_intersection_state(intersection);
    assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN);
    assert(intersection.get_signal_state(approaches[1]) == LightState::RED);

    // --- Tick through GREEN phase for approach 10 ---
    for (int i = 0; i < green_duration - 1; ++i) { // -1 because one tick already passed making it green
        intersection.update_signal_state();
        assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN);
        assert(intersection.get_signal_state(approaches[1]) == LightState::RED);
    }

    // --- Tick to transition approach 10 to YELLOW ---
    intersection.update_signal_state(); // Should transition from G to Y
    // std::cout << "After G->Y transition for approach " << approaches[0] << ":" << std::endl;
    // print_intersection_state(intersection);
    assert(intersection.get_signal_state(approaches[0]) == LightState::YELLOW);
    assert(intersection.get_signal_state(approaches[1]) == LightState::RED);

    // --- Tick through YELLOW phase for approach 10 ---
    for (int i = 0; i < yellow_duration - 1; ++i) { // -1 because one tick already passed making it yellow
        intersection.update_signal_state();
        assert(intersection.get_signal_state(approaches[0]) == LightState::YELLOW);
        assert(intersection.get_signal_state(approaches[1]) == LightState::RED);
    }

    // --- Tick to transition approach 10 to RED, and approach 20 to GREEN ---
    intersection.update_signal_state(); // Should transition from Y to R, and next (approaches[1]) to G
    // std::cout << "After Y->R for " << approaches[0] << " and next G for " << approaches[1] << ":" << std::endl;
    // print_intersection_state(intersection);
    assert(intersection.get_signal_state(approaches[0]) == LightState::RED);
    assert(intersection.get_signal_state(approaches[1]) == LightState::GREEN);

    // --- Tick through GREEN phase for approach 20 ---
    for (int i = 0; i < green_duration - 1; ++i) {
        intersection.update_signal_state();
        assert(intersection.get_signal_state(approaches[0]) == LightState::RED);
        assert(intersection.get_signal_state(approaches[1]) == LightState::GREEN);
    }

    // --- Tick to transition approach 20 to YELLOW ---
    intersection.update_signal_state();
    assert(intersection.get_signal_state(approaches[0]) == LightState::RED);
    assert(intersection.get_signal_state(approaches[1]) == LightState::YELLOW);

    // --- Tick through YELLOW phase for approach 20 ---
    for (int i = 0; i < yellow_duration - 1; ++i) {
        intersection.update_signal_state();
        assert(intersection.get_signal_state(approaches[0]) == LightState::RED);
        assert(intersection.get_signal_state(approaches[1]) == LightState::YELLOW);
    }

    // --- Tick to transition approach 20 to RED, and approach 10 (cycle back) to GREEN ---
    intersection.update_signal_state();
    assert(intersection.get_signal_state(approaches[0]) == LightState::GREEN);
    assert(intersection.get_signal_state(approaches[1]) == LightState::RED);

    std::cout << "test_signal_cycling PASSED." << std::endl;
}

void test_intersection_no_approaches() {
    std::cout << "Running test_intersection_no_approaches..." << std::endl;
    Intersection intersection(1, {}); // No approaches
    assert(intersection.get_id() == 1);
    assert(intersection.get_approach_ids().empty());

    intersection.update_signal_state(); // Should do nothing, not crash

    bool thrown = false;
    try {
        intersection.get_signal_state(0); // No approaches to query
    } catch (const std::out_of_range& e) {
        thrown = true;
    }
    assert(thrown);

    std::cout << "test_intersection_no_approaches PASSED." << std::endl;
}


int main() {
    std::cout << "Starting Intersection tests (test_intersection.cpp)..." << std::endl;
    test_intersection_creation_and_initial_state();
    test_vehicle_queuing();
    test_signal_cycling();
    test_intersection_no_approaches();
    std::cout << "All Intersection tests PASSED." << std::endl;
    return 0;
}
