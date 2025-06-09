#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <map>
#include <string>

// Represents a node in the graph (e.g., an intersection)
// Contains x and y coordinates for visualization.
struct Node
{
    int id;
    double x;
    double y;
};

// Represents an edge in the graph (e.g., a road)
struct Edge
{
    int id;
    int from_node_id;
    int to_node_id;
    double weight; // e.g., distance, travel time
};

// --- CLASS DECLARATION ---
class Graph
{
public:
    // Constructor
    Graph();

    // Node operations
    bool add_node(int node_id, double x, double y);
    bool has_node(int node_id) const;
    const Node *get_node(int node_id) const;

    // Edge operations
    bool add_edge(int edge_id, int from_node_id, int to_node_id, double weight);
    bool has_edge(int edge_id) const;
    bool has_edge_between(int from_node_id, int to_node_id) const;
    const Edge *get_edge(int edge_id) const;
    const Edge *get_edge_between(int from_node_id, int to_node_id) const;

    // Accessors
    const std::map<int, Node> &get_all_nodes() const;
    const std::map<int, Edge> &get_all_edges() const;
    const std::vector<Edge> &get_edges_from_node(int node_id) const;

    // Algorithms
    std::vector<int> find_shortest_path(int start_node_id, int end_node_id) const;

    // Utility
    bool load_from_file(const std::string &filepath);
    void clear();

private:
    std::map<int, Node> nodes_;
    std::map<int, Edge> edges_;
    std::map<int, std::vector<Edge>> adj_list_;
};

#endif // GRAPH_HPP