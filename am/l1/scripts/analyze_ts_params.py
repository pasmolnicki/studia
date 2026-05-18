#!/usr/bin/env python3
from __future__ import annotations

import argparse
import math
import os
from dataclasses import dataclass
from typing import Dict, List, Tuple

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.ensemble import RandomForestRegressor
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_absolute_error, r2_score
from sklearn.model_selection import train_test_split
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler


PARAM_COLUMNS = [
    "tabu_tenure",
    "max_iterations",
    "no_improve_limit",
]
TARGET_COLUMN = "avg_distance"
REQUIRED_COLUMNS = PARAM_COLUMNS + [TARGET_COLUMN]


@dataclass
class ModelReport:
    name: str
    r2: float
    mae: float


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Analyze Tabu Search parameter tuning results and generate plots."
    )
    parser.add_argument(
        "csv_path",
        nargs="?",
        default="results/ts_parameter_tuning.csv",
        help="Path to the CSV file (default: results/ts_parameter_tuning.csv)",
    )
    parser.add_argument(
        "--top",
        type=int,
        default=10,
        help="Number of best parameter combinations to display",
    )
    parser.add_argument(
        "--out-dir",
        default="results/ts_analysis",
        help="Directory to write plots and reports",
    )
    parser.add_argument(
        "--show",
        action="store_true",
        help="Show plots interactively instead of only saving",
    )
    return parser.parse_args()


def load_data(csv_path: str) -> pd.DataFrame:
    df = pd.read_csv(csv_path)
    missing = [c for c in REQUIRED_COLUMNS if c not in df.columns]
    if missing:
        raise ValueError(f"Missing required columns: {missing}")

    df = df.copy()
    for col in REQUIRED_COLUMNS:
        df[col] = pd.to_numeric(df[col], errors="coerce")

    if df[REQUIRED_COLUMNS].isna().any().any():
        bad_rows = df[df[REQUIRED_COLUMNS].isna().any(axis=1)]
        raise ValueError(f"Found non-numeric values in rows:\n{bad_rows}")

    return df


def ensure_out_dir(path: str) -> None:
    os.makedirs(path, exist_ok=True)


def print_top_combinations(df: pd.DataFrame, top_n: int) -> pd.DataFrame:
    top_n = max(1, top_n)
    top_df = df.nsmallest(top_n, TARGET_COLUMN).reset_index(drop=True)
    print("\nBest parameter combinations:\n")
    print(top_df[REQUIRED_COLUMNS].to_string(index=False))
    return top_df


def correlation_analysis(df: pd.DataFrame) -> Tuple[pd.DataFrame, pd.DataFrame]:
    pearson = df[REQUIRED_COLUMNS].corr(method="pearson")
    spearman = df[REQUIRED_COLUMNS].corr(method="spearman")

    print("\nPearson correlation with avg_distance:\n")
    print(
        pearson[TARGET_COLUMN]
        .drop(TARGET_COLUMN)
        .sort_values()
        .to_string()
    )

    print("\nSpearman correlation with avg_distance:\n")
    print(
        spearman[TARGET_COLUMN]
        .drop(TARGET_COLUMN)
        .sort_values()
        .to_string()
    )

    return pearson, spearman


def fit_models(df: pd.DataFrame) -> Tuple[List[ModelReport], Dict[str, float]]:
    X = df[PARAM_COLUMNS].values
    y = df[TARGET_COLUMN].values

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.25, random_state=42
    )

    lr = Pipeline(
        [
            ("scaler", StandardScaler()),
            ("model", LinearRegression()),
        ]
    )
    lr.fit(X_train, y_train)
    lr_pred = lr.predict(X_test)
    lr_report = ModelReport(
        name="LinearRegression",
        r2=r2_score(y_test, lr_pred),
        mae=mean_absolute_error(y_test, lr_pred),
    )

    rf = RandomForestRegressor(
        n_estimators=300,
        random_state=42,
        n_jobs=-1,
    )
    rf.fit(X_train, y_train)
    rf_pred = rf.predict(X_test)
    rf_report = ModelReport(
        name="RandomForestRegressor",
        r2=r2_score(y_test, rf_pred),
        mae=mean_absolute_error(y_test, rf_pred),
    )

    importances = dict(zip(PARAM_COLUMNS, rf.feature_importances_))

    print("\nModel quality (higher R2, lower MAE are better):\n")
    for report in [lr_report, rf_report]:
        print(
            f"- {report.name}: R2={report.r2:.3f}, MAE={report.mae:.1f}"
        )

    print("\nRandom forest feature importance:\n")
    for name, score in sorted(importances.items(), key=lambda x: x[1], reverse=True):
        print(f"- {name}: {score:.3f}")

    return [lr_report, rf_report], importances


