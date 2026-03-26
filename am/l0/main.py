from dataclasses import dataclass
import json, os
import numpy as np
import matplotlib.pyplot as plt

BASE_PATH = os.path.dirname(os.path.abspath(__file__))
RESULTS_PATH = os.path.join(BASE_PATH, "results")
FIGURES_PATH = os.path.join(BASE_PATH, "figures")

@dataclass
class ExperimentResult:
    name: str
    mean: int
    min_values: list[int]
    groups: int
    samples_per_group: int

@dataclass
class SolutionResult:
    points: list[(float, float)]
    name: str

def load_results(file_name: str) -> ExperimentResult:
    """
    Returns json:
    {
        "name": string,
        "mean": float,
        "min_values": list of floats,
        "groups": int,
        "samples_per_group": int
    }

    Example:
    {"name":"dj38-1-1000","mean":21272,"min_values":[21272],"groups":1,"samples_per_group":1000}
    """
    with open(os.path.join(RESULTS_PATH, file_name), "r") as f:
        return ExperimentResult(**json.load(f))

def load_solution(file_name: str) -> SolutionResult:
    """
    Expects a file with json:
    {
        "points": [[float, float], ...],
        "name": str
    }
    """
    with open(os.path.join(RESULTS_PATH, file_name), "r") as f:
        return SolutionResult(**json.load(f))

def assert_figures_path():
    if not os.path.exists(FIGURES_PATH):
        os.makedirs(FIGURES_PATH)

def plot_results(results: list[ExperimentResult], save: bool = True) -> None:
    """
    Results is a pack of the same subject with different groups and samples per group.
    Plots a scatter plot of the results, with group number as the x-axis
    and the mean + min values as the y-axis
    """

    if save:
        assert_figures_path()
    
    from collections import defaultdict
    
    # Group results by dataset prefix (e.g., "dj38" from "dj38-1-1000")
    grouped = defaultdict(list)
    for result in results:
        prefix = result.name.rsplit('-', 2)[0]
        grouped[prefix].append(result)
    
    # Create a figure for each dataset
    for prefix in sorted(grouped.keys()):
        result_list = sorted(grouped[prefix], key=lambda r: r.groups)
        
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))

        if not save:
            fig.suptitle(f"TSP Results for {prefix}", fontsize=14, fontweight='bold')
        
        for idx, result in enumerate(result_list):
            ax = axes[idx]
            
            # Plot scatter plot of min_values with group indices
            group_indices = np.arange(len(result.min_values))
            ax.scatter(group_indices, result.min_values, alpha=0.6, s=50)
            
            # Plot mean line
            ax.axhline(y=result.mean, color='red', linestyle='--', linewidth=2, 
                      label=f'Mean: {result.mean}')
            
            ax.set_xlabel('Group Index')
            ax.set_ylabel('Min Value')
            ax.set_title(f'Groups: {result.groups}, Samples/Group: {result.samples_per_group}')
            ax.legend()
            ax.grid(True, alpha=0.3)
        

        plt.tight_layout()

        if save:
            fig.savefig(os.path.join(FIGURES_PATH, f"{prefix}_results.png"), dpi=300)
        else:
            plt.show()

def plot_solutions(results: list[SolutionResult], save: bool = True) -> None:
    if save:
        assert_figures_path()

    """
    Creates a 2D plane plot of the points, with connected lines between them, and saves the plot as a PNG file with the name of the dataset.
    """

    for result in results:
        points = result.points
        x, y = zip(*points)

        fig = plt.figure(figsize=(8, 8))
        plt.plot(x, y, marker='o', linestyle='-', color='blue')
        plt.title(result.name, fontsize=14, fontweight='bold')
        plt.xlabel('X Coordinate')
        plt.ylabel('Y Coordinate')
        plt.grid(True, alpha=0.3)
        
        if save:
            fig.savefig(os.path.join(FIGURES_PATH, f"{result.name}.png"), dpi=300)
        else:
            plt.show()


def main():
    all_files = [f for f in os.listdir(RESULTS_PATH) if f.endswith('.json')]
    data_files = [f for f in all_files if f.split('-')[1] != 'solution.json']
    solution_files = [f for f in all_files if f.split('-')[1] == 'solution.json']
    results = [load_results(file) for file in data_files]
    solutions = [load_solution(file) for file in solution_files]
    plot_results(results)
    plot_solutions(solutions)

if __name__ == "__main__":
    main()