import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import t


def ci_mean_unknown_sigma(mu: float, sigma: float, n: int, rng: np.random.Generator):
	"""Generuje probe z N(mu, sigma^2), buduje 95% CI dla mu przy nieznanym sigma.

	Zwraca:
	- przedzial (dolna, gorna)
	- czy przedzial pokrywa prawdziwe mu
	- estymator odchylenia standardowego S
	"""
	x = rng.normal(loc=mu, scale=sigma, size=n)
	x_bar = x.mean()
	s = x.std(ddof=1)
	t_crit = t.ppf(0.975, df=n - 1)
	margin = t_crit * s / np.sqrt(n)
	ci = (x_bar - margin, x_bar + margin)
	covers = ci[0] <= mu <= ci[1]
	return ci, covers, s


def main() -> None:
	rng = np.random.default_rng(2026)

	mu = 260.0
	sigma = 18.0

	# ---------- (a) ----------
	n = 9
	b = 1000

	cover_count = 0
	widths_unknown = np.empty(b)
	for i in range(b):
		ci, covers, _ = ci_mean_unknown_sigma(mu=mu, sigma=sigma, n=n, rng=rng)
		cover_count += int(covers)
		widths_unknown[i] = ci[1] - ci[0]

	empirical_coverage = cover_count / b
	avg_width_unknown = widths_unknown.mean()

	z_975 = 1.959963984540054
	width_known_sigma = 2.0 * z_975 * sigma / np.sqrt(n)

	print("(a) n=9, B=1000")
	print(f"Empiryczna czestosc pokrycia: {empirical_coverage:.4f} (cel: 0.95)")
	print(f"Srednia szerokosc CI (sigma nieznane, t): {avg_width_unknown:.4f}")
	print(f"Szerokosc CI (sigma znane, z): {width_known_sigma:.4f}")
	print()

	# ---------- (b) ----------
	m = 100
	intervals = []
	covers_list = []

	for _ in range(m):
		ci, covers, _ = ci_mean_unknown_sigma(mu=mu, sigma=sigma, n=n, rng=rng)
		intervals.append(ci)
		covers_list.append(covers)

	red_percent = 100.0 * (1.0 - np.mean(covers_list))
	print("(b) Wizualizacja 100 przedzialow")
	print(f"Procent czerwonych odcinkow (bez pokrycia mu): {red_percent:.2f}%")
	print()

	y = np.arange(1, m + 1)
	plt.figure(figsize=(10, 8))
	for i, ((lo, hi), covers) in enumerate(zip(intervals, covers_list), start=1):
		color = "tab:blue" if covers else "tab:red"
		plt.hlines(y=i, xmin=lo, xmax=hi, color=color, linewidth=2)

	plt.axvline(mu, color="black", linestyle="--", linewidth=1.5, label=f"mu = {mu:.0f}")
	plt.title("(b) 100 przedzialow ufnosci 95% dla sredniej")
	plt.xlabel("wartosc")
	plt.ylabel("numer przedzialu")
	plt.grid(alpha=0.25)
	plt.legend()
	plt.tight_layout()
	plt.show()

	# ---------- (c) ----------
	n_values = [5, 9, 30, 100]
	print("(c) Porownanie dla roznych n (B=1000)")
	for n_curr in n_values:
		cover_count = 0
		widths = np.empty(b)
		for i in range(b):
			ci, covers, _ = ci_mean_unknown_sigma(mu=mu, sigma=sigma, n=n_curr, rng=rng)
			cover_count += int(covers)
			widths[i] = ci[1] - ci[0]

		coverage = cover_count / b
		mean_width = widths.mean()
		print(
			f"n={n_curr:>3} | empiryczna czestosc pokrycia={coverage:.4f} | "
			f"srednia szerokosc={mean_width:.4f}"
		)


if __name__ == "__main__":
	main()
