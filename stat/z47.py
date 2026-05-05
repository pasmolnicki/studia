import numpy as np
import matplotlib.pyplot as plt

"""
 Objętość hiperkuli w przestrzeniach wielowymiarowych. Rozważamy
objętość Vd hiperkuli o promieniu 1 w przestrzeni d-wymiarowej dla d = 1, 2, . . . , 20. Dla jakich d udział
objętości hiperkuli w objętości otaczającej ją hiperkostki [−1, 1]d
staje się pomijalny? Porównaj objętość
wyliczoną ze wzoru analitycznego z objętością uzyskaną metodą Monte Carlo (tzn. losuj punkty z [−1, 1]d
i sprawdzaj, czy należą do hiperkuli). Dla jakich d metoda Monte Carlo przestaje dawać sensowne wyniki
i dlaczego?
"""

def volume_hypersphere(d: int) -> float:
    """Oblicza objętość hiperkuli o promieniu 1 w przestrzeni d-wymiarowej."""
    from math import gamma, pi
    return (pi ** (d / 2)) / gamma((d / 2) + 1)

def monte_carlo_volume(d: int, n_samples: int, rng: np.random.Generator) -> float:
    """Oblicza objętość hiperkuli metodą Monte Carlo."""
    points = rng.uniform(low=-1, high=1, size=(n_samples, d))
    inside_count = np.sum(np.linalg.norm(points, axis=1) <= 1)
    return (inside_count / n_samples) * (2 ** d) # ilość punktów wewnątrz hiperkuli / ilość wszystkich punktów * objętość hiperkostki

def main():
    rng = np.random.default_rng(2026)
    dimensions = range(1, 21)
    analytical_volumes = []
    monte_carlo_volumes = []

    n_samples = 1000000

    for d in dimensions:
        Vd_analytical = volume_hypersphere(d)
        Vd_monte_carlo = monte_carlo_volume(d, n_samples, rng)

        analytical_volumes.append(Vd_analytical)
        monte_carlo_volumes.append(Vd_monte_carlo)

        print(f"Dimension: {d}, Analytical Volume: {Vd_analytical:.6e}, Monte Carlo Volume: {Vd_monte_carlo:.6e}")

    plt.figure(figsize=(10, 6))
    plt.plot(dimensions, analytical_volumes, label='Analytical Volume', marker='o')
    plt.plot(dimensions, monte_carlo_volumes, label='Monte Carlo Volume', marker='x')
    plt.yscale('log')
    plt.xlabel('Dimension')
    plt.ylabel('Volume of Hypersphere (log scale)')
    plt.title('Volume of Hypersphere vs Dimension')
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == "__main__":    main()