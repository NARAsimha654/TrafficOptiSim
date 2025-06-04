#include <iostream>
#include <vector>
#include <cassert> // For basic assertions
#include "graph.hpp" // Adjust path as necessary based on include paths used during compilation

void test_add_node() {
    std::cout << "Running test_add_node..." << std::endl;
    Graph g;
    assert(g.add_node(1));
    assert(g.has_node(1));
    assert(g.get_all_nodes().size() == 1);
    assert(!g.add_node(1)); // Cannot add duplicate node
    assert(g.add_node(2));
    assert(g.has_node(2));
    assert(g.get_all_nodes().size() == 2);
    const Node* node1 = g.get_node(1);
    assert(node1 != nullptr);
    assert(node1->id == 1);
    assert(g.get_node(3) == nullptr); // Node not present
    std::cout << "test_add_node PASSED." << std::endl;
}

void test_add_edge() {
    std::cout << "Running test_add_edge..." << std::endl;
    Graph g;
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);

    // Test adding a valid edge
    assert(g.add_edge(101, 1, 2, 5.0));
    assert(g.has_edge(101));
    assert(g.has_edge_between(1, 2));
    assert(g.get_all_edges().size() == 1);
    const Edge* edge101 = g.get_edge(101);
    assert(edge101 != nullptr);
    assert(edge101->id == 101);
    assert(edge101->from_node_id == 1);
    assert(edge101->to_node_id == 2);
    assert(edge101->weight == 5.0);

    // Test getting edge between nodes
    const Edge* edge_between_1_2 = g.get_edge_between(1,2);
    assert(edge_between_1_2 != nullptr);
    assert(edge_between_1_2->id == 101);


    // Test adding another valid edge
    assert(g.add_edge(102, 2, 3, 2.5));
    assert(g.has_edge(102));
    assert(g.has_edge_between(2, 3));
    assert(g.get_all_edges().size() == 2);

    // Test duplicate edge ID
    assert(!g.add_edge(101, 1, 3, 1.0));

    // Test duplicate edge between same nodes in same direction
    assert(!g.add_edge(103, 1, 2, 3.0));


    // Test edge to non-existent node
    assert(!g.add_edge(104, 1, 4, 1.0)); // Node 4 does not exist

    // Test edge from non-existent node
    assert(!g.add_edge(105, 5, 2, 1.0)); // Node 5 does not exist

    // Test get_edges_from_node
    const auto& edges_from_1 = g.get_edges_from_node(1);
    assert(edges_from_1.size() == 1);
    assert(edges_from_1[0].id == 101);

    const auto& edges_from_2 = g.get_edges_from_node(2);
    assert(edges_from_2.size() == 1);
    assert(edges_from_2[0].id == 102);

    const auto& edges_from_3 = g.get_edges_from_node(3);
    assert(edges_from_3.empty());

    // Test get_edges_from_node for a non-existent node
    const auto& edges_from_99 = g.get_edges_from_node(99);
    assert(edges_from_99.empty());


    std::cout << "test_add_edge PASSED." << std::endl;
}

void test_get_non_existent() {
    std::cout << "Running test_get_non_existent..." << std::endl;
    Graph g;
    g.add_node(1);
    g.add_edge(101,1,1,1.0); // Edge from node 1 to node 1

    assert(g.get_node(99) == nullptr);
    assert(g.get_edge(999) == nullptr);
    assert(g.get_edge_between(1,99) == nullptr); // To node does not exist
    assert(g.get_edge_between(99,1) == nullptr); // From node does not exist

    std::cout << "test_get_non_existent PASSED." << std::endl;
}


int main() {
    std::cout << "Starting Graph tests..." << std::endl;
    test_add_node();
    test_add_edge();
    test_get_non_existent();
    std::cout << "All Graph tests PASSED." << std::endl;
    return 0;
}
