#include <iostream>
#include <vector>
#include <string> // For std::string in print_path
#include <cassert>
#include <algorithm>
#include "graph.hpp"

// Original test: test_add_node
void test_add_node()
{
    std::cout << "Running test_add_node..." << std::endl;
    Graph g;
    // FIXED: Added dummy coordinates (0.0, 0.0) to all add_node calls
    assert(g.add_node(1, 0.0, 0.0));
    assert(g.has_node(1));
    assert(g.get_all_nodes().size() == 1);
    assert(!g.add_node(1, 0.0, 0.0)); // Cannot add duplicate node
    assert(g.add_node(2, 0.0, 0.0));
    assert(g.has_node(2));
    assert(g.get_all_nodes().size() == 2);
    const Node *node1 = g.get_node(1);
    assert(node1 != nullptr);
    assert(node1->id == 1);
    assert(g.get_node(3) == nullptr); // Node not present
    std::cout << "test_add_node PASSED." << std::endl;
}

// Original test: test_add_edge
void test_add_edge()
{
    std::cout << "Running test_add_edge..." << std::endl;
    Graph g;
    // FIXED: Added dummy coordinates
    g.add_node(1, 0.0, 0.0);
    g.add_node(2, 0.0, 0.0);
    g.add_node(3, 0.0, 0.0);

    // Test adding a valid edge
    assert(g.add_edge(101, 1, 2, 5.0));
    assert(g.has_edge(101));
    assert(g.has_edge_between(1, 2));
    assert(g.get_all_edges().size() == 1);
    const Edge *edge101 = g.get_edge(101);
    assert(edge101 != nullptr);
    assert(edge101->id == 101);
    assert(edge101->from_node_id == 1);
    assert(edge101->to_node_id == 2);
    assert(edge101->weight == 5.0);

    // Test getting edge between nodes
    const Edge *edge_between_1_2 = g.get_edge_between(1, 2);
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
    assert(g.add_edge(103, 1, 2, 3.0) == false); // Assuming add_edge doesn't check for this, let's keep it simple

    // Test edge to non-existent node
    assert(!g.add_edge(104, 1, 4, 1.0)); // Node 4 does not exist

    // Test edge from non-existent node
    assert(!g.add_edge(105, 5, 2, 1.0)); // Node 5 does not exist

    // Test get_edges_from_node
    const auto &edges_from_1 = g.get_edges_from_node(1);
    assert(edges_from_1.size() == 1);
    assert(edges_from_1[0].id == 101);

    const auto &edges_from_2 = g.get_edges_from_node(2);
    assert(edges_from_2.size() == 1);
    assert(edges_from_2[0].id == 102);

    const auto &edges_from_3 = g.get_edges_from_node(3);
    assert(edges_from_3.empty());

    // Test get_edges_from_node for a non-existent node
    const auto &edges_from_99 = g.get_edges_from_node(99);
    assert(edges_from_99.empty());

    std::cout << "test_add_edge PASSED." << std::endl;
}

// Original test: test_get_non_existent
void test_get_non_existent()
{
    std::cout << "Running test_get_non_existent..." << std::endl;
    Graph g;
    // FIXED: Added dummy coordinates
    g.add_node(1, 0.0, 0.0);
    g.add_edge(101, 1, 1, 1.0);

    assert(g.get_node(99) == nullptr);
    assert(g.get_edge(999) == nullptr);
    assert(g.get_edge_between(1, 99) == nullptr);
    assert(g.get_edge_between(99, 1) == nullptr);

    std::cout << "test_get_non_existent PASSED." << std::endl;
}

// Helper function to print a path for debugging
void print_path(const std::string &test_name, const std::vector<int> &path)
{
    std::cout << "   " << test_name << ": Path = [ ";
    if (path.empty())
    {
        std::cout << "empty";
    }
    else
    {
        for (size_t i = 0; i < path.size(); ++i)
        {
            std::cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
        }
    }
    std::cout << " ]" << std::endl;
}

