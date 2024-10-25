import matplotlib.pyplot as plt

iterations = []
lengths = []

with open("of.txt", "r") as f:
    for line in f:
        iteration, length = map(int, line.split())
        iterations.append(iteration)
        lengths.append(length)

plt.plot(iterations, lengths, marker='o')
plt.xlabel('Iteration')
plt.ylabel('Path Length')
plt.title('ACO Path Length vs Iterations')
plt.grid(True)
plt.show()
