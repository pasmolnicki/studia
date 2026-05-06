import numpy as np
import matplotlib.pyplot as plt


def generate_points(n, dim):
    return np.random.rand(n, dim)

def calc_statistic(n, dim):
    points = generate_points(n, dim)

    # d_min(i)/d_max(i) - euclidean distance from point i to the nearest neighbor / furthest neighbor
    # Calculate R(d) = 1/n * sum(d_max(i) - d_min(i) / d_min(i))
    d_min = np.zeros(n)
    d_max = np.zeros(n)

    for i in range(n):
        distances = np.linalg.norm(points - points[i], axis=1)
        d_min[i] = np.min(distances[distances > 0])  # Exclude the point itself
        d_max[i] = np.max(distances)

    return np.mean((d_max - d_min) / d_min)


def main():
    dims = [1, 2, 5, 10, 50, 100, 500]
    n = 500

    R_d = [calc_statistic(n, dim) for dim in dims]
    plt.figure(figsize=(10, 6))
    plt.plot(dims, R_d, marker='o')
    plt.xscale('log')
    plt.xlabel('Dimension (log scale)')
    plt.ylabel('R(d)')
    plt.title('R(d) as a function of dimension')
    plt.grid()
    plt.show()

if __name__ == "__main__":
    main()