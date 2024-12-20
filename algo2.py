import numpy as np
from collections import defaultdict
import random
import matplotlib.pyplot as plt

class AntColony:
    def __init__(self, n_ants, n_iterations, decay, alpha=1, beta=2):
        self.n_ants = n_ants
        self.n_iterations = n_iterations
        self.decay = decay
        self.alpha = alpha
        self.beta = beta
        
    def build_graph(self, filename):
        edges = defaultdict(dict)
        max_vertex = 0
        
        with open(filename, 'r') as f:
            for line in f:
                v1, v2, weight = map(int, line.strip().split())
                edges[v1][v2] = weight
                edges[v2][v1] = weight
                max_vertex = max(max_vertex, v1, v2)
        
        n = max_vertex + 1
        distances = np.full((n, n), float('inf'))
        
        for v1 in edges:
            for v2, weight in edges[v1].items():
                distances[v1][v2] = weight
        
        max_weight = np.max(distances[distances != float('inf')]) * 10
        distances[distances == float('inf')] = max_weight
        
        return distances
        
    def solve(self, distances):
        n_cities = len(distances)
        pheromone = np.ones((n_cities, n_cities)) / n_cities
        best_path = None
        best_path_length = float('inf')
        all_lengths = []
        
        # Очищаем файлы для записи новых данных
        open('phero.txt', 'w').close()
        open('prob.txt', 'w').close()
        
        for iteration in range(self.n_iterations):
            paths, path_probabilities = self.construct_solutions(distances, pheromone)
            current_best_path = None
            current_best_length = float('inf')
            
            # Находим лучший путь в текущей итерации
            for i, path in enumerate(paths):
                path_length = self.calculate_path_length(path, distances)
                if path_length < current_best_length:
                    current_best_length = path_length
                    current_best_path = path
                    best_path_probabilities = path_probabilities[i]
            
            # Обновляем лучшее глобальное решение
            if current_best_length < best_path_length:
                best_path_length = current_best_length
                best_path = current_best_path
            
            # Записываем длину текущего лучшего пути
            with open('of2.txt', 'a') as f:
                f.write(f"{current_best_length}\n")
            
            # Записываем уровень феромонов на лучшем пути
            if best_path:
                avg_pheromone = self.calculate_path_pheromone(best_path, pheromone)
                with open('phero.txt', 'a') as f:
                    f.write(f"{avg_pheromone}\n")
            
            # Записываем вероятности выбора правильных вершин
            if best_path and best_path_probabilities:
                avg_probability = sum(best_path_probabilities) / len(best_path_probabilities)
                with open('prob.txt', 'a') as f:
                    f.write(f"{avg_probability}\n")
            
            all_lengths.append(current_best_length)
            pheromone = self.update_pheromone(pheromone, paths, distances)
            
        return best_path, best_path_length, all_lengths
    
    def calculate_path_pheromone(self, path, pheromone):
        path_pheromone = 0
        for i in range(len(path)):
            current = path[i]
            next_city = path[(i+1)%len(path)]
            path_pheromone += pheromone[current][next_city]
        return path_pheromone / len(path)
    
    def construct_solutions(self, distances, pheromone):
        n_cities = len(distances)
        paths = []
        path_probabilities = []
        
        for ant in range(self.n_ants):
            path, probabilities = self.construct_path(distances, pheromone, n_cities)
            paths.append(path)
            path_probabilities.append(probabilities)
            
        return paths, path_probabilities
    
    def construct_path(self, distances, pheromone, n_cities):
        unvisited = set(range(n_cities))
        start = random.randint(0, n_cities-1)
        path = [start]
        path_probabilities = []
        unvisited.remove(start)
        
        while unvisited:
            current = path[-1]
            probabilities = self.calculate_probabilities(current, unvisited, distances, pheromone)
            next_city = random.choices(list(unvisited), probabilities)[0]
            
            # Сохраняем вероятность выбора следующего города
            city_index = list(unvisited).index(next_city)
            path_probabilities.append(probabilities[city_index])
            
            path.append(next_city)
            unvisited.remove(next_city)
            
        return path, path_probabilities
    
    def calculate_probabilities(self, current, unvisited, distances, pheromone):
        probabilities = []
        
        for city in unvisited:
            p = pow(pheromone[current][city], self.alpha) * pow(1.0/distances[current][city], self.beta)
            probabilities.append(p)
            
        total = sum(probabilities)
        if total == 0:
            return [1.0/len(unvisited)] * len(unvisited)
        return [p/total for p in probabilities]
    
    def calculate_path_length(self, path, distances):
        length = 0
        for i in range(len(path)):
            length += distances[path[i]][path[(i+1)%len(path)]]
        return length
    
    def update_pheromone(self, pheromone, paths, distances):
        pheromone *= (1.0 - self.decay)
        
        for path in paths:
            path_length = self.calculate_path_length(path, distances)
            for i in range(len(path)):
                current = path[i]
                next_city = path[(i+1)%len(path)]
                pheromone[current][next_city] += 1.0 / path_length
                pheromone[next_city][current] += 1.0 / path_length
                
        return pheromone

def plot_all_metrics():
    plt.figure(figsize=(15, 5))
    
    # График длины пути
    plt.subplot(131)
    lengths = np.loadtxt('of.txt')
    plt.plot(lengths)
    plt.title('Path Length Convergence')
    plt.xlabel('Iteration')
    plt.ylabel('Path Length')
    plt.grid(True)
    
    # График уровня феромонов
    plt.subplot(132)
    pheromones = np.loadtxt('phero.txt')
    plt.plot(pheromones)
    plt.title('Average Pheromone Level\non Best Path')
    plt.xlabel('Iteration')
    plt.ylabel('Pheromone Level')
    plt.grid(True)
    
    # График вероятностей
    plt.subplot(133)
    probabilities = np.loadtxt('prob.txt')
    plt.plot(probabilities)
    plt.title('Average Probability of\nChoosing Correct City')
    plt.xlabel('Iteration')
    plt.ylabel('Probability')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('all_metrics.png')
    plt.close()

# Параметры алгоритма
n_ants = 20
n_iterations = 100
decay = 0.1
alpha = 1
beta = 2

# Очищаем файл с результатами
open('of2.txt', 'w').close()

# Создаем экземпляр класса и решаем задачу
aco = AntColony(n_ants, n_iterations, decay, alpha, beta)
distances = aco.build_graph('if.txt')
best_path, best_length, all_lengths = aco.solve(distances)

print(f"Best path length: {best_length}")
print(f"Best path: {best_path}")

# Строим графики всех метрик
plot_all_metrics()
