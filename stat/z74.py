#! /usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import make_pipeline
from sklearn.inspection import DecisionBoundaryDisplay
from sklearn.linear_model import LogisticRegression
from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier

df = pd.read_csv("Credit.csv", index_col=0)
df["Income_gt_50"] = (df["Income"] > 50).astype(int)

X = df[["Limit", "Rating"]].values

y_a = df["Income_gt_50"].values
y_b = df["Cards"].values

datasets = [(X, y_a, "a) Income > 50"), (X, y_b, "b) Number of cards")]

names = ["K-Nearest Neighbors", "Logistic Regression", "Decision Tree"]

classifiers = [
    KNeighborsClassifier(n_neighbors=5),
    LogisticRegression(random_state=42),
    DecisionTreeClassifier(max_depth=5, random_state=42),
]

figure = plt.figure(figsize=(18, 9))
i = 1

for ds_cnt, (X, y, ds_name) in enumerate(datasets):
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.4, random_state=42
    )

    x_min, x_max = X[:, 0].min() - 0.5, X[:, 0].max() + 0.5
    y_min, y_max = X[:, 1].min() - 0.5, X[:, 1].max() + 0.5

    n_classes = len(np.unique(y))
    if n_classes == 2:
        cm = plt.cm.RdBu
        cm_bright = ListedColormap(["#FF0000", "#0000FF"])
    else:
        cm = plt.cm.tab10
        cm_bright = plt.cm.tab10

    ax = plt.subplot(len(datasets), len(classifiers) + 1, i)
    if ds_cnt == 0:
        ax.set_title("Input data", fontsize=14)

    ax.set_ylabel(ds_name, fontsize=14, fontweight="bold")

    ax.scatter(X_train[:, 0], X_train[:, 1], c=y_train, cmap=cm_bright, edgecolors="k")
    ax.scatter(
        X_test[:, 0], X_test[:, 1], c=y_test, cmap=cm_bright, alpha=0.6, edgecolors="k"
    )

    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)
    ax.set_xticks(())
    ax.set_yticks(())
    i += 1

    for name, clf in zip(names, classifiers):
        ax = plt.subplot(len(datasets), len(classifiers) + 1, i)

        clf = make_pipeline(StandardScaler(), clf)

        clf.fit(X_train, y_train)
        score = clf.score(X_test, y_test)

        DecisionBoundaryDisplay.from_estimator(
            clf, X, cmap=cm, alpha=0.8, ax=ax, eps=0.5, response_method="predict"
        )

        ax.scatter(
            X_train[:, 0], X_train[:, 1], c=y_train, cmap=cm_bright, edgecolors="k"
        )
        ax.scatter(
            X_test[:, 0],
            X_test[:, 1],
            c=y_test,
            cmap=cm_bright,
            edgecolors="k",
            alpha=0.6,
        )

        ax.set_xlim(x_min, x_max)
        ax.set_ylim(y_min, y_max)
        ax.set_xticks(())
        ax.set_yticks(())

        if ds_cnt == 0:
            ax.set_title(name, fontsize=14)

        ax.text(
            x_max - 0.05 * (x_max - x_min),
            y_min + 0.05 * (y_max - y_min),
            ("%.2f" % score).lstrip("0"),
            size=15,
            horizontalalignment="right",
            bbox=dict(boxstyle="round", alpha=0.8, facecolor="white"),
        )
        i += 1

plt.tight_layout()
plt.show()
