import numpy as np
import matplotlib.pyplot as plt

def plot_all_metrics():
    plt.figure(figsize=(15, 5))
    
    plt.subplot(131)
    lengths = np.loadtxt('ofile.txt')
    plt.plot(lengths)
    plt.title('Ковариация длин пути')
    plt.xlabel('Итерация')
    plt.ylabel('Длина пути')
    plt.grid(True)
    
    plt.subplot(132)
    pheromones = np.loadtxt('pheromones.txt')
    plt.plot(pheromones)
    plt.title('Уровень феромонов\nна лучшем пути')
    plt.xlabel('Итерация')
    plt.ylabel('Уровень феромонов')
    plt.grid(True)
    
    plt.subplot(133)
    probabilities = np.loadtxt('probabilities.txt')
    plt.plot(probabilities)
    plt.title('Вероятность\nВыбора правильного направления')
    plt.xlabel('Итерация')
    plt.ylabel('Вероятность')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('all_graphs.png')
    plt.close()

if __name__ == "__main__":
    plot_all_metrics()