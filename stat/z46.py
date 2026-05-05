import numpy as np
import matplotlib.pyplot as plt

def generate_data(n: int, d: int, rng: np.random.Generator):
    X = rng.normal(loc=0.0, scale=1.0, size=(n, d))
    norms = np.linalg.norm(X, axis=1)
    rd = np.median(norms)
    y = (norms <= rd).astype(int)
    return X, y

def knn_predict(X_train: np.ndarray, y_train: np.ndarray, X_test: np.ndarray):
    from sklearn.neighbors import KNeighborsClassifier
    knn = KNeighborsClassifier(n_neighbors=1)
    knn.fit(X_train, y_train)
    return knn.predict(X_test)

def main():
    rng = np.random.default_rng(2026)
    n_train = 1000
    n_test = 500
    dimensions = [1, 2, 5, 10, 20, 50, 100]
    accuracies = []

    for d in dimensions:
        X_train, y_train = generate_data(n_train, d, rng)
        X_test, y_test = generate_data(n_test, d, rng)
        y_pred = knn_predict(X_train, y_train, X_test)
        accuracy = np.mean(y_pred == y_test)
        accuracies.append(accuracy)
        print(f"Dimension: {d}, Accuracy: {accuracy:.4f}")

    plt.figure(figsize=(10, 6))
    plt.plot(dimensions, accuracies, marker='o')
    plt.axhline(0.5, color='red', linestyle='--', label='Random Classifier Accuracy (0.5)')
    plt.xlabel('Dimension')
    plt.ylabel('Accuracy')
    plt.title('KNN Accuracy vs Dimension')
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == "__main__":    main()