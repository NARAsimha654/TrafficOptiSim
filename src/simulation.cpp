#include "simulation.hpp"
#include <iostream> // For initial tick printout

Simulation::Simulation() : current_tick_(0) {
    // Graph, vehicles, intersections are default-initialized (e.g. empty maps)
}

void Simulation::set_graph(const Graph& graph) {
    graph_ = graph;
    // Potentially, validate that intersection IDs match nodes in the graph if intersections exist
}

void Simulation::add_vehicle(const Vehicle& vehicle) {
    vehicles_.emplace(vehicle.get_id(), vehicle);
}

void Simulation::add_intersection(const Intersection& intersection) {
    intersections_.emplace(intersection.get_id(), intersection);
    // Here, we might want to ensure intersection.get_id() is a valid node in graph_
    // if graph_ is already set. For now, keeping it simple.
}

void Simulation::tick() {
    current_tick_++;
    // std::cout << "Simulation Tick: " << current_tick_ << std::endl; // Optional: for basic main.cpp output

    // Future:
    // 1. Update all intersection signals
    for (auto& pair : intersections_) {
        pair.second.update_signal_state();
    }

    // 2. Update all vehicle positions/states
    //    - For each vehicle:
    //        - Check current intersection's signal if at an intersection
    //        - If green or no obstacle, move along its path
    //        - If path is empty or completed, remove vehicle or mark as arrived
    //        - If waiting at a red light, add to intersection's queue (or already there)
    // This logic will be complex and added incrementally.
    // For now, only intersection signals are updated.
}

int Simulation::get_current_tick() const {
    return current_tick_;
}

const Graph& Simulation::get_graph() const {
    return graph_;
}

const std::map<int, Vehicle>& Simulation::get_vehicles() const {
    return vehicles_;
}

const std::map<int, Intersection>& Simulation::get_intersections() const {
    return intersections_;
}

Vehicle* Simulation::get_vehicle_by_id(int vehicle_id) {
    auto it = vehicles_.find(vehicle_id);
    if (it != vehicles_.end()) {
        return &(it->second);
    }
    return nullptr;
}

Intersection* Simulation::get_intersection_by_id(int intersection_id) {
    auto it = intersections_.find(intersection_id);
    if (it != intersections_.end()) {
        return &(it->second);
    }
    return nullptr;
}
