from dataclasses import dataclass
import json, os
import numpy as np
import matplotlib.pyplot as plt

BASE_PATH = os.path.dirname(os.path.abspath(__file__))
RESULTS_PATH = os.path.join(BASE_PATH, "results")
FIGURES_PATH = os.path.join(BASE_PATH, "figures")

@dataclass
class Result:
    name: str
    mean: int
    min_values: list[int]
    groups: int
    samples_per_group: int

def load_results(file_name) -> Result:
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
        return Result(**json.load(f))

def plot_results(results: list[Result], save: bool = True) -> None:
    """
    Results is a pack of the same subject with different groups and samples per group.
    Plots a scatter plot of the results, with group number as the x-axis
    and the mean + min values as the y-axis
    """

    if save and not os.path.exists(FIGURES_PATH):
        os.makedirs(FIGURES_PATH)

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

def main():
    result_files = [f for f in os.listdir(RESULTS_PATH) if f.endswith(".json")]
    results = [load_results(file) for file in result_files]
    plot_results(results)

if __name__ == "__main__":
    main()