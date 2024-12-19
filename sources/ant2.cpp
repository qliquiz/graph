#include "../headers/graph.h"
#include "../headers/way.h"
#include <random>

class AntColony
{
  Graph &graph;
  std::map<std::pair<Node *, Node *>, double> pheromones;
  std::map<std::pair<Node *, Node *>, size_t> edge_weights;

  double alpha; // влияние феромонов
  double beta;  // влияние эвристики
  double rho;   // скорость испарения
  double Q;     // количество откладываемых феромонов

  size_t ant_count;
  size_t iterations;

  void initializePheromones()
  {
    // Инициализация феромонов и весов рёбер
    for (const auto &node : graph.getNodes())
    {
      for (const auto &neighbour : node->getNeighbours())
      {
        pheromones[{node, neighbour.first}] = 1.0;
        edge_weights[{node, neighbour.first}] = neighbour.second;
      }
    }
  }

  double calculateProbability(Node *current, Node *next)
  {
    if (edge_weights.find({current, next}) == edge_weights.end())
      return 0.0;

    double tau = pheromones[{current, next}];
    double eta = 1.0 / edge_weights[{current, next}];

    return pow(tau, alpha) * pow(eta, beta);
  }

  Node *selectNextNode(Node *current, const std::set<Node *> &unvisited)
  {
    std::vector<std::pair<Node *, double>> probabilities;
    double total = 0.0;

    // Вычисляем вероятности только для непосещённых соседей
    for (const auto &neighbour : current->getNeighbours())
    {
      if (unvisited.find(neighbour.first) != unvisited.end())
      {
        double prob = calculateProbability(current, neighbour.first);
        probabilities.push_back({neighbour.first, prob});
        total += prob;
      }
    }

    if (probabilities.empty())
      return nullptr;

    // Используем рулеточную селекцию для выбора следующего узла
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, total);
    double r = dis(gen);

    double cumsum = 0.0;
    for (const auto &[node, prob] : probabilities)
    {
      cumsum += prob;
      if (r <= cumsum)
        return node;
    }

    return probabilities[0].first;
  }

  void updatePheromones(const std::vector<Way> &ant_paths)
  {
    // Испарение феромонов
    for (auto &[edge, pheromone] : pheromones)
    {
      pheromone *= (1.0 - rho);
      if (pheromone < 0.0001)
        pheromone = 0.0001; // Минимальный уровень феромонов
    }

    // Добавление новых феромонов
    for (const auto &path : ant_paths)
    {
      double delta = Q / path.length; // Количество феромонов обратно пропорционально длине пути

      for (size_t i = 0; i < path.nodes.size() - 1; ++i)
      {
        Node *current = path.nodes[i];
        Node *next = path.nodes[i + 1];
        pheromones[{current, next}] += delta;
      }
    }
  }

public:
  AntColony(Graph &g, double a = 1.0, double b = 5.0, double evap_rate = 0.1,
            double pher_intensity = 100.0, size_t ants = 20, size_t iters = 100)
      : graph(g), alpha(a), beta(b), rho(evap_rate), Q(pher_intensity),
        ant_count(ants), iterations(iters)
  {
    initializePheromones();
  }

  std::pair<Way, std::vector<int>> shortestWay(const std::string departure, const std::string target)
  {
    Node *start = std::get<Node *>(graph[departure]);
    Node *end = std::get<Node *>(graph[target]);

    Way best_way;
    std::vector<int> best_lengths;

    for (size_t iter = 0; iter < iterations; ++iter)
    {
      std::vector<Way> ant_paths;

      // Каждый муравей ищет путь
      for (size_t ant = 0; ant < ant_count; ++ant)
      {
        std::set<Node *> unvisited(graph.getNodes().begin(), graph.getNodes().end());
        Node *current = start;
        Way way;
        way.nodes.push_back(current);
        unvisited.erase(current);

        while (current != end && !unvisited.empty())
        {
          Node *next = selectNextNode(current, unvisited);
          if (!next)
            break;

          way.nodes.push_back(next);
          way.length += edge_weights[{current, next}];
          unvisited.erase(next);
          current = next;
        }

        if (current == end)
        {
          ant_paths.push_back(way);
          if (best_way.nodes.empty() || way.length < best_way.length)
          {
            best_way = way;
          }
        }
      }

      updatePheromones(ant_paths);
      best_lengths.push_back(best_way.length);
    }

    return {best_way, best_lengths};
  }
};
