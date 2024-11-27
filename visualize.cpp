#include "graph.h"
#include "ant.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

void visualize(const Graph &graph, const std::map<std::pair<Node *, Node *>, double> &pheromones, const Way &best_way)
{
    const int width = 800;
    const int height = 600;

    sf::RenderWindow window(sf::VideoMode(width, height), "Ant Colony Visualization");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    std::map<Node *, sf::Vector2f> positions;
    int radius = 20;
    int centerX = width / 2, centerY = height / 2;
    int num_nodes = graph.getNodes().size();
    float angle_step = 2 * M_PI / num_nodes;
    int idx = 0;

    for (Node *node : graph.getNodes())
    {
        float angle = idx++ * angle_step;
        float x = centerX + cos(angle) * 200;
        float y = centerY + sin(angle) * 200;
        positions[node] = sf::Vector2f(x, y);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        // Draw edges with pheromone levels
        for (auto &[edge, pheromone] : pheromones)
        {
            Node *u = edge.first;
            Node *v = edge.second;

            sf::Vertex line[] = {
                sf::Vertex(positions[u], sf::Color::Black),
                sf::Vertex(positions[v], sf::Color::Black)};

            float normalized_pheromone = pheromone / 10.0f;
            sf::Color color = sf::Color(0, 0, 255, std::min(255.0f, normalized_pheromone * 255.0f));
            line[0].color = color;
            line[1].color = color;

            window.draw(line, 2, sf::Lines);
        }

        // Draw best path
        for (size_t i = 0; i < best_way.nodes.size() - 1; ++i)
        {
            Node *u = best_way.nodes[i];
            Node *v = best_way.nodes[i + 1];

            sf::Vertex path_line[] = {
                sf::Vertex(positions[u], sf::Color::Red),
                sf::Vertex(positions[v], sf::Color::Red)};

            window.draw(path_line, 2, sf::Lines);
        }

        // Draw nodes
        for (auto &[node, pos] : positions)
        {
            sf::CircleShape circle(radius);
            circle.setPosition(pos.x - radius, pos.y - radius);
            circle.setFillColor(sf::Color::Blue);
            window.draw(circle);

            sf::Text label(node->getName(), font, 12);
            label.setPosition(pos.x - 10, pos.y - 10);
            label.setFillColor(sf::Color::Black);
            window.draw(label);
        }

        window.display();
    }
}
