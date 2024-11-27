#include "../headers/dijkstra.h"
#include "../headers/node.h"

Way Dijkstra::shortestWay(std::string departure, std::string target)
{
    Node* begin = std::get<Node*>(graph[departure]);
    Node* end = std::get<Node*>(graph[target]);
    std::map<Node*, int> distances; // Кратчайшие расстояния от начального узла
    std::map<Node*, Node*> previous; // Предыдущие узлы для восстановления пути
    std::priority_queue<std::pair<int, Node*>, std::vector<std::pair<int, Node*>>, std::greater<>> pq; // Очередь с приоритетом

    // Инициализация начальных значений
    for (Node* node : graph.getNodes())
    {
        distances[node] = std::numeric_limits<int>::max();
        previous[node] = nullptr;
    }
    distances[begin] = 0;
    pq.push({0, begin});

    while (!pq.empty())
    {
        Node* current = pq.top().second;
        int current_distance = pq.top().first;
        pq.pop();

        if (current == end)
            break; // Если достигли конечного узла, можно завершать

        // Обходим всех соседей
        for (const auto& neighbour : current->getNeighbours())
        {
            Node* next = neighbour.first;
            int weight = int(neighbour.second);
            int new_distance = current_distance + weight;

            // Обновляем расстояние, если нашли более короткий путь
            if (new_distance < distances[next])
            {
                distances[next] = new_distance;
                previous[next] = current;
                pq.push({new_distance, next});
            }
        }
    }

    // Восстанавливаем путь
    Way way;
    way.length = distances[end];
    for (Node* at = end; at != nullptr; at = previous[at]) way.nodes.push_back(at);
    std::reverse(way.nodes.begin(), way.nodes.end());

    return way;
}
