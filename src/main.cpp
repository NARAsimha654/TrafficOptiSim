#include <SFML/Graphics.hpp>
#include "simulation.hpp"
#include "visualizer.hpp"

// This is a crucial function that sets up your city map.
// You can load this from a file later, but for now, we define it here.
void setup_simulation(Simulation &sim)
{
    Graph g;
    // A simple grid for testing
    g.add_node(1, 100, 100);
    g.add_node(2, 500, 100);
    g.add_node(3, 900, 100);
    g.add_node(4, 100, 400);
    g.add_node(5, 500, 400);
    g.add_node(6, 900, 400);
    g.add_node(7, 100, 700);
    g.add_node(8, 500, 700);
    g.add_node(9, 900, 700);

    // Edges (id, from, to, weight) - weight is travel time in ticks
    g.add_edge(12, 1, 2, 80);
    g.add_edge(21, 2, 1, 80);
    g.add_edge(23, 2, 3, 80);
    g.add_edge(32, 3, 2, 80);
    g.add_edge(45, 4, 5, 80);
    g.add_edge(54, 5, 4, 80);
    g.add_edge(56, 5, 6, 80);
    g.add_edge(65, 6, 5, 80);
    g.add_edge(78, 7, 8, 80);
    g.add_edge(87, 8, 7, 80);
    g.add_edge(89, 8, 9, 80);
    g.add_edge(98, 9, 8, 80);

    g.add_edge(14, 1, 4, 60);
    g.add_edge(41, 4, 1, 60);
    g.add_edge(25, 2, 5, 60);
    g.add_edge(52, 5, 2, 60);
    g.add_edge(36, 3, 6, 60);
    g.add_edge(63, 6, 3, 60);
    g.add_edge(47, 4, 7, 60);
    g.add_edge(74, 7, 4, 60);
    g.add_edge(58, 5, 8, 60);
    g.add_edge(85, 8, 5, 60);
    g.add_edge(69, 6, 9, 60);
    g.add_edge(96, 9, 6, 60);
    sim.set_graph(g);

    // Add intersections at all nodes (approaches are outgoing edge IDs)
    sim.add_intersection(Intersection(1, {12, 14}));
    sim.add_intersection(Intersection(2, {21, 23, 25}));
    sim.add_intersection(Intersection(3, {32, 36}));
    sim.add_intersection(Intersection(4, {41, 45, 47}));
    sim.add_intersection(Intersection(5, {52, 54, 56, 58}));
    sim.add_intersection(Intersection(6, {63, 65, 69}));
    sim.add_intersection(Intersection(7, {74, 78}));
    sim.add_intersection(Intersection(8, {85, 87, 89}));
    sim.add_intersection(Intersection(9, {96, 98}));
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "TrafficOptiSim Visualization");
    window.setFramerateLimit(60);

    Simulation sim;
    setup_simulation(sim); // Load our city map

    // Create the visualizer AFTER the simulation graph is set
    Visualizer visualizer(sim.get_graph());

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        sim.tick();

        window.clear(sf::Color(25, 30, 50)); // Dark navy blue background

        // Use the visualizer to draw the current state of the simulation
        visualizer.draw(window, sim);

        window.display();
    }

    return 0;
}