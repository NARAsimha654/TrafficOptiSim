#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <SFML/Graphics.hpp>
#include "simulation.hpp"

class Visualizer
{
public:
    Visualizer(const Graph &graph);
    void draw(sf::RenderWindow &window, const Simulation &sim);

private:
    void draw_edges(sf::RenderWindow &window);
    void draw_nodes(sf::RenderWindow &window);
    void draw_vehicles(sf::RenderWindow &window, const Simulation &sim);
    void draw_intersections(sf::RenderWindow &window, const Simulation &sim);
    void draw_hud(sf::RenderWindow &window, const Simulation &sim);

    const Graph &graph_;
    sf::Font font_;
    std::vector<sf::CircleShape> node_shapes_;
};

#endif // VISUALIZER_HPP