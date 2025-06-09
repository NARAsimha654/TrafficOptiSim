#include "visualizer.hpp" // Use "" for local project headers
#include <algorithm> // For std::max_element if needed, or other algorithms
#include <iomanip>   // For std::setw, std::left if formatting text output

namespace Visualization {

TextVisualizer::TextVisualizer() {
    // Constructor, if any setup is needed
}

void TextVisualizer::clear_console() const {
    // Simple platform-dependent way to clear console.
    // For Windows: system("cls");
    // For Linux/macOS: system("clear");
    // This is generally not recommended for production code but fine for a simple tool.
    // A more portable way would involve ANSI escape codes or platform-specific API calls.
    // For this example, we'll just print newlines to simulate clearing for basic output.
    // std::cout << std::string(50, '\n'); // Not a true clear
    // Or, if system calls are okay for this project:
    #if defined(_WIN32) || defined(_WIN64)
        // system("cls"); // Can be disruptive, let's avoid for now
    #else
        // system("clear"); // Can be disruptive, let's avoid for now
    #endif
    // For a text-based display that updates in place, one might use ANSI escape codes
    // to move the cursor to the top-left (e.g., "\033[H").
    // For now, we will just let output scroll.
}

void TextVisualizer::display_state(
    int current_tick,
    const Graph& graph,
    const std::map<int, Vehicle>& vehicles,
    const std::map<int, Intersection>& intersections) const {

    // clear_console(); // Call this if you want to attempt to clear per frame

    std::cout << "=================================================" << std::endl;
    std::cout << "            Traffic Simulation State             " << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << "Current Tick: " << current_tick << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    draw_intersections(intersections);
    std::cout << "-------------------------------------------------" << std::endl;
    draw_vehicles(vehicles, graph); // Pass graph for context if needed (e.g. to know edge lengths)
    std::cout << "-------------------------------------------------" << std::endl;
    // draw_graph(graph); // Drawing the full graph can be very verbose for text. Optional.
    // std::cout << "-------------------------------------------------" << std::endl;
    std::cout << std::endl; // Add some spacing
}

void TextVisualizer::draw_graph(const Graph& graph) const {
    std::cout << "Graph Structure (Nodes and Edges):" << std::endl;
    const auto& nodes = graph.get_all_nodes();
    const auto& edges = graph.get_all_edges();

    if (nodes.empty()) {
        std::cout << "  No nodes in the graph." << std::endl;
    } else {
        std::cout << "  Nodes (" << nodes.size() << "): ";
        for (const auto& node_pair : nodes) {
            std::cout << node_pair.first << " ";
        }
        std::cout << std::endl;
    }

    if (edges.empty()) {
        std::cout << "  No edges in the graph." << std::endl;
    } else {
        std::cout << "  Edges (" << edges.size() << "):" << std::endl;
        for (const auto& edge_pair : edges) {
            const Edge& e = edge_pair.second;
            std::cout << "    ID " << e.id << ": " << e.from_node_id << " -> " << e.to_node_id << " (Weight: " << e.weight << ")" << std::endl;
        }
    }
}

void TextVisualizer::draw_vehicles(const std::map<int, Vehicle>& vehicles, const Graph& graph) const {
    std::cout << "Active Vehicles (" << vehicles.size() << "):" << std::endl;
    if (vehicles.empty()) {
        std::cout << "  No active vehicles." << std::endl;
        return;
    }

    // Table headers
    std::cout << std::left // Left-align text in columns
              << std::setw(8) << "Veh ID"
              << std::setw(18) << "State"
              << std::setw(12) << "CurrNode"
              << std::setw(12) << "NextNode"
              << std::setw(15) << "EdgeProg"
              << std::setw(10) << "DestNode"
              << std::endl;
    std::cout << std::string(75, '-') << std::endl; // Separator line

    for (const auto& pair : vehicles) {
        const Vehicle& v = pair.second;
        std::string progress_str = std::to_string(v.get_current_edge_progress_ticks()) + "/" + std::to_string(v.get_current_edge_total_ticks());

        std::cout << std::left
                  << std::setw(8) << v.get_id()
                  << std::setw(18) << vehicle_state_to_string(v.get_state())
                  << std::setw(12) << (v.get_current_node_id() == -1 ? "N/A" : std::to_string(v.get_current_node_id()))
                  << std::setw(12) << (v.get_next_node_id() == -1 ? "N/A" : std::to_string(v.get_next_node_id()))
                  << std::setw(15) << (v.get_state() == VehicleState::EN_ROUTE ? progress_str : "-")
                  << std::setw(10) << v.get_destination_node_id()
                  << std::endl;
    }
}

void TextVisualizer::draw_intersections(const std::map<int, Intersection>& intersections) const {
    std::cout << "Intersection States (" << intersections.size() << "):" << std::endl;
    if (intersections.empty()) {
        std::cout << "  No intersections defined." << std::endl;
        return;
    }

    for (const auto& pair : intersections) {
        const Intersection& intersection = pair.second;
        std::cout << "  Intersection ID: " << intersection.get_id() << std::endl;
        const auto& approach_ids = intersection.get_approach_ids(); // These are outgoing edge IDs
        if (approach_ids.empty()) {
            std::cout << "    No approaches (outgoing edges) defined for this intersection." << std::endl;
        } else {
            for (int edge_id : approach_ids) {
                std::cout << "    - Outgoing Edge ID " << std::setw(5) << edge_id << ": "
                          << "Signal: " << std::setw(6) << std::left << light_state_to_string(intersection.get_signal_state(edge_id))
                          << " Queue Size: " << intersection.get_vehicle_queue(edge_id).size()
                          << std::endl;
            }
        }
    }
}

} // namespace Visualization
