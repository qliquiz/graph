#include <vector>
#include <unordered_map>
#include <random>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

class AntColony
{
private:
  int n_ants;
  int n_iterations;
  double decay;
  double alpha;
  double beta;
  std::random_device rd;
  std::mt19937 gen;

public:
  AntColony(int ants, int iterations, double decay_rate, double a = 1.0, double b = 2.0)
      : n_ants(ants), n_iterations(iterations), decay(decay_rate), alpha(a), beta(b), gen(rd()) {}

  std::vector<std::vector<double>> buildGraph(const std::string &filename)
  {
    std::unordered_map<int, std::unordered_map<int, int>> edges;
    int max_vertex = 0;

    std::ifstream file(filename);
    int v1, v2, weight;

    while (file >> v1 >> v2 >> weight)
    {
      edges[v1][v2] = weight;
      edges[v2][v1] = weight;
      max_vertex = std::max({max_vertex, v1, v2});
    }

    int n = max_vertex + 1;
    std::vector<std::vector<double>> distances(n, std::vector<double>(n, std::numeric_limits<double>::infinity()));

    double max_weight = 0;
    for (const auto &[vertex1, connections] : edges)
    {
      for (const auto &[vertex2, weight] : connections)
      {
        distances[vertex1][vertex2] = weight;
        max_weight = std::max(max_weight, (double)weight);
      }
    }

    max_weight *= 10;
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        if (std::isinf(distances[i][j]))
        {
          distances[i][j] = max_weight;
        }
      }
    }

    return distances;
  }

  double calculatePathLength(const std::vector<int> &path, const std::vector<std::vector<double>> &distances)
  {
    double length = 0;
    for (size_t i = 0; i < path.size(); i++)
    {
      length += distances[path[i]][path[(i + 1) % path.size()]];
    }
    return length;
  }

  std::vector<double> calculateProbabilities(int current, const std::vector<bool> &visited,
                                              const std::vector<std::vector<double>> &distances,
                                              const std::vector<std::vector<double>> &pheromone)
  {
    std::vector<double> probabilities;
    std::vector<int> unvisited;

    for (size_t i = 0; i < visited.size(); i++)
    {
      if (!visited[i])
      {
        unvisited.push_back(i);
      }
    }

    for (int city : unvisited)
    {
      double p = pow(pheromone[current][city], alpha) * pow(1.0 / distances[current][city], beta);
      probabilities.push_back(p);
    }

    double total = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
    if (total == 0)
    {
      return std::vector<double>(unvisited.size(), 1.0 / unvisited.size());
    }

    for (double &p : probabilities)
    {
      p /= total;
    }

    return probabilities;
  }

  std::pair<std::vector<int>, std::vector<double>> constructPath(
      const std::vector<std::vector<double>> &distances,
      const std::vector<std::vector<double>> &pheromone)
  {
    int n_cities = distances.size();
    std::vector<bool> visited(n_cities, false);
    std::vector<int> path;
    std::vector<double> path_probabilities;

    std::uniform_int_distribution<> dis(0, n_cities - 1);
    int start = dis(gen);
    path.push_back(start);
    visited[start] = true;

    while (path.size() < n_cities)
    {
      int current = path.back();
      auto probabilities = calculateProbabilities(current, visited, distances, pheromone);

      std::vector<int> unvisited;
      for (int i = 0; i < n_cities; i++)
      {
        if (!visited[i])
        {
          unvisited.push_back(i);
        }
      }

      std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());
      int next_idx = dist(gen);
      int next_city = unvisited[next_idx];

      path_probabilities.push_back(probabilities[next_idx]);
      path.push_back(next_city);
      visited[next_city] = true;
    }

    return {path, path_probabilities};
  }

  void solve(const std::vector<std::vector<double>> &distances)
  {
    int n_cities = distances.size();
    std::vector<std::vector<double>> pheromone(n_cities, std::vector<double>(n_cities, 1.0 / n_cities));

    std::vector<int> best_path;
    double best_path_length = std::numeric_limits<double>::infinity();

    std::ofstream ofile("ofile.txt", std::ios::trunc);
    std::ofstream phero("pheromones.txt", std::ios::trunc);
    std::ofstream prob("probabilities.txt", std::ios::trunc);
    ofile.close();
    phero.close();
    prob.close();

    for (int iteration = 0; iteration < n_iterations; iteration++)
    {
      std::vector<std::vector<int>> paths;
      std::vector<std::vector<double>> path_probabilities;

      for (int ant = 0; ant < n_ants; ant++)
      {
        auto [path, probs] = constructPath(distances, pheromone);
        paths.push_back(path);
        path_probabilities.push_back(probs);
      }

      std::vector<int> current_best_path;
      double current_best_length = std::numeric_limits<double>::infinity();
      std::vector<double> best_path_probs;

      for (size_t i = 0; i < paths.size(); i++)
      {
        double path_length = calculatePathLength(paths[i], distances);
        if (path_length < current_best_length)
        {
          current_best_length = path_length;
          current_best_path = paths[i];
          best_path_probs = path_probabilities[i];
        }
      }

      if (current_best_length < best_path_length)
      {
        best_path_length = current_best_length;
        best_path = current_best_path;
      }

      std::ofstream ofile("ofile.txt", std::ios::app);
      ofile << current_best_length << std::endl;
      ofile.close();

      if (!best_path.empty())
      {
        double pher_level = 0;
        for (size_t i = 0; i < best_path.size(); i++)
        {
          pher_level += pheromone[best_path[i]][best_path[(i + 1) % best_path.size()]];
        }

        std::ofstream phero("pheromones.txt", std::ios::app);
        phero << pher_level << std::endl;
        phero.close();
      }

      if (!best_path_probs.empty())
      {
        double probability = std::accumulate(best_path_probs.begin(), best_path_probs.end(), 0.0) / best_path_probs.size();
        std::ofstream prob("probabilities.txt", std::ios::app);
        prob << probability << std::endl;
        prob.close();
      }

      for (auto &row : pheromone)
      {
        for (double &p : row)
        {
          p *= (1.0 - decay);
        }
      }

      for (const auto &path : paths)
      {
        double path_length = calculatePathLength(path, distances);
        for (size_t i = 0; i < path.size(); i++)
        {
          int current = path[i];
          int next = path[(i + 1) % path.size()];
          pheromone[current][next] += 1.0 / path_length;
          pheromone[next][current] += 1.0 / path_length;
        }
      }
    }

    std::cout << "Best path length: " << best_path_length << std::endl;
    std::cout << "Best path: ";
    for (int city : best_path)
    {
      std::cout << city << " ";
    }
    std::cout << std::endl;
  }
};

int main()
{
  int n_ants = 20;
  int n_iterations = 100;
  double decay = 0.1;
  double alpha = 1.0;
  double beta = 2.0;

  AntColony aco(n_ants, n_iterations, decay, alpha, beta);
  auto distances = aco.buildGraph("if.txt");
  aco.solve(distances);

  return 0;
}
