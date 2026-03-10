import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import numpy.typing as tp

def gen_normal(avg: float = 700, sd: float = 100, N: int = 100):
    return np.random.normal(avg, sd, size=(N,)).astype(int)

def plot_histograms(arr: tp.NDArray, title: str = "Histogramy"):
    """Plot histograms with 10 and 30 bins"""
    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    
    axes[0].hist(arr, bins=10, edgecolor='black', alpha=0.7, color='skyblue')
    axes[0].set_title('Histogram - 10')
    axes[0].set_xlabel('Cena (tys. PLN)')
    axes[0].set_ylabel('F')
    axes[0].grid(alpha=0.3)
    
    axes[1].hist(arr, bins=30, edgecolor='black', alpha=0.7, color='lightcoral')
    axes[1].set_title('Histogram - 30')
    axes[1].set_xlabel('Cena (tys. PLN)')
    axes[1].set_ylabel('F')
    axes[1].grid(alpha=0.3)
    
    fig.suptitle(title, fontsize=14, fontweight='bold')
    plt.plot()

def plot_ecdf(arr: tp.NDArray, title: str = "Empiryczna dystrybuanta"):
    """Plot empirical CDF"""
    sorted_arr = np.sort(arr)
    ecdf = np.arange(1, len(arr) + 1) / len(arr)
    
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(sorted_arr, ecdf, marker='.', linestyle='none', markersize=6, alpha=0.7)
    ax.set_xlabel('Cena (tys PLN)')
    ax.set_ylabel('Dystrybuanta')
    ax.set_title(title)
    ax.grid(alpha=0.3)
    plt.plot()

def main():    
    # Generate sample
    x = gen_normal(avg=700, sd=100, N=100)

    plot_histograms(x, "Ceny mieszkań")
    plot_ecdf(x)
    
    # Add outliers
    outliers = np.array([1500, 1500, 1500, 1500, 1500])
    x_with_outliers = np.concatenate([x, outliers])
    
    comparison_data = {
        'Statistic': ['Mean', 'SD', 'Median', 'Q1', 'Q3', 'IQR'],
        'Original': [
            f"{np.mean(x):.2f}",
            f"{np.std(x, ddof=1):.2f}",
            f"{np.median(x):.2f}",
            f"{np.quantile(x, q=0.25):.2f}",
            f"{np.quantile(x, q=0.75):.2f}",
            f"{np.quantile(x, q=0.75) - np.quantile(x, q=0.25):.2f}"
        ],
        'With Outliers': [
            f"{np.mean(x_with_outliers):.2f}",
            f"{np.std(x_with_outliers, ddof=1):.2f}",
            f"{np.median(x_with_outliers):.2f}",
            f"{np.quantile(x_with_outliers, q=0.25):.2f}",
            f"{np.quantile(x_with_outliers, q=0.75):.2f}",
            f"{np.quantile(x_with_outliers, q=0.75) - np.quantile(x_with_outliers, q=0.25):.2f}"
        ]
    }
    
    comparison_df = pd.DataFrame(comparison_data)
    print(comparison_df.to_string(index=False))
    
    # Plot comparison
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    
    # Histograms
    axes[0, 0].hist(x, bins=15, edgecolor='black', alpha=0.7, color='skyblue', label='Oryginał')
    axes[0, 0].set_title('Histogram')
    axes[0, 0].set_xlabel('Cena (tys. PLN)')
    axes[0, 0].set_ylabel('F')
    axes[0, 0].grid(alpha=0.3)
    
    axes[0, 1].hist(x_with_outliers, bins=15, edgecolor='black', alpha=0.7, color='lightcoral', label='outliers')
    axes[0, 1].set_title('Histogram - outliers')
    axes[0, 1].set_xlabel('Cena (tys. PLN)')
    axes[0, 1].set_ylabel('F')
    axes[0, 1].grid(alpha=0.3)
    
    # Box plots
    axes[1, 0].boxplot(x)
    axes[1, 0].set_ylabel('Cena (tys. PLN)')
    axes[1, 0].set_title('Wykres pudełkowy')
    axes[1, 0].grid(alpha=0.3)
    
    axes[1, 1].boxplot(x_with_outliers)
    axes[1, 1].set_ylabel('Cena (tys. PLN)')
    axes[1, 1].set_title('Wykres pudełkowy - outliers')
    axes[1, 1].grid(alpha=0.3)
    
    fig.suptitle('Oryginał vs outliers', fontsize=14, fontweight='bold')
    # plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    main()