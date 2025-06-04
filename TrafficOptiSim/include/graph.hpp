#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <map> // For adjacency list

// Represents a node in the graph (e.g., an intersection)
struct Node {
    int id;
    // Potentially other properties like coordinates for visualization
};

// Represents an edge in the graph (e.g., a road)
struct Edge {
    int id;
    int from_node_id;
    int to_node_id;
    double weight; // e.g., distance, travel time, traffic delay
    // Potentially other properties like road type, speed limit
};

class Graph {
public:
    Graph(); // Constructor

    // Node operations
    bool add_node(int node_id);
    bool has_node(int node_id) const;
    const Node* get_node(int node_id) const; // Returns nullptr if not found

    // Edge operations
    // Assumes nodes from_node_id and to_node_id already exist
    bool add_edge(int edge_id, int from_node_id, int to_node_id, double weight);
    bool has_edge(int edge_id) const;
    bool has_edge_between(int from_node_id, int to_node_id) const;
    const Edge* get_edge(int edge_id) const; // Returns nullptr if not found
    const Edge* get_edge_between(int from_node_id, int to_node_id) const; // Returns nullptr if not found

    // Get all nodes and edges
    const std::map<int, Node>& get_all_nodes() const;
    const std::map<int, Edge>& get_all_edges() const;
    const std::vector<Edge>& get_edges_from_node(int node_id) const;


    // Pathfinding (placeholder for now)
    // std::vector<int> find_shortest_path(int start_node_id, int end_node_id);

private:
    std::map<int, Node> nodes_; // Map node_id to Node struct
    std::map<int, Edge> edges_; // Map edge_id to Edge struct
    // Adjacency list for efficient lookup of outgoing edges from a node
    // Maps a node_id to a vector of Edge structs that start from this node
    std::map<int, std::vector<Edge>> adj_list_;
};

#endif // GRAPH_HPP
