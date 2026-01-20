import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import typing
import argparse

VecFunc = typing.Callable[[np.ndarray, pd.Series], np.ndarray]

NETWORK_OUTPUTS_FOLDER = 'network'
INSERTION_OUTPUTS_FOLDER = 'insertion-sort'
OUTPUT_NET_FMT = 'network_{}_{}.csv'
OUTPUT_INS_FMT = 'outputs_{}.csv'
FIG_OUTPUTS = 'figures'
N_SIMULATIONS = 100

def load_network_outputs(sim_index: int, p: float) -> pd.DataFrame:
    file_path = os.path.join(NETWORK_OUTPUTS_FOLDER, OUTPUT_NET_FMT.format(sim_index, p))
    return pd.read_csv(file_path, sep=';', dtype={'n' : int, 'p': float, 'trials': int})

def load_insertion_outputs(sim_index: int) -> pd.DataFrame:
    file_path = os.path.join(INSERTION_OUTPUTS_FOLDER, OUTPUT_INS_FMT.format(sim_index))
    return pd.read_csv(file_path, sep=';', dtype={'n' : int, 'comparisons': int, 'n_swaps': int})

def aggregate_data(fn, **kwargs) -> pd.DataFrame:
    all_data = []
    for k in range(1, N_SIMULATIONS + 1):
        all_data.append(fn(k, **kwargs))
    return pd.concat(all_data, ignore_index=True)

def plot_with_mean(
        aggregated: pd.DataFrame, field: str, title: str, ylabel: str,
        plot=True, save_fig: typing.Optional[str] = None):
    fig, ax = plt.subplots(figsize=(10, 6))
    ns = aggregated['n']
    mean_values = aggregated.groupby('n')[field].mean()

    # All scatter points
    ax.scatter(ns, aggregated[field], color="#ED9A35", 
                s=12, alpha=0.9, label='Wyniki symulacji')

    # Mean values
    ax.scatter(ns.unique(), mean_values, color="#075891",
                s=24, alpha=0.9, label=f'Średnia {ylabel}')

    ax.set_xscale('linear')
    ax.set_yscale('linear')
    ax.grid(True)

    if save_fig:
        fig.savefig(os.path.join(FIG_OUTPUTS, save_fig), dpi=400)
    if plot:
        plt.show()

def plot_asymptotic_behavior(
        aggregated: pd.DataFrame, field: str,
        func: VecFunc, best_fit_degree: int = 1,
        plot=True, save_fig: typing.Optional[str] = None):

    fig, ax = plt.subplots(figsize=(10, 6))
    ns = aggregated['n']
    mean_values = aggregated.groupby('n')[field].mean()

    # Plot asymptotic behavior
    n_vals = np.array(sorted(ns.unique()))
    asymptotic_vals = func(n_vals, mean_values)
    best_fit = np.polyfit(n_vals, asymptotic_vals, best_fit_degree)
    fitted_vals = np.polyval(best_fit, n_vals)

    ax.plot(n_vals, fitted_vals, color="#075891",
             linestyle='-', linewidth=2)
    ax.scatter(n_vals, asymptotic_vals, s=24, color="#ED9A35")

    ax.set_xscale('linear')
    ax.set_yscale('linear')
    ax.grid(True)
    if plot:
        plt.show()
    if save_fig:
        fig.savefig(os.path.join(FIG_OUTPUTS, save_fig), dpi=400)

def plot_network_sim(plot: bool = False, save_fig: typing.Optional[str] = None):
    data = aggregate_data(load_network_outputs, p=0.1)
    plot_with_mean(data, 'trials', '', 'Trials', plot=plot, 
                   save_fig=f'{save_fig}-p-0-1.png' if save_fig else None)
    data = aggregate_data(load_network_outputs, p=0.5)
    plot_with_mean(data, 'trials', '', 'Trials', plot=plot,
                   save_fig=f'{save_fig}-p-0-5.png' if save_fig else None)

def plot_insertion_sim(plot: bool = False, save_fig: typing.Optional[str] = None):
    data = aggregate_data(load_insertion_outputs)
    plot_with_mean(data, 'comparisons', '', 'Comparsions', plot=plot, save_fig=save_fig)
    plot_with_mean(data, 'n_swaps', '', '# swaps', plot=plot, 
                   save_fig=f'{save_fig}-swap.png' if save_fig else None)

    plot_asymptotic_behavior(data, 'comparisons', 
                             lambda n, v: v.to_numpy() / n, plot=plot, 
                             save_fig=f'{save_fig}-cmp(n)-n.png' if save_fig else None)

    plot_asymptotic_behavior(data, 'comparisons', 
                             lambda n, v: v.to_numpy() / n ** 2, plot=plot, 
                             save_fig=f'{save_fig}-cmp(n)-n2.png' if save_fig else None)
    
    plot_asymptotic_behavior(data, 'n_swaps', 
                             lambda n, v: v.to_numpy() / n, plot=plot, 
                             save_fig=f'{save_fig}-s(n)-n.png'if save_fig else None)

    plot_asymptotic_behavior(data, 'n_swaps', 
                             lambda n, v: v.to_numpy() / n ** 2, plot=plot, 
                             save_fig=f'{save_fig}-s(n)-n2.png' if save_fig else None)

def main():
    if not os.path.exists(NETWORK_OUTPUTS_FOLDER) or not os.path.exists(INSERTION_OUTPUTS_FOLDER):
        print(f'Missing .csv results: {NETWORK_OUTPUTS_FOLDER if os.path.exists(INSERTION_OUTPUTS_FOLDER) else INSERTION_OUTPUTS_FOLDER}')
        return

    if not os.path.exists(FIG_OUTPUTS):
        os.makedirs(FIG_OUTPUTS)
        print(f'Created figure directory: {FIG_OUTPUTS}')

    plot_insertion_sim(save_fig='ins')
    plot_network_sim(save_fig='net')

if __name__ == '__main__':
    main()