#include "visualizer.hpp"
#include <iostream>
#include <string> // For std::to_string
#include <cmath>  // For sqrt and atan2

Visualizer::Visualizer(const Graph &graph) : graph_(graph)
{
    if (!font_.loadFromFile("DejaVuSans.ttf"))
    {
        std::cerr << "Error: Could not load font 'DejaVuSans.ttf'." << std::endl;
    }

    // Prepare nodes (Edges will be drawn dynamically)
    for (const auto &node_pair : graph_.get_all_nodes())
    {
        const Node &node = node_pair.second;
        sf::CircleShape shape(10.f);                  // Made nodes slightly bigger
        shape.setFillColor(sf::Color(120, 120, 120)); // Darker grey
        shape.setOrigin(10.f, 10.f);
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
    draw_hud(window, sim); // Draw the text last so it's on top
}

// --- Private Helper Implementations ---

// --- COMPLETELY REWRITTEN ---
void Visualizer::draw_edges(sf::RenderWindow &window)
{
    const float road_thickness = 8.f;

    for (const auto &edge_pair : graph_.get_all_edges())
    {
        const Edge &edge = edge_pair.second;
        const Node *from_node = graph_.get_node(edge.from_node_id);
        const Node *to_node = graph_.get_node(edge.to_node_id);

        if (from_node && to_node)
        {
            sf::Vector2f start_pos(from_node->x, from_node->y);
            sf::Vector2f end_pos(to_node->x, to_node->y);
            sf::Vector2f direction = end_pos - start_pos;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            sf::RectangleShape road(sf::Vector2f(length, road_thickness));
            road.setOrigin(0, road_thickness / 2);
            road.setPosition(start_pos);
            road.setFillColor(sf::Color(60, 60, 70)); // Darker road color

            float angle = atan2(direction.y, direction.x) * 180 / 3.14159265f;
            road.setRotation(angle);

            window.draw(road);
        }
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

        sf::CircleShape int_shape(15.f); // Made intersection circle bigger
        int_shape.setOrigin(15.f, 15.f);
        int_shape.setPosition(node->x, node->y);
        int_shape.setFillColor(sf::Color(40, 40, 50)); // Fill with dark color
        int_shape.setOutlineThickness(3);

        if (!intersection.get_approach_ids().empty())
        {
            LightState state = intersection.get_signal_state(intersection.get_approach_ids()[0]);
            switch (state)
            {
            case LightState::GREEN:
                int_shape.setOutlineColor(sf::Color(0, 255, 0, 200));
                break;
            case LightState::YELLOW:
                int_shape.setOutlineColor(sf::Color(255, 255, 0, 200));
                break;
            case LightState::RED:
                int_shape.setOutlineColor(sf::Color(255, 0, 0, 200));
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

// --- HEAVILY MODIFIED ---
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
            if (progress > 1.0f)
                progress = 1.0f; // Cap progress at 100%

            sf::Vector2f start_pos(start_node->x, start_node->y);
            sf::Vector2f end_pos(end_node->x, end_node->y);
            sf::Vector2f current_pos = start_pos + (end_pos - start_pos) * progress;

            // Using a simple circle for a cleaner look
            sf::CircleShape vehicle_shape(6.f);
            vehicle_shape.setOrigin(6.f, 6.f);
            vehicle_shape.setPosition(current_pos);
            vehicle_shape.setFillColor(sf::Color(255, 180, 0)); // A nice orange/yellow
            vehicle_shape.setOutlineColor(sf::Color::Black);
            vehicle_shape.setOutlineThickness(1);

            window.draw(vehicle_shape);
        }
    }
}

// --- NEW FUNCTION ---
void Visualizer::draw_hud(sf::RenderWindow &window, const Simulation &sim)
{
    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 10);

    std::string hud_string = "Tick: " + std::to_string(sim.get_current_tick()) + "\n" + "Active Vehicles: " + std::to_string(sim.get_vehicles().size());

    text.setString(hud_string);
    window.draw(text);
}