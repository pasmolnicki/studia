import numpy as np
import matplotlib.pyplot as plt
import statsmodels.api as sm

# a) Generowanie danych
rng = np.random.default_rng(seed=1)

x = rng.normal(0, 1, 100)

# b) generowanie eps
eps = rng.normal(0, 0.5, 100)

# c) generowanie y
y = -1 + 0.5 * x + eps

# d) scatterplot
plt.scatter(x, y)
plt.xlabel("x")
plt.ylabel("y")
plt.title("Scatterplot of x and y")
plt.show()

# e) Estymacja modelu regresji liniowej
X = sm.add_constant(x)

model = sm.OLS(y, X).fit()
print(model.summary())

# f) Add regression line and population line to scatterplot

plt.scatter(x, y, label="Data")

# Estimated regression line
x_sorted = np.sort(x)
y_hat = model.params[0] + model.params[1] * x_sorted

plt.plot(x_sorted, y_hat,
         color="red",
         label="Least Squares Line")

# True population regression line
y_true = -1 + 0.5 * x_sorted

plt.plot(x_sorted, y_true,
         color="green",
         linestyle="--",
         label="Population Line")

plt.xlabel("x")
plt.ylabel("y")
plt.legend()
plt.show()

# g) Add quadratic term and estimate model

X_poly = np.column_stack((x, x**2))
X_poly = sm.add_constant(X_poly)

poly_model = sm.OLS(y, X_poly).fit()
print(poly_model.summary())

# h) less nosy data

eps_small = rng.normal(0, 0.1, 100)

y_small = -1 + 0.5 * x + eps_small

X_small = sm.add_constant(x)
model_small = sm.OLS(y_small, X_small).fit()

print(model_small.summary())

# (i)

eps_large = rng.normal(0, 1, 100)

y_large = -1 + 0.5 * x + eps_large

X_large = sm.add_constant(x)
model_large = sm.OLS(y_large, X_large).fit()

print(model_large.summary())


# j confidence intervals
print(model.conf_int())
print(model_small.conf_int())
print(model_large.conf_int())