def plot_correlation_heatmap(
    corr: pd.DataFrame,
    out_path: str,
    close_fig: bool = True,
) -> None:
    fig, ax = plt.subplots(figsize=(7, 6))
    im = ax.imshow(corr.values, cmap="coolwarm", vmin=-1, vmax=1)
    ax.set_xticks(range(len(corr.columns)))
    ax.set_yticks(range(len(corr.index)))
    ax.set_xticklabels(corr.columns, rotation=30, ha="right")
    ax.set_yticklabels(corr.index)

    for (i, j), val in np.ndenumerate(corr.values):
        ax.text(j, i, f"{val:.2f}", ha="center", va="center", fontsize=8)

    ax.set_title("Correlation heatmap (Pearson)")
    fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
    fig.tight_layout()
    fig.savefig(out_path, dpi=160)
    if close_fig:
        plt.close(fig)


def plot_feature_importance(
    importances: Dict[str, float],
    out_path: str,
    close_fig: bool = True,
) -> None:
    labels = list(importances.keys())
    scores = [importances[k] for k in labels]

    fig, ax = plt.subplots(figsize=(6, 4))
    x = np.arange(len(labels))
    ax.bar(x, scores, color="#4c78a8")
    ax.set_title("Random forest feature importance")
    ax.set_ylabel("Importance")
    ax.set_ylim(0, max(scores) * 1.15)
    ax.set_xticks(x)
    ax.set_xticklabels(labels, rotation=25, ha="right")
    fig.tight_layout()
    fig.savefig(out_path, dpi=160)
    if close_fig:
        plt.close(fig)


def plot_3d_pairs(
    df: pd.DataFrame,
    out_path: str,
    close_fig: bool = True,
) -> None:
    pairs = [
        ("tabu_tenure", "max_iterations"),
        ("tabu_tenure", "no_improve_limit"),
        ("max_iterations", "no_improve_limit"),
    ]

    fig = plt.figure(figsize=(15, 5))
    for idx, (xcol, ycol) in enumerate(pairs, start=1):
        ax = fig.add_subplot(1, 3, idx, projection="3d")
        sc = ax.scatter(
            df[xcol],
            df[ycol],
            df[TARGET_COLUMN],
            c=df[TARGET_COLUMN],
            cmap="viridis",
            s=35,
            alpha=0.85,
        )
        ax.set_xlabel(xcol)
        ax.set_ylabel(ycol)
        ax.set_zlabel(TARGET_COLUMN)
        ax.set_title(f"{xcol} vs {ycol}")

    fig.suptitle("Parameter pairs vs avg_distance", fontsize=12, y=0.98)
    fig.tight_layout()
    fig.subplots_adjust(right=0.88)
    cbar_ax = fig.add_axes([0.9, 0.18, 0.02, 0.62])
    fig.colorbar(sc, cax=cbar_ax, label=TARGET_COLUMN)
    fig.savefig(out_path, dpi=160)
    if close_fig:
        plt.close(fig)


def summarize_best_by_param(df: pd.DataFrame) -> None:
    print("\nBest average values by parameter (lower avg_distance is better):\n")
    for col in PARAM_COLUMNS:
        grouped = df.groupby(col)[TARGET_COLUMN].mean().sort_values()
        best_val = grouped.index[0]
        best_avg = grouped.iloc[0]
        print(f"- {col}: best={best_val} (avg_distance={best_avg:.1f})")


def main() -> None:
    args = parse_args()
    df = load_data(args.csv_path)
    ensure_out_dir(args.out_dir)

    print_top_combinations(df, args.top)
    summarize_best_by_param(df)
    pearson, spearman = correlation_analysis(df)
    _, importances = fit_models(df)

    close_fig = not args.show
    plot_correlation_heatmap(
        pearson,
        os.path.join(args.out_dir, "correlation_heatmap.png"),
        close_fig=close_fig,
    )
    plot_feature_importance(
        importances,
        os.path.join(args.out_dir, "feature_importance.png"),
        close_fig=close_fig,
    )
    plot_3d_pairs(
        df,
        os.path.join(args.out_dir, "parameter_pairs_3d.png"),
        close_fig=close_fig,
    )

    print(f"\nPlots saved to: {args.out_dir}")

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
