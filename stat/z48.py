import numpy as np
import matplotlib.pyplot as plt

"""
Wygeneruj n = 200 obserwacji z modelu y = sin(2πx) + ε, gdzie
x ∼ U[0, 1] i ε ∼ N (0, 0.3
2
). Używając 5-krotnej walidacji krzyżowej (5-fold CV), dobierz optymalny
stopień wielomianu d ∈ {1, 2, . . . , 100} w regresji wielomianowej, tzn. modelu postaci
ˆf(x) = θ0 + θ1x + θ2x
2 + · · · + θdx
d
,
gdzie współczynniki θ0, . . . , θd są dopasowywane metodą najmniejszych kwadratów. Narysuj w zależności
od d: MSECV (średni kwadratowy błąd predykcji wyznaczony przez CV) oraz błąd treningowy (MSE na
całym zbiorze treningowym po dopasowaniu modelu stopnia d do wszystkich n obserwacji). Wskaż
optymalny stopień wielomianu i skomentuj wyniki.
Wskazówka: Model regresji wielomianowej stopnia d można zbudować jako pipeline sklearn: model =
make_pipeline(PolynomialFeatures(d), LinearRegression()), a następnie użyć
cross_val_score(model, X, y, cv=5, scoring=’neg_mean_squared_error’)
"""

from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import cross_val_score

def main():
    rng = np.random.default_rng(2026)
    n = 200
    x = rng.uniform(0, 1, n)
    epsilon = rng.normal(0, 0.3, n)
    y = np.sin(2 * np.pi * x) + epsilon

    degrees = range(1, 101)
    mse_cv = []
    mse_train = []

    best_degree = None
    best_mse_cv = float('inf')

    for d in degrees:
        model = make_pipeline(PolynomialFeatures(d), LinearRegression())
        neg_mse_cv = cross_val_score(model, x.reshape(-1, 1), y, cv=5, scoring='neg_mean_squared_error')
        mse_cv.append(-neg_mse_cv.mean())

        model.fit(x.reshape(-1, 1), y)
        y_pred_train = model.predict(x.reshape(-1, 1))
        mse_train.append(np.mean((y - y_pred_train) ** 2))

        if mse_cv[-1] < best_mse_cv:
            best_mse_cv = mse_cv[-1]
            best_degree = d

        print(f"Degree: {d}, MSE CV: {mse_cv[-1]:.6f}, MSE Train: {mse_train[-1]:.6f}")

    print(f"\nOptimal Degree: {best_degree} with MSE CV: {best_mse_cv:.6f}")

    plt.figure(figsize=(10, 6))
    plt.plot(degrees, mse_cv, label='MSE CV', marker='o')
    plt.plot(degrees, mse_train, label='MSE Train', marker='x')
    plt.xlabel('Degree of Polynomial')
    plt.ylabel('Mean Squared Error')
    plt.title('MSE vs Degree of Polynomial')
    plt.legend()
    plt.grid()
    plt.show()


if __name__ == "__main__":    main()