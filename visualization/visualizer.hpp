#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map> // Required for std::map

// Forward declare Simulation class to avoid circular dependency if Visualizer needs full Simulation details
// However, it's often better to pass only necessary data (Graph, Vehicles, Intersections)
// class Simulation;

// For this version, we'll include the necessary components directly
#include "graph.hpp"
#include "vehicle.hpp"
#include "intersection.hpp"

namespace Visualization {

// Simple text-based visualizer for the traffic simulation
class TextVisualizer {
public:
    TextVisualizer();

    // Clears the console (simple platform-dependent way)
    void clear_console() const;

    // Displays the current state of the simulation
    // Takes const references to the simulation components
    void display_state(
        int current_tick,
        const Graph& graph,
        const std::map<int, Vehicle>& vehicles,
        const std::map<int, Intersection>& intersections
    ) const;

private:
    // Helper method to draw the graph (e.g., as an adjacency list or simple map)
    void draw_graph(const Graph& graph) const;

    // Helper method to draw vehicles on the graph or list their states
    void draw_vehicles(const std::map<int, Vehicle>& vehicles, const Graph& graph) const;

    // Helper method to draw intersection states (signals, queues)
    void draw_intersections(const std::map<int, Intersection>& intersections) const;
};

} // namespace Visualization

#endif // VISUALIZER_HPP
