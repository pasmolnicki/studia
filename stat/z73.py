#! /usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.model_selection import KFold, cross_val_score
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import make_pipeline
from sklearn.dummy import DummyClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier


df = pd.read_csv("Credit.csv", index_col=0)

df["Income_gt_50"] = (df["Income"] > 50).astype(int)
df = df.drop("Income", axis=1)

cat_cols = ["Gender", "Student", "Married", "Ethnicity"]
df[cat_cols] = df[cat_cols].apply(lambda x: x.str.strip() if x.dtype == "object" else x)
df = pd.get_dummies(df, columns=cat_cols, drop_first=True)

models = {
    "Baseline": DummyClassifier(strategy="most_frequent"),
    "LogReg": make_pipeline(StandardScaler(), LogisticRegression(max_iter=1000)),
    "Tree (Depth=3)": DecisionTreeClassifier(max_depth=3, random_state=42),
    "Tree (Depth=5)": DecisionTreeClassifier(max_depth=5, random_state=42),
    "k-NN (k=3)": make_pipeline(StandardScaler(), KNeighborsClassifier(n_neighbors=3)),
    "k-NN (k=5)": make_pipeline(StandardScaler(), KNeighborsClassifier(n_neighbors=5)),
}

kf = KFold(n_splits=10, shuffle=True, random_state=42)


def evaluate_models(X, y, title):
    results = {}
    for name, model in models.items():
        cv_scores = cross_val_score(model, X, y, cv=kf, scoring="accuracy")
        results[name] = cv_scores
        print(
            f"{name}: Mean accuracy = {np.mean(cv_scores):.4f} | std = {np.std(cv_scores):.4f}"
        )

    plt.figure(figsize=(10, 6))
    sns.boxplot(data=pd.DataFrame(results))
    plt.title(title, fontsize=14)
    plt.ylabel("Accuracy")
    plt.xticks(rotation=45)
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.tight_layout()
    plt.show()


print("Income > 50")
X_a = df.drop("Income_gt_50", axis=1)
y_a = df["Income_gt_50"]
evaluate_models(X_a, y_a, "Income > 50")

print("\nNumber of credit cards")
X_b = df.drop("Cards", axis=1)
y_b = df["Cards"]
evaluate_models(X_b, y_b, "Number of credit cards ")
