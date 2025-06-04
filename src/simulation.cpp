#include "simulation.hpp"
#include <iostream>
#include <algorithm> // For std::remove_if, std::vector operations, std::shuffle
#include <vector>    // For std::vector to hold keys or IDs

// Constructor
Simulation::Simulation() :
    current_tick_(0),
    last_vehicle_id_(0),
    spawn_timer_(0),
    random_engine_(std::random_device{}()) // Seed the random engine
{
    // Graph, vehicles, intersections are default-initialized
}

void Simulation::set_graph(const Graph& graph) {
    graph_ = graph;
}

void Simulation::add_vehicle(const Vehicle& vehicle) {
    vehicles_.emplace(vehicle.get_id(), vehicle); // Using emplace as per previous fix
}

void Simulation::add_intersection(const Intersection& intersection) {
    intersections_.emplace(intersection.get_id(), intersection); // Using emplace
}

void Simulation::tick() {
    current_tick_++;

    // 1. Update intersection signals
    for (auto& pair : intersections_) {
        pair.second.update_signal_state();
    }

    // --- Vehicle Spawning ---
    spawn_timer_++;
    if (spawn_timer_ >= SPAWN_INTERVAL) {
        spawn_timer_ = 0;
        const auto& all_nodes_map = graph_.get_all_nodes();
        if (all_nodes_map.size() >= 2) {
            std::vector<int> node_ids;
            for (const auto& node_pair : all_nodes_map) {
                node_ids.push_back(node_pair.first);
            }

            std::shuffle(node_ids.begin(), node_ids.end(), random_engine_); // Shuffle to pick different src/dest

            int source_node = node_ids[0];
            int dest_node = node_ids[1];

            if (source_node != dest_node) { // Ensure source and destination are different
                Vehicle new_vehicle(++last_vehicle_id_, source_node, dest_node);
                new_vehicle.plan_route(graph_); // Plan the route first
                if (!new_vehicle.get_current_path().empty()) { // Only add if a path exists
                    // Vehicle is added in NOT_STARTED state.
                    // Simulation loop's NOT_STARTED case will call start_journey.
                    add_vehicle(new_vehicle);
                    // std::cout << "Tick " << current_tick_ << ": Spawned Vehicle " << new_vehicle.get_id()
                    //           << " (" << source_node << " -> " << dest_node << ")" << std::endl;
                }
            }
        }
    }

    // --- Vehicle Updates (Movement Logic) ---
    std::vector<int> arrived_vehicle_ids; // To collect IDs of vehicles that have arrived

    for (auto& vehicle_pair : vehicles_) {
        Vehicle& vehicle = vehicle_pair.second;

        switch (vehicle.get_state()) {
            case VehicleState::NOT_STARTED: {
                vehicle.start_journey(graph_);
                // If vehicle arrived immediately (source==dest), its state will be ARRIVED.
                // This will be caught in the check below.
                break;
            }
            case VehicleState::EN_ROUTE: {
                vehicle.increment_edge_progress_ticks();
                if (vehicle.get_current_edge_progress_ticks() >= vehicle.get_current_edge_total_ticks()) {
                    int new_current_node_id = vehicle.get_next_node_id();
                    vehicle.set_current_node_id(new_current_node_id);
                    vehicle.set_current_edge_ticks(0, 0);

                    if (new_current_node_id == vehicle.get_destination_node_id()) {
                        vehicle.set_state(VehicleState::ARRIVED);
                        vehicle.set_next_node_id(-1);
                    } else {
                        vehicle.set_state(VehicleState::WAITING_AT_INTERSECTION);
                        const auto& path = vehicle.get_current_path();
                        size_t current_path_index = (size_t)-1; // Initialize with an invalid index
                        for (size_t i = 0; i < path.size(); ++i) {
                            if (path[i] == new_current_node_id) { current_path_index = i; break; }
                        }
                        if (current_path_index != (size_t)-1 && current_path_index + 1 < path.size()) {
                            vehicle.set_next_node_id(path[current_path_index + 1]);
                            if (intersections_.count(new_current_node_id)) {
                                const Edge* outgoing_edge = graph_.get_edge_between(new_current_node_id, vehicle.get_next_node_id());
                                if (outgoing_edge) {
                                    intersections_.at(new_current_node_id).add_vehicle_to_queue(vehicle.get_id(), outgoing_edge->id);
                                } else {
                                    // std::cerr << "Tick " << current_tick_ << ": ERR! Vehicle " << vehicle.get_id() << " no edge in graph for path segment from " << new_current_node_id << " to " << vehicle.get_next_node_id() << std::endl;
                                    vehicle.set_state(VehicleState::ARRIVED); // Path error
                                }
                            } else {
                                // std::cerr << "Tick " << current_tick_ << ": ERR! Vehicle " << vehicle.get_id() << " target intersection " << new_current_node_id << " not in simulation." << std::endl;
                                vehicle.set_state(VehicleState::ARRIVED);
                            } // Intersection error
                        } else {
                            // std::cerr << "Tick " << current_tick_ << ": ERR! Vehicle " << vehicle.get_id() << " path ended prematurely or next node not found after " << new_current_node_id << std::endl;
                            vehicle.set_state(VehicleState::ARRIVED);
                        } // Path error
                    }
                }
                break;
            }
            case VehicleState::WAITING_AT_INTERSECTION: {
                int current_loc_node_id = vehicle.get_current_node_id();
                int next_target_node_id = vehicle.get_next_node_id();
                if (next_target_node_id == -1) { // Should only happen if path ended / error
                    // std::cerr << "Tick " << current_tick_ << ": ERR! Vehicle " << vehicle.get_id() << " WAITING but no next_node_id." << std::endl;
                    vehicle.set_state(VehicleState::ARRIVED);
                    break;
                }

                if (intersections_.count(current_loc_node_id)) {
                    Intersection& intersection = intersections_.at(current_loc_node_id);
                    const Edge* outgoing_edge = graph_.get_edge_between(current_loc_node_id, next_target_node_id);
                    if (outgoing_edge) {
                        int outgoing_edge_id = outgoing_edge->id;
                        bool can_proceed = false;
                        // Check if vehicle is at the front of the queue for this specific outgoing edge
                        if (!intersection.get_vehicle_queue(outgoing_edge_id).empty() &&
                            intersection.get_vehicle_queue(outgoing_edge_id).front() == vehicle.get_id()) {
                            can_proceed = true;
                        }

                        if (intersection.get_signal_state(outgoing_edge_id) == LightState::GREEN && can_proceed) {
                            intersection.pop_vehicle_from_queue(outgoing_edge_id);
                            vehicle.set_state(VehicleState::EN_ROUTE);
                            int travel_ticks = static_cast<int>(outgoing_edge->weight);
                            if (travel_ticks < 1) travel_ticks = 1; // Min 1 tick
                            vehicle.set_current_edge_ticks(0, travel_ticks);
                        }
                        // Else: still waiting (red/yellow light, or not at front of queue)
                    } else {
                        // std::cerr << "Tick " << current_tick_ << ": ERR! Vehicle " << vehicle.get_id() << " WAITING but no edge in graph from " << current_loc_node_id << " to " << next_target_node_id << std::endl;
                        vehicle.set_state(VehicleState::ARRIVED);
                    } // Edge error
                } else {
                    // std::cerr << "Tick " << current_tick_ << ": ERR! Vehicle " << vehicle.get_id() << " WAITING at non-existent intersection " << current_loc_node_id << std::endl;
                    vehicle.set_state(VehicleState::ARRIVED);
                } // Intersection error
                break;
            }
            case VehicleState::ARRIVED: {
                // No action needed here, will be collected for despawning below
                break;
            }
        } // End of switch

        // Collect ID if vehicle has arrived (either by reaching destination or due to an error)
        if (vehicle.get_state() == VehicleState::ARRIVED) {
            arrived_vehicle_ids.push_back(vehicle_pair.first);
        }
    } // End of vehicle update loop

    // --- Vehicle Despawning ---
    for (int vehicle_id : arrived_vehicle_ids) {
        vehicles_.erase(vehicle_id);
        // std::cout << "Tick " << current_tick_ << ": Despawned Vehicle " << vehicle_id << std::endl;
    }
}

// Accessor implementations (get_current_tick, get_graph, etc.) remain the same
int Simulation::get_current_tick() const { return current_tick_; }
const Graph& Simulation::get_graph() const { return graph_; }
const std::map<int, Vehicle>& Simulation::get_vehicles() const { return vehicles_; }
const std::map<int, Intersection>& Simulation::get_intersections() const { return intersections_; }
Vehicle* Simulation::get_vehicle_by_id(int vehicle_id) {
    auto it = vehicles_.find(vehicle_id);
    if (it != vehicles_.end()) { return &(it->second); }
    return nullptr;
}
Intersection* Simulation::get_intersection_by_id(int intersection_id) {
    auto it = intersections_.find(intersection_id);
    if (it != intersections_.end()) { return &(it->second); }
    return nullptr;
}
