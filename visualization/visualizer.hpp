#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <SFML/Graphics.hpp>
#include "simulation.hpp" // Needs to know about the simulation to draw it

class Visualizer
{
public:
    // Constructor can take a reference to the graph to pre-process static elements
    Visualizer(const Graph &graph);

    // Main draw function to be called every frame
    void draw(sf::RenderWindow &window, const Simulation &sim);

private:
    // --- Private Helper Functions ---
    void draw_edges(sf::RenderWindow &window);
    void draw_nodes(sf::RenderWindow &window);
    void draw_vehicles(sf::RenderWindow &window, const Simulation &sim);
    void draw_intersections(sf::RenderWindow &window, const Simulation &sim);

    // --- Member Variables ---
    const Graph &graph_; // A reference to the graph for drawing
    sf::Font font_;      // Font for drawing text (like node IDs)

    // Pre-calculated shapes for static elements (more efficient)
    std::vector<sf::Vertex> edge_vertices_;
    std::vector<sf::CircleShape> node_shapes_;
};

#endif // VISUALIZER_HPP