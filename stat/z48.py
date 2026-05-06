import numpy as np
import matplotlib.pyplot as plt

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
    plt.axvline(best_degree, color='red', linestyle='--', label=f'Optimal Degree: {best_degree}')
    # plt.ylim(0, 20.0)
    plt.yscale('log')
    plt.xlabel('Degree of Polynomial')
    plt.ylabel('Mean Squared Error')
    plt.title('MSE vs Degree of Polynomial')
    plt.legend()
    plt.grid()
    plt.show()


if __name__ == "__main__":    main()