import matplotlib.pyplot as plt

iterations = []
lengths = []

with open('of.txt', 'r') as file:
    for line in file:
        iter_num, path_length = map(int, line.split())
        iterations.append(iter_num)
        lengths.append(path_length)

plt.figure(figsize=(10, 6))
plt.plot(iterations, lengths, marker='o')
plt.title('Длина пути на каждой итерации муравьиного алгоритма')
plt.xlabel('Номер итерации')
plt.ylabel('Длина пути')
plt.grid(True)

plt.show()
