#include "graph.hpp"

#include <algorithm> // For std::reverse
#include <iostream>  // For error reporting
#include <limits>    // For std::numeric_limits
#include <queue>     // For std::priority_queue
#include <vector>

// --- METHOD DEFINITIONS ---

Graph::Graph()
{
    // Constructor is empty, members are default-initialized.
}

bool Graph::add_node(int node_id, double x, double y)
{
    if (has_node(node_id))
    {
        return false; // Node already exists
    }
    nodes_[node_id] = {node_id, x, y};
    return true;
}

bool Graph::has_node(int node_id) const
{
    return nodes_.count(node_id) > 0;
}

const Node *Graph::get_node(int node_id) const
{
    auto it = nodes_.find(node_id);
    if (it != nodes_.end())
    {
        return &(it->second);
    }
    return nullptr;
}

bool Graph::add_edge(int edge_id, int from_node_id, int to_node_id, double weight)
{
    if (!has_node(from_node_id) || !has_node(to_node_id) || has_edge(edge_id))
    {
        return false;
    }
    Edge new_edge = {edge_id, from_node_id, to_node_id, weight};
    edges_[edge_id] = new_edge;
    adj_list_[from_node_id].push_back(new_edge);
    return true;
}

bool Graph::has_edge(int edge_id) const
{
    return edges_.count(edge_id) > 0;
}

bool Graph::has_edge_between(int from_node_id, int to_node_id) const
{
    if (adj_list_.count(from_node_id) == 0)
    {
        return false;
    }
    const auto &outgoing_edges = adj_list_.at(from_node_id);
    for (const auto &edge : outgoing_edges)
    {
        if (edge.to_node_id == to_node_id)
        {
            return true;
        }
    }
    return false;
}

const Edge *Graph::get_edge(int edge_id) const
{
    auto it = edges_.find(edge_id);
    if (it != edges_.end())
    {
        return &(it->second);
    }
    return nullptr;
}

const Edge *Graph::get_edge_between(int from_node_id, int to_node_id) const
{
    if (adj_list_.count(from_node_id) == 0)
    {
        return nullptr;
    }
    const auto &outgoing_edges = adj_list_.at(from_node_id);
    for (const auto &edge : outgoing_edges)
    {
        if (edge.to_node_id == to_node_id)
        {
            return get_edge(edge.id);
        }
    }
    return nullptr;
}

const std::map<int, Node> &Graph::get_all_nodes() const
{
    return nodes_;
}

const std::map<int, Edge> &Graph::get_all_edges() const
{
    return edges_;
}

const std::vector<Edge> &Graph::get_edges_from_node(int node_id) const
{
    static const std::vector<Edge> empty_vector;
    auto it = adj_list_.find(node_id);
    if (it != adj_list_.end())
    {
        return it->second;
    }
    return empty_vector;
}

std::vector<int> Graph::find_shortest_path(int start_node_id, int end_node_id) const
{
    if (!has_node(start_node_id) || !has_node(end_node_id))
        return {};
    if (start_node_id == end_node_id)
        return {start_node_id};

    std::map<int, double> dist;
    std::map<int, int> prev;
    using PathPair = std::pair<double, int>;
    std::priority_queue<PathPair, std::vector<PathPair>, std::greater<PathPair>> pq;

    for (const auto &pair : nodes_)
    {
        dist[pair.first] = std::numeric_limits<double>::infinity();
    }

    dist[start_node_id] = 0.0;
    pq.push({0.0, start_node_id});

    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();

        if (u == end_node_id)
            break;

        if (adj_list_.count(u))
        {
            for (const auto &edge : adj_list_.at(u))
            {
                int v = edge.to_node_id;
                double weight = edge.weight;
                if (dist.count(u) && dist[u] + weight < dist[v])
                {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
    }

    std::vector<int> path;
    if (dist.count(end_node_id) && dist[end_node_id] != std::numeric_limits<double>::infinity())
    {
        for (int at = end_node_id; at != 0; at = prev.count(at) ? prev.at(at) : 0)
        {
            path.push_back(at);
            if (at == start_node_id)
                break;
        }
        if (path.back() == start_node_id)
        {
            std::reverse(path.begin(), path.end());
        }
        else
        {
            return {}; // Path not found
        }
    }
    return path;
}

bool Graph::load_from_file(const std::string &filepath)
{
    std::cerr << "Warning: Graph::load_from_file(" << filepath << ") is not implemented." << std::endl;
    return false;
}

void Graph::clear()
{
    nodes_.clear();
    edges_.clear();
    adj_list_.clear();
}