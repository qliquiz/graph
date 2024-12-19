import matplotlib.pyplot as plt

iter = 0
iterations = []
lengths = []

with open('of.txt', 'r') as file:
    for line in file:
        iter += 1
        iterations.append(iter)
        lengths.append(int(line))

plt.figure(figsize=(10, 6))
plt.plot(iterations, lengths, marker='o')
plt.title('Длина пути на каждой итерации муравьиного алгоритма')
plt.xlabel('Номер итерации')
plt.ylabel('Длина пути')
plt.grid(True)

plt.show()
