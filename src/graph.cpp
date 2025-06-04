#include "graph.hpp" // Corrected include path
#include <algorithm> // For std::find_if
#include <queue>
#include <map>

Graph::Graph() {
    // Constructor can be empty for now
}

// Node operations
bool Graph::add_node(int node_id) {
    if (nodes_.find(node_id) != nodes_.end()) {
        return false; // Node already exists
    }
    nodes_[node_id] = {node_id};
    adj_list_[node_id] = {}; // Initialize adjacency list for this node
    return true;
}

bool Graph::has_node(int node_id) const {
    return nodes_.count(node_id) > 0;
}

const Node* Graph::get_node(int node_id) const {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        return &(it->second);
    }
    return nullptr;
}

// Edge operations
bool Graph::add_edge(int edge_id, int from_node_id, int to_node_id, double weight) {
    if (!has_node(from_node_id) || !has_node(to_node_id)) {
        return false; // One or both nodes do not exist
    }
    if (edges_.count(edge_id) > 0) {
        return false; // Edge with this ID already exists
    }
    if (has_edge_between(from_node_id, to_node_id)) {
        return false; // An edge already exists between these two nodes in this direction
    }

    Edge new_edge = {edge_id, from_node_id, to_node_id, weight};
    edges_[edge_id] = new_edge;
    adj_list_[from_node_id].push_back(new_edge);
    return true;
}

bool Graph::has_edge(int edge_id) const {
    return edges_.count(edge_id) > 0;
}

bool Graph::has_edge_between(int from_node_id, int to_node_id) const {
    if (!has_node(from_node_id)) {
        return false;
    }
    const auto& outgoing_edges = adj_list_.at(from_node_id);
    for (const auto& edge : outgoing_edges) {
        if (edge.to_node_id == to_node_id) {
            return true;
        }
    }
    return false;
}

const Edge* Graph::get_edge(int edge_id) const {
    auto it = edges_.find(edge_id);
    if (it != edges_.end()) {
        return &(it->second);
    }
    return nullptr;
}

const Edge* Graph::get_edge_between(int from_node_id, int to_node_id) const {
    if (!has_node(from_node_id)) {
        return nullptr;
    }
    const auto& outgoing_edges = adj_list_.at(from_node_id);
    for (const auto& edge : outgoing_edges) {
        if (edge.to_node_id == to_node_id) {
            // Need to return a pointer to the edge stored in edges_, not the copy in adj_list_
            return get_edge(edge.id);
        }
    }
    return nullptr;
}

const std::map<int, Node>& Graph::get_all_nodes() const {
    return nodes_;
}

const std::map<int, Edge>& Graph::get_all_edges() const {
    return edges_;
}

const std::vector<Edge>& Graph::get_edges_from_node(int node_id) const {
    // Ensure node exists to prevent std::out_of_range
    if (adj_list_.count(node_id)) {
        return adj_list_.at(node_id);
    }
    // Return an empty vector if node doesn't exist or has no outgoing edges
    static const std::vector<Edge> empty_edges; // static to ensure it's safe to return a reference
    return empty_edges;
}

// Pathfinding (placeholder implementation)
// std::vector<int> Graph::find_shortest_path(int start_node_id, int end_node_id) {
//     // To be implemented (e.g., Dijkstra's or A*)
//     return {};
// }

std::vector<int> Graph::find_shortest_path(int start_node_id, int end_node_id) const {
    if (nodes_.find(start_node_id) == nodes_.end() || nodes_.find(end_node_id) == nodes_.end()) {
        return {};
    }
    if (start_node_id == end_node_id) {
        return {start_node_id};
    }

    std::map<int, double> distances;
    std::map<int, int> predecessors;
    std::priority_queue<std::pair<double, int>,
                        std::vector<std::pair<double, int>>,
                        std::greater<std::pair<double, int>>> pq;

    for (const auto& node_pair : nodes_) {
        distances[node_pair.first] = std::numeric_limits<double>::infinity();
    }
    distances[start_node_id] = 0.0;

    pq.push({0.0, start_node_id});

    while (!pq.empty()) {
        double current_dist_to_u = pq.top().first;
        int u_node_id = pq.top().second;
        pq.pop();

        if (current_dist_to_u > distances.at(u_node_id)) {
            continue;
        }

        if (u_node_id == end_node_id) {
            break;
        }

        if (adj_list_.count(u_node_id)) {
            for (const Edge& edge : adj_list_.at(u_node_id)) {
                int v_node_id = edge.to_node_id;
                double edge_weight = edge.weight;

                if (distances.count(v_node_id)) {
                    if (distances.at(u_node_id) + edge_weight < distances.at(v_node_id)) {
                        distances.at(v_node_id) = distances.at(u_node_id) + edge_weight;
                        predecessors[v_node_id] = u_node_id;
                        pq.push({distances.at(v_node_id), v_node_id});
                    }
                }
            }
        }
    }

    std::vector<int> path;
    if (distances.at(end_node_id) == std::numeric_limits<double>::infinity()) {
        return {};
    }

    int current_node_for_path = end_node_id;
    while (current_node_for_path != start_node_id) {
        path.push_back(current_node_for_path);
        if (predecessors.find(current_node_for_path) == predecessors.end()) {
            return {};
        }
        current_node_for_path = predecessors.at(current_node_for_path);
    }
    path.push_back(start_node_id);
    std::reverse(path.begin(), path.end());

    return path;
}
