import argparse
import os
import matplotlib.pyplot as plt
import pandas as pd


def analyze_ga_data(file_path):
    # 1. Load the data
    if not os.path.exists(file_path):
        print(f"Error: The file at '{file_path}' does not exist.")
        return

    print(f"Loading data from: {file_path}")
    df = pd.read_csv(file_path)

    # Clean column names just in case there are stray spaces
    df.columns = df.columns.str.strip()

    # --- TASK 1: Calculate and Visualize Correlations ---
    print("\n--- Calculating Correlations ---")
    # Convert categorical text data to numerical codes so we can run correlation on everything
    df_encoded = df.copy()
    for col in df_encoded.select_dtypes(include=["object"]).columns:
        df_encoded[col] = df_encoded[col].astype("category").cat.codes

    correlation_matrix = df_encoded.corr()
    print(correlation_matrix)

    # Plotting using matplotlib
    fig, ax = plt.subplots(figsize=(8, 6))
    cax = ax.matshow(correlation_matrix, cmap="coolwarm", vmin=-1, vmax=1)
    fig.colorbar(cax)

    # Set up labels
    columns_list = correlation_matrix.columns
    ticks = list(range(len(columns_list)))
    ax.set_xticks(ticks)
    ax.set_yticks(ticks)
    ax.set_xticklabels(columns_list, rotation=45, ha="left")
    ax.set_yticklabels(columns_list)

    plt.title("Correlation Matrix (Categorical Variables Encoded)", pad=30)
    plt.tight_layout()

    # --- TASK 2: Compare Crossover Types ---
    print("\n--- Crossover Type Performance Comparison ---")
    crossover_perf = df.groupby("crossover_type")["avg_distance"].mean()
    print(crossover_perf.to_string())

    best_crossover = crossover_perf.idxmin()
    print(
        f"\nResult: '{best_crossover}' yielded better results on average (lower average distance)."
    )

    # --- TASK 3: Find Best Overall Combination ---
    print("\n--- Best Possible Configuration (Minimum Avg Distance) ---")
    best_idx = df["avg_distance"].idxmin()
    best_config = df.loc[best_idx]

    for param, val in best_config.items():
        print(f"{param}: {val}")

    # Show the correlation plot at the very end
    plt.show()


if __name__ == "__main__":
    # Handle the relative default path based on __file__
    current_dir = os.path.dirname(os.path.abspath(__file__))
    default_csv_path = os.path.join(
        current_dir, "..", "results", "ga_parameter_tuning.csv"
    )

    # Set up argument parsing to allow manual file overrides
    parser = argparse.ArgumentParser(
        description="Analyze Genetic Algorithm tuning results."
    )
    parser.add_argument(
        "file",
        nargs="?",
        default=default_csv_path,
        help="Path to the ga_parameter_tuning.csv file (default: relative to script location)",
    )

    args = parser.parse_args()
    analyze_ga_data(args.file)
