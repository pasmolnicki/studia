import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import typing
import argparse

VecFunc = typing.Callable[[np.ndarray, pd.Series], np.ndarray]

# Configuration
SIM_OUTPUTS = "outputs"
SIM_REPETITIONS = 50
FIG_OUTPUTS = "figures"

FUNC_MAPPINGS = {
    'u': 'n_empty_bins_after_n',
    'b': 'first_collision',
    'c': 'min_balls_no_empty_bin',
    'd': 'min_balls_each_2_in_bin',
    'd-c': 'n_balls_from_1_to_2',
    'l': 'max_load_d_after_n',
}

def load_simulation_data(sim_index: int) -> pd.DataFrame:
    file_path = os.path.join(SIM_OUTPUTS, f"outputs{sim_index}.csv")
    return pd.read_csv(file_path, sep=';', dtype={'n_bins': int, 'first_collision': int, 'n_empty_bins_after_n': int,
                                                  'min_balls_no_empty_bin': int, 'min_balls_each_2_in_bin': int,
                                                  'n_balls_from_1_to_2': int,
                                                  'max_load_d_after_n': int})


def plot_with_mean(
        aggregated: pd.DataFrame, field: str, title: str, ylabel: str,
        plot=True, save_fig: typing.Optional[str] = None):
    fig, ax = plt.subplots(figsize=(10, 6))
    ns = aggregated['n_bins']
    mean_values = aggregated.groupby('n_bins')[field].mean()

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
        aggregated: pd.DataFrame, field: str, title: str, 
        ylabel: str, func: VecFunc, label: str, best_fit_degree: int = 1,
        plot=True, save_fig: typing.Optional[str] = None):

    fig, ax = plt.subplots(figsize=(10, 6))
    ns = aggregated['n_bins']
    mean_values = aggregated.groupby('n_bins')[field].mean()

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

def aggregate_simulation_data() -> pd.DataFrame:
    all_data = []
    for k in range(1, SIM_REPETITIONS + 1):
        sim_data = load_simulation_data(k)
        all_data.append(sim_data)
    return pd.concat(all_data, ignore_index=True)

def plot_first_collision(aggregated_data: pd.DataFrame, plot: bool = True, save_fig: typing.Optional[str] = None):
    plot_with_mean(aggregated_data, 'first_collision', '', 'Pierwsza kolizja', plot=plot, save_fig=save_fig)
    plot_asymptotic_behavior(
        aggregated_data, 'first_collision', '', 'Wartość asymptotyczna pierwszej kolizji / √n',
        func=lambda n, v: v.to_numpy() / np.sqrt(n), label='b(n)/√n', plot=plot, save_fig=
            f'{save_fig}-b(n)-sq(n).png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'first_collision', '', 'Wartość asymptotyczna pierwszej kolizji / √n',
        func=lambda n, v: v.to_numpy() / n, label='b(n)/n', best_fit_degree=1, plot=plot, save_fig=
            f'{save_fig}-b(n)-n.png' if save_fig else None
    )

def plot_empty_bins(aggregated_data: pd.DataFrame, plot: bool = True, save_fig: typing.Optional[str] = None):
    plot_with_mean(aggregated_data, 'n_empty_bins_after_n', '',
                   'Liczba pustych urn po wrzuceniu n kul', plot=plot, save_fig=save_fig)
    plot_asymptotic_behavior(
        aggregated_data, 'n_empty_bins_after_n', '', 'u(n) / n',
        func=lambda n, v: v.to_numpy() / n, label='u(n)/n', best_fit_degree=1, 
        plot=plot, save_fig=f'{save_fig}-u(n)-n.png' if save_fig else None
    )

def plot_min_balls_no_empty_bin(aggregated_data: pd.DataFrame, plot: bool = True, save_fig: typing.Optional[str] = None):
    plot_with_mean(aggregated_data, 'min_balls_no_empty_bin', '',
                   'Minimalna liczba kul bez pustej urny', plot=plot, save_fig=save_fig)
    plot_asymptotic_behavior(
        aggregated_data, 'min_balls_no_empty_bin', '', 'c(n) / n',
        func=lambda n, v: v.to_numpy() / n, label='c(n)/n', 
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-c(n)-n.png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'min_balls_no_empty_bin', '', 'c(n) / nln(n)',
        func=lambda n, v: v.to_numpy() / (n * np.log(n)), label='c(n)/nln(n)', 
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-c(n)-nln(n).png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'min_balls_no_empty_bin', '', 'c(n) / n^2',
        func=lambda n, v: v.to_numpy() / (n ** 2), label='c(n)/n^2', 
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-c(n)-n2.png' if save_fig else None
    )

def plot_min_balls_each_2_in_bin(aggregated_data: pd.DataFrame, plot: bool = True, save_fig: typing.Optional[str] = None):
    plot_with_mean(aggregated_data, 'min_balls_each_2_in_bin', '', 
                   'Minimalna liczba kul, aby każda urna miała co najmniej 2 kule', plot=plot, save_fig=save_fig)
    plot_asymptotic_behavior(
        aggregated_data, 'min_balls_each_2_in_bin', '', 'd(n)/n',
        func=lambda n, v: v.to_numpy() / n, label='d(n)/n',
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-d(n)-n.png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'min_balls_each_2_in_bin', '', 'd(n)/nln(n)',
        func=lambda n, v: v.to_numpy() / (n * np.log(n)), label='d(n)/nln(n)',
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-d(n)-nln(n).png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'min_balls_each_2_in_bin', '', 'd(n)/n^2',
        func=lambda n, v: v.to_numpy() / (n ** 2), label='d(n)/n^2',
        best_fit_degree=1, plot=plot, 
        save_fig=f'{save_fig}-d(n)-n2.png' if save_fig else None
    )

