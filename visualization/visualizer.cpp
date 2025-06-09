#include "visualizer.hpp"
#include <iostream> // For error messages

Visualizer::Visualizer(const Graph &graph) : graph_(graph)
{
    // --- Load a font ---
    // Make sure "DejaVuSans.ttf" is in your project directory or provide a full path.
    // You can download this font for free online.
    if (!font_.loadFromFile("DejaVuSans.ttf"))
    {
        std::cerr << "Error: Could not load font 'DejaVuSans.ttf'. Please make sure it's in the executable's directory." << std::endl;
    }

    // --- Pre-process static elements for efficiency ---

    // Prepare edges
    for (const auto &edge_pair : graph_.get_all_edges())
    {
        const Edge &edge = edge_pair.second;
        const Node *from_node = graph_.get_node(edge.from_node_id);
        const Node *to_node = graph_.get_node(edge.to_node_id);

        if (from_node && to_node)
        {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(from_node->x, from_node->y), sf::Color(100, 100, 100)),
                sf::Vertex(sf::Vector2f(to_node->x, to_node->y), sf::Color(100, 100, 100))};
            edge_vertices_.push_back(line[0]);
            edge_vertices_.push_back(line[1]);
        }
    }

    // Prepare nodes
    for (const auto &node_pair : graph_.get_all_nodes())
    {
        const Node &node = node_pair.second;
        sf::CircleShape shape(8.f);
        shape.setFillColor(sf::Color(200, 200, 200));
        shape.setOrigin(8.f, 8.f); // Center the origin
        shape.setPosition(node.x, node.y);
        node_shapes_.push_back(shape);
    }
}

// This is the main function called from the game loop
void Visualizer::draw(sf::RenderWindow &window, const Simulation &sim)
{
    draw_edges(window);
    draw_nodes(window);
    draw_intersections(window, sim);
    draw_vehicles(window, sim);
}

// --- Private Helper Implementations ---

void Visualizer::draw_edges(sf::RenderWindow &window)
{
    if (!edge_vertices_.empty())
    {
        window.draw(&edge_vertices_[0], edge_vertices_.size(), sf::Lines);
    }
}

void Visualizer::draw_nodes(sf::RenderWindow &window)
{
    for (const auto &shape : node_shapes_)
    {
        window.draw(shape);
    }
}

void Visualizer::draw_intersections(sf::RenderWindow &window, const Simulation &sim)
{
    for (const auto &pair : sim.get_intersections())
    {
        const Intersection &intersection = pair.second;
        const Node *node = graph_.get_node(intersection.get_id());
        if (!node)
            continue;

        // Draw a larger circle for the intersection
        sf::CircleShape int_shape(12.f);
        int_shape.setOrigin(12.f, 12.f);
        int_shape.setPosition(node->x, node->y);
        int_shape.setFillColor(sf::Color::Transparent);
        int_shape.setOutlineThickness(2);

        // Color the outline based on the first approach's light state for simplicity
        if (!intersection.get_approach_ids().empty())
        {
            LightState state = intersection.get_signal_state(intersection.get_approach_ids()[0]);
            switch (state)
            {
            case LightState::GREEN:
                int_shape.setOutlineColor(sf::Color::Green);
                break;
            case LightState::YELLOW:
                int_shape.setOutlineColor(sf::Color::Yellow);
                break;
            case LightState::RED:
                int_shape.setOutlineColor(sf::Color::Red);
                break;
            }
        }
        else
        {
            int_shape.setOutlineColor(sf::Color(150, 150, 150));
        }

        window.draw(int_shape);
    }
}

void Visualizer::draw_vehicles(sf::RenderWindow &window, const Simulation &sim)
{
    for (const auto &pair : sim.get_vehicles())
    {
        const Vehicle &vehicle = pair.second;

        if (vehicle.get_state() != VehicleState::EN_ROUTE)
            continue;

        const Node *start_node = graph_.get_node(vehicle.get_current_node_id());
        const Node *end_node = graph_.get_node(vehicle.get_next_node_id());

        if (start_node && end_node)
        {
            float progress = (float)vehicle.get_current_edge_progress_ticks() / vehicle.get_current_edge_total_ticks();

            sf::Vector2f start_pos(start_node->x, start_node->y);
            sf::Vector2f end_pos(end_node->x, end_node->y);

            // Linear interpolation to find the vehicle's current position
            sf::Vector2f current_pos = start_pos + (end_pos - start_pos) * progress;

            sf::RectangleShape vehicle_shape(sf::Vector2f(10.f, 6.f));
            vehicle_shape.setOrigin(5.f, 3.f);
            vehicle_shape.setPosition(current_pos);
            vehicle_shape.setFillColor(sf::Color::Cyan);

            // Rotate the vehicle to face the direction of travel
            float angle = atan2(end_pos.y - start_pos.y, end_pos.x - start_pos.x) * 180 / 3.14159265f;
            vehicle_shape.setRotation(angle);

            window.draw(vehicle_shape);
        }
    }
}