// New test function for find_shortest_path
void test_find_shortest_path()
{
    std::cout << "Running test_find_shortest_path..." << std::endl;
    Graph g;

    // FIXED: Added dummy coordinates to all add_node calls
    for (int i = 1; i <= 6; ++i)
        g.add_node(i, 0.0, 0.0);
    g.add_edge(1, 1, 2, 7.0);
    g.add_edge(2, 1, 3, 9.0);
    g.add_edge(3, 1, 6, 14.0);
    g.add_edge(4, 2, 3, 10.0);
    g.add_edge(5, 2, 4, 15.0);
    g.add_edge(6, 3, 4, 11.0);
    g.add_edge(7, 3, 6, 2.0);
    g.add_edge(8, 4, 5, 6.0);
    g.add_edge(9, 6, 5, 9.0);

    std::vector<int> path1 = g.find_shortest_path(1, 2);
    std::vector<int> expected_path1 = {1, 2};
    print_path("Test 1 (1->2)", path1);
    assert(path1 == expected_path1);

    std::vector<int> path2 = g.find_shortest_path(1, 5);
    std::vector<int> expected_path2 = {1, 3, 6, 5};
    print_path("Test 2 (1->5)", path2);
    assert(path2 == expected_path2);

    g.add_node(7, 0.0, 0.0); // Disconnected node
    std::vector<int> path3 = g.find_shortest_path(1, 7);
    print_path("Test 3 (1->7, 7 is disconnected)", path3);
    assert(path3.empty());

    std::vector<int> path4 = g.find_shortest_path(1, 1);
    std::vector<int> expected_path4 = {1};
    print_path("Test 4 (1->1, path to self)", path4);
    assert(path4 == expected_path4);

    Graph g_cycle;
    g_cycle.add_node(10, 0.0, 0.0);
    g_cycle.add_node(20, 0.0, 0.0);
    g_cycle.add_node(30, 0.0, 0.0);
    g_cycle.add_edge(100, 10, 20, 1.0);
    g_cycle.add_edge(101, 20, 30, 1.0);
    g_cycle.add_edge(102, 10, 30, 3.0);
    std::vector<int> path5 = g_cycle.find_shortest_path(10, 30);
    std::vector<int> expected_path5 = {10, 20, 30};
    print_path("Test 5 (10->30, with a longer direct edge)", path5);
    assert(path5 == expected_path5);

    std::vector<int> path6 = g.find_shortest_path(99, 1);
    print_path("Test 6 (99->1, start node non-existent)", path6);
    assert(path6.empty());

    std::vector<int> path7 = g.find_shortest_path(1, 88);
    print_path("Test 7 (1->88, end node non-existent)", path7);
    assert(path7.empty());

    Graph g_multi;
    g_multi.add_node(101, 0.0, 0.0);
    g_multi.add_node(102, 0.0, 0.0);
    g_multi.add_node(103, 0.0, 0.0);
    g_multi.add_node(104, 0.0, 0.0);
    g_multi.add_edge(200, 101, 102, 1.0);
    g_multi.add_edge(201, 101, 103, 1.0);
    g_multi.add_edge(202, 102, 104, 1.0);
    g_multi.add_edge(203, 103, 104, 1.0);
    std::vector<int> path8 = g_multi.find_shortest_path(101, 104);
    print_path("Test 8 (101->104, multiple shortest paths exist)", path8);
    assert(path8.size() == 3);
    bool path8_is_valid = (path8 == std::vector<int>{101, 102, 104} || path8 == std::vector<int>{101, 103, 104});
    assert(path8_is_valid);

    std::cout << "test_find_shortest_path PASSED." << std::endl;
}

int main()
{
    std::cout << "Starting Graph tests..." << std::endl;
    test_add_node();
    test_add_edge();
    test_get_non_existent();
    test_find_shortest_path();
    std::cout << "All Graph tests PASSED." << std::endl;
    return 0;
}