def plot_n_balls_from_1_to_2(aggregated_data: pd.DataFrame, plot: bool = True, save_fig: typing.Optional[str] = None):
    plot_with_mean(aggregated_data, 'n_balls_from_1_to_2', '', 'Liczba kul od 1 do 2 w każdej urnie',
                   plot=plot, save_fig=save_fig)
    plot_asymptotic_behavior(
        aggregated_data, 'n_balls_from_1_to_2', '', 'r(n) / n',
        func=lambda n, v: v.to_numpy() / n, label='r(n)/n', 
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-r(n)-n.png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'n_balls_from_1_to_2', '', 'r(n) / nln(n)',
        func=lambda n, v: v.to_numpy() / (n * np.log(n)), label='r(n)/nln(n)',
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-r(n)-nln(n).png' if save_fig else None
    )
    plot_asymptotic_behavior(
        aggregated_data, 'n_balls_from_1_to_2', '', '(d(n) - c(n))/ nln(lnn)',
        func=lambda n, v: v.to_numpy() / (n * np.log(np.log(n))), label='(d(n) - c(n))/nln(lnn)',
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-r(n)-nln(lnn).png' if save_fig else None
    )

def plot_max_load_d_after_n(aggregated_data: pd.DataFrame, plot: bool = True, save_fig: typing.Optional[str] = None, **kwargs):
    d = kwargs.get('d', 2)
    plot_with_mean(aggregated_data, 'max_load_d_after_n', '', f'Maksymalne obciążenie przy d={d} po wrzuceniu n kul',
                   plot=plot, save_fig=f'{save_fig}-{d}.png' if save_fig else None)
    
    # L(n) / f1(n)
    # f1(n) = ln(n) / ln(ln(n))
    plot_asymptotic_behavior(
        aggregated_data, 'max_load_d_after_n', '', 'L(n) / (ln(n) / ln(ln(n)))',
        func=lambda n, v: v.to_numpy() / (np.log(n) / np.log(np.log(n))), label='L(n) / (ln(n) / ln(ln(n)))',
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-{d}-L(n)ln(ln(n))-ln(n).png' if save_fig else None
    )
    # L(n) / f2(n)
    # f2(n) = ln(ln(n)) / ln(2)
    plot_asymptotic_behavior(
        aggregated_data, 'max_load_d_after_n', '', 'L(n) / (ln(ln(n)) / ln(2))',
        func=lambda n, v: v.to_numpy() / (np.log(np.log(n)) / np.log(2)), label='L(n) / (ln(ln(n)) / ln(2))',
        best_fit_degree=1, plot=plot, save_fig=f'{save_fig}-{d}-L(n)ln(2)-ln(ln(n)).png' if save_fig else None
    )

def parse_options() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Parse simulation results and generate plots.")
    parser.add_argument('--plot', action='store_true', default=False, help='Display plots.')
    parser.add_argument('--no-saving', action='store_true', default=False, help='Do not save figures to output directory.')
    parser.add_argument('--func', '-f', type=str, choices=FUNC_MAPPINGS.keys(), default=None,
                        help='Function to plot (u, b, c, d, d-c). If not provided, all functions will be plotted.')
    parser.add_argument('--d', type=int, default=2, help='Value of d for max_load_d_after_n function (default: 2).')
    return parser.parse_args()

def main():
    if not os.path.exists(SIM_OUTPUTS):
        print(f"Output directory '{SIM_OUTPUTS}' does not exist. Please run the C++ simulations first.")
        return
    
    if not os.path.exists(FIG_OUTPUTS):
        os.makedirs(FIG_OUTPUTS)
        print(f"Created figure output directory '{FIG_OUTPUTS}'.")
    
    args = parse_options()
    plot_flag = args.plot
    save_fig_flag = not args.no_saving
    func_to_plot = [FUNC_MAPPINGS.get(args.func, '')] if args.func else list(FUNC_MAPPINGS.values())

    if not args.plot and args.no_saving:
        print("No action specified (neither plotting nor saving figures). Exiting.")
        return

    aggregated_data = aggregate_simulation_data()
    for func in func_to_plot:
        {
            'first_collision': plot_first_collision,
            'n_empty_bins_after_n': plot_empty_bins,
            'min_balls_no_empty_bin': plot_min_balls_no_empty_bin,
            'min_balls_each_2_in_bin': plot_min_balls_each_2_in_bin,
            'n_balls_from_1_to_2': plot_n_balls_from_1_to_2,
            'max_load_d_after_n': plot_max_load_d_after_n,
        }.get(func, plot_first_collision)(aggregated_data, plot=plot_flag, save_fig=
                                          func if save_fig_flag else None, **({'d': args.d} if func == 'max_load_d_after_n' else {}))
        print(f"Plotted function '{func}'")

if __name__ == "__main__":
    main()