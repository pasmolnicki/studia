import numpy as np
import matplotlib.pyplot as plt


def log_likelihood_exp(lam: np.ndarray, sample: np.ndarray) -> np.ndarray:
	n = sample.size
	s = sample.sum()
	return n * np.log(lam) - lam * s


def mle_lambda(sample: np.ndarray) -> float:
	return 1.0 / sample.mean()


def main() -> None:
	rng = np.random.default_rng(2026)

	lambda0 = 2.0
	n0 = 30
	lambda_grid = np.linspace(0.5, 4.0, 600)

	# (a) Proba n=30 dla lambda0=2
	sample_n30 = rng.exponential(scale=1.0 / lambda0, size=n0)
	print("(a) Wygenerowana proba (n=30, lambda0=2):")
	print(np.array2string(sample_n30, precision=4, separator=", "))
	print()

	# (b) Wykres log-wiarygodnosci i zaznaczenie estymatora MNW
	ll_n30 = log_likelihood_exp(lambda_grid, sample_n30)
	lambda_hat_n30 = mle_lambda(sample_n30)

	plt.figure(figsize=(8, 5))
	plt.plot(lambda_grid, ll_n30, label="log-wiarygodnosc l(lambda)", color="tab:blue")
	plt.axvline(
		x=lambda_hat_n30,
		color="tab:red",
		linestyle="--",
		label=f"MNW lambda_hat = {lambda_hat_n30:.4f}",
	)
	plt.title("(b) Log-wiarygodnosc dla n=30")
	plt.xlabel("lambda")
	plt.ylabel("l(lambda)")
	plt.xlim(0.5, 4.0)
	plt.grid(alpha=0.3)
	plt.legend()
	plt.tight_layout()
	plt.show()

	# (c) Krzywe l(lambda)/n dla n in {5, 30, 200}
	n_values = [5, 30, 200]
	plt.figure(figsize=(9, 5.5))
	for n in n_values:
		sample_n = rng.exponential(scale=1.0 / lambda0, size=n)
		ll_per_obs = log_likelihood_exp(lambda_grid, sample_n) / n
		plt.plot(lambda_grid, ll_per_obs, label=f"n={n}")

	plt.title("(c) Porownanie krzywych l(lambda)/n")
	plt.xlabel("lambda")
	plt.ylabel("l(lambda)/n")
	plt.xlim(0.5, 4.0)
	plt.grid(alpha=0.3)
	plt.legend()
	plt.tight_layout()
	plt.show()

	print("(c) Komentarz:")
	print(
		"Wraz ze wzrostem n krzywa l(lambda)/n staje sie gladsza i wyrazniej\n"
		"maksymalizuje sie blisko prawdziwego lambda0=2 (mniejszy wplyw losowych odchylen probki)."
	)
	print()

	# (d) Badanie obciazenia estymatora lambda_hat = 1 / X_srednie
	b = 1000
	print("(d) Symulacja obciazenia estymatora MNW (B=1000):")
	for n in n_values:
		lambda_hats = np.empty(b)
		for i in range(b):
			sample_n = rng.exponential(scale=1.0 / lambda0, size=n)
			lambda_hats[i] = mle_lambda(sample_n)

		mean_hat = lambda_hats.mean()
		bias = mean_hat - lambda0
		print(
			f"n={n:>3}: srednia(lambda_hat)={mean_hat:.4f}, "
			f"obciazenie={bias:.4f}"
		)

	print()
	print("Wniosek: wraz ze wzrostem n obciazenie maleje i estymator zbliza sie srednio do lambda0=2.")


if __name__ == "__main__":
	main()
