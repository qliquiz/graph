#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <set>
#include <limits>
#include <algorithm>

class AntColony
{
private:
  int n_ants;
  int n_iterations;
  double decay;
  double alpha;
  double beta;
  std::mt19937 rng;

public:
  AntColony(int ants, int iterations, double decay_rate, double a = 1, double b = 2)
      : n_ants(ants), n_iterations(iterations), decay(decay_rate), alpha(a), beta(b)
  {
    std::random_device rd;
    rng = std::mt19937(rd());
  }

  std::vector<std::vector<double>> build_graph(const std::string &filename)
  {
    std::ifstream file(filename);
    int v1, v2, weight;
    int max_vertex = 0;

    // Первый проход для определения размера матрицы
    while (file >> v1 >> v2 >> weight)
    {
      max_vertex = std::max(max_vertex, std::max(v1, v2));
    }

    // Создаем матрицу расстояний
    int n = max_vertex + 1;
    std::vector<std::vector<double>> distances(n, std::vector<double>(n, std::numeric_limits<double>::infinity()));

    // Второй проход для заполнения матрицы
    file.clear();
    file.seekg(0);
    while (file >> v1 >> v2 >> weight)
    {
      distances[v1][v2] = weight;
      distances[v2][v1] = weight;
    }

    // Заполняем отсутствующие рёбра большими весами
    double max_weight = 0;
    for (const auto &row : distances)
    {
      for (double weight : row)
      {
        if (weight != std::numeric_limits<double>::infinity())
        {
          max_weight = std::max(max_weight, weight);
        }
      }
    }
    max_weight *= 10;

    for (auto &row : distances)
    {
      for (double &weight : row)
      {
        if (weight == std::numeric_limits<double>::infinity())
        {
          weight = max_weight;
        }
      }
    }

    return distances;
  }

  double calculate_path_length(const std::vector<int> &path, const std::vector<std::vector<double>> &distances)
  {
    double length = 0;
    for (size_t i = 0; i < path.size(); ++i)
    {
      length += distances[path[i]][path[(i + 1) % path.size()]];
    }
    std::cout << length << std::endl;
    return length;
  }

  std::vector<double> calculate_probabilities(int current, const std::set<int> &unvisited,
                                              const std::vector<std::vector<double>> &distances,
                                              const std::vector<std::vector<double>> &pheromone)
  {
    std::vector<double> probabilities;
    for (int city : unvisited)
    {
      double p = std::pow(pheromone[current][city], alpha) * std::pow(1.0 / distances[current][city], beta);
      probabilities.push_back(p);
    }

    double total = 0;
    for (double p : probabilities)
    {
      total += p;
    }

    if (total == 0)
    {
      probabilities = std::vector<double>(unvisited.size(), 1.0 / unvisited.size());
    }
    else
    {
      for (double &p : probabilities)
      {
        p /= total;
      }
    }

    return probabilities;
  }

  std::vector<int> construct_path(const std::vector<std::vector<double>> &distances,
                                  const std::vector<std::vector<double>> &pheromone)
  {
    int n_cities = distances.size();
    std::set<int> unvisited;
    for (int i = 0; i < n_cities; ++i)
    {
      unvisited.insert(i);
    }

    std::uniform_int_distribution<int> dist(0, n_cities - 1);
    int start = dist(rng);
    std::vector<int> path = {start};
    unvisited.erase(start);

    while (!unvisited.empty())
    {
      int current = path.back();
      std::vector<double> probabilities = calculate_probabilities(current, unvisited, distances, pheromone);

      std::discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());
      std::vector<int> unvisited_vec(unvisited.begin(), unvisited.end());
      int next_city_idx = distribution(rng);
      int next_city = unvisited_vec[next_city_idx];

      path.push_back(next_city);
      unvisited.erase(next_city);
    }

    return path;
  }

  std::vector<std::vector<double>> update_pheromone(std::vector<std::vector<double>> pheromone,
                                                    const std::vector<std::vector<int>> &paths,
                                                    const std::vector<std::vector<double>> &distances)
  {
    // Испарение феромона
    for (auto &row : pheromone)
    {
      for (double &p : row)
      {
        p *= (1.0 - decay);
      }
    }

    // Добавление нового феромона
    for (const auto &path : paths)
    {
      double path_length = calculate_path_length(path, distances);
      for (size_t i = 0; i < path.size(); ++i)
      {
        int current = path[i];
        int next = path[(i + 1) % path.size()];
        pheromone[current][next] += 1.0 / path_length;
        pheromone[next][current] += 1.0 / path_length;
      }
    }

    return pheromone;
  }

  std::pair<std::vector<int>, double> solve(const std::vector<std::vector<double>> &distances)
  {
    int n_cities = distances.size();
    std::vector<std::vector<double>> pheromone(n_cities, std::vector<double>(n_cities, 1.0 / n_cities));
    std::vector<int> best_path;
    double best_length = std::numeric_limits<double>::infinity();
    std::ofstream out_file("of.txt");

    for (int iteration = 0; iteration < n_iterations; ++iteration)
    {
      std::vector<std::vector<int>> paths;
      for (int ant = 0; ant < n_ants; ++ant)
      {
        paths.push_back(construct_path(distances, pheromone));
      }

      double current_best_length = std::numeric_limits<double>::infinity();
      std::vector<int> current_best_path;

      for (const auto &path : paths)
      {
        double length = calculate_path_length(path, distances);
        if (length < current_best_length)
        {
          current_best_length = length;
          current_best_path = path;
        }
      }

      if (current_best_length < best_length)
      {
        best_length = current_best_length;
        best_path = current_best_path;
      }

      out_file << current_best_length << std::endl;
      pheromone = update_pheromone(pheromone, paths, distances);
    }

    return {best_path, best_length};
  }
};

int main()
{
  int n_ants = 20;
  int n_iterations = 100;
  double decay = 0.1;
  double alpha = 1;
  double beta = 2;

  AntColony aco(n_ants, n_iterations, decay, alpha, beta);
  auto distances = aco.build_graph("if.txt");
  auto [best_path, best_length] = aco.solve(distances);

  std::cout << "Best path length: " << best_length << std::endl;
  std::cout << "Best path: ";
  for (int city : best_path)
  {
    std::cout << city << " ";
  }
  std::cout << std::endl;

  return 0;
}
