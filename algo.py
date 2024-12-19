import numpy as np
from collections import defaultdict
import random
import matplotlib.pyplot as plt

class AntColony:
    def __init__(self, n_ants, n_iterations, decay, alpha=1, beta=2):
        self.n_ants = n_ants
        self.n_iterations = n_iterations
        self.decay = decay
        self.alpha = alpha  # влияние феромона
        self.beta = beta    # влияние расстояния
        
    def build_graph(self, filename):
      # Читаем данные из файла и строим граф
      edges = defaultdict(dict)
      max_vertex = 0  # Добавляем отслеживание максимального номера вершины
      
      with open(filename, 'r') as f:
          for line in f:
              v1, v2, weight = map(int, line.strip().split())
              edges[v1][v2] = weight
              edges[v2][v1] = weight
              max_vertex = max(max_vertex, v1, v2)  # Обновляем максимальный номер вершины
      
      # Создаем матрицу расстояний с размером (max_vertex + 1) × (max_vertex + 1)
      n = max_vertex + 1
      distances = np.full((n, n), float('inf'))
      
      # Заполняем известные расстояния
      for v1 in edges:
          for v2, weight in edges[v1].items():
              distances[v1][v2] = weight
      
      # Достраиваем граф до полного, используя большие веса для отсутствующих ребер
      max_weight = np.max(distances[distances != float('inf')]) * 10
      distances[distances == float('inf')] = max_weight
      
      return distances
        
    def solve(self, distances):
        n_cities = len(distances)
        pheromone = np.ones((n_cities, n_cities)) / n_cities
        best_path = None
        best_path_length = float('inf')
        all_lengths = []
        
        for iteration in range(self.n_iterations):
            paths = self.construct_solutions(distances, pheromone)
            current_best_path = None
            current_best_length = float('inf')
            
            # Находим лучший путь в текущей итерации
            for path in paths:
                path_length = self.calculate_path_length(path, distances)
                if path_length < current_best_length:
                    current_best_length = path_length
                    current_best_path = path
                    
            # Обновляем лучшее глобальное решение
            if current_best_length < best_path_length:
                best_path_length = current_best_length
                best_path = current_best_path
                
            # Записываем длину текущего лучшего пути
            with open('of.txt', 'a') as f:
                f.write(f"{current_best_length}\n")
            all_lengths.append(current_best_length)
                
            # Обновление феромонов
            pheromone = self.update_pheromone(pheromone, paths, distances)
            
        return best_path, best_path_length, all_lengths
    
    def construct_solutions(self, distances, pheromone):
        n_cities = len(distances)
        paths = []
        
        for ant in range(self.n_ants):
            path = self.construct_path(distances, pheromone, n_cities)
            paths.append(path)
            
        return paths
    
    def construct_path(self, distances, pheromone, n_cities):
        unvisited = set(range(n_cities))
        start = random.randint(0, n_cities-1)
        path = [start]
        unvisited.remove(start)
        
        while unvisited:
            current = path[-1]
            probabilities = self.calculate_probabilities(current, unvisited, distances, pheromone)
            next_city = random.choices(list(unvisited), probabilities)[0]
            path.append(next_city)
            unvisited.remove(next_city)
            
        return path
    
    def calculate_probabilities(self, current, unvisited, distances, pheromone):
        probabilities = []
        
        for city in unvisited:
            # Используем феромоны и эвристическую информацию
            p = pow(pheromone[current][city], self.alpha) * pow(1.0/distances[current][city], self.beta)
            probabilities.append(p)
            
        # Нормализация вероятностей
        total = sum(probabilities)
        if total == 0:
            # Если все вероятности нулевые, используем равномерное распределение
            return [1.0/len(unvisited)] * len(unvisited)
        return [p/total for p in probabilities]
    
    def calculate_path_length(self, path, distances):
        length = 0
        for i in range(len(path)):
            length += distances[path[i]][path[(i+1)%len(path)]]
        print(length)
        return length
    
    def update_pheromone(self, pheromone, paths, distances):
        # Испарение феромона
        pheromone *= (1.0 - self.decay)
        
        # Добавление нового феромона
        for path in paths:
            path_length = self.calculate_path_length(path, distances)
            for i in range(len(path)):
                current = path[i]
                next_city = path[(i+1)%len(path)]
                pheromone[current][next_city] += 1.0 / path_length
                pheromone[next_city][current] += 1.0 / path_length
                
        return pheromone

def plot_convergence(lengths):
    plt.figure(figsize=(10, 6))
    plt.plot(lengths)
    plt.title('Convergence of Ant Colony Optimization')
    plt.xlabel('Iteration')
    plt.ylabel('Path Length')
    plt.grid(True)
    plt.savefig('convergence.png')
    plt.close()

# Параметры алгоритма
n_ants = 20
n_iterations = 100
decay = 0.1
alpha = 1
beta = 2

# Очищаем файл с результатами
open('of.txt', 'w').close()

# Создаем экземпляр класса и решаем задачу
aco = AntColony(n_ants, n_iterations, decay, alpha, beta)
distances = aco.build_graph('if.txt')
best_path, best_length, all_lengths = aco.solve(distances)

print(f"Best path length: {best_length}")
print(f"Best path: {best_path}")

# Строим график сходимости
plot_convergence(all_lengths)