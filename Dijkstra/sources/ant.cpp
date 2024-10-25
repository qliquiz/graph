#include "../headers/ant.h"

double AntColony::probability(Node* current, Node* next)
{
    double tau = pheromones[{current, next}];  // уровень феромонов
    double eta = 1.0 / next->getNeighbours().size(); // эвристика, обратная расстоянию или числу соседей

    return pow(tau, alpha) * pow(eta, beta);
}

void AntColony::updatePheromones(const Way& way)
{
    for (size_t i = 0; i < way.nodes.size() - 1; ++i)
    {
        Node* current = way.nodes[i];
        Node* next = way.nodes[i + 1];
        pheromones[{current, next}] += pheromone_intensity / way.length;
    }
    // Испарение феромонов
    for (auto& [edge, pheromone] : pheromones)
        pheromone *= (1 - evaporation_rate);
}


std::pair<Way, std::vector<int>> AntColony::shortestWay(char departure, char target)
{
    Node* start = std::get<Node*>(graph[departure]);
    Node* end = std::get<Node*>(graph[target]);

    Way best_way;
    std::vector<int> best_lengths_per_iteration; // вектор для хранения длин оптимальных путей

    for (size_t iter = 0; iter < iterations; ++iter)
    {
        for (size_t ant = 0; ant < ant_count; ++ant)
        {
            Node* current = start;
            Way way;
            int distance = 0;
            bool valid_path = true;

            while (current != end)
            {
                way.nodes.push_back(current);

                if (current->getNeighbours().empty() && current != end) {
                    valid_path = false;
                    break;
                }

                std::vector<std::pair<Node*, double>> probabilities;
                double total_prob = 0;

                for (const auto& neighbour : current->getNeighbours())
                {
                    double prob = probability(current, neighbour.first);
                    probabilities.push_back({neighbour.first, prob});
                    total_prob += prob;
                }

                if (total_prob == 0) {
                    valid_path = false;
                    break;
                }

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> dis(0, total_prob);

                double rand_prob = dis(gen);
                double cumulative_prob = 0;
                Node* next = nullptr;
                size_t edge_weight = 0;

                for (const auto& p : probabilities)
                {
                    cumulative_prob += p.second;
                    if (rand_prob <= cumulative_prob)
                    {
                        next = p.first;
                        for (const auto& neighbour : current->getNeighbours())
                        {
                            if (neighbour.first == next) {
                                edge_weight = neighbour.second;
                                break;
                            }
                        }
                        distance += edge_weight;
                        break;
                    }
                }

                if (next == nullptr) {
                    valid_path = false;
                    break;
                }

                current = next;
            }

            if (current == end && valid_path)
            {
                way.nodes.push_back(end);
                way.length = distance;

                if (way.length < best_way.length) {
                    best_way = way;
                }

                updatePheromones(way);
            }
        }

        best_lengths_per_iteration.push_back(best_way.length);
    }

    return {best_way, best_lengths_per_iteration}; // возвращаем лучший путь и длины на каждой итерации
}
