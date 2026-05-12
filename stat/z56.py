import numpy as np
import matplotlib.pyplot as plt
import statsmodels.api as sm

rng = np.random.default_rng(10)

x1 = rng.uniform(0, 1, size=100)
x2 = 0.5 * x1 + rng.normal(size=100) / 10
y = 2 + 2 * x1 + 0.3 * x2 + rng.normal(size=100)

corr = np.corrcoef(x1, x2)[0, 1]
print(f"Korelacja między x1 i x2: {corr:.4f}")

plt.figure(figsize=(8, 5))
plt.scatter(x1, x2, alpha=0.7)
plt.xlabel('x1')
plt.ylabel('x2')
plt.title('Zależność między x1 i x2')
plt.grid(True)
plt.show()


X = sm.add_constant(np.column_stack((x1, x2)))
model_c = sm.OLS(y, X).fit()
print(model_c.summary())


model_d = sm.OLS(y, sm.add_constant(x1)).fit()
print(model_d.summary())


model_e = sm.OLS(y, sm.add_constant(x2)).fit()
print(model_e.summary())


x1_new = np.concatenate([x1, [0.1]])
x2_new = np.concatenate([x2, [0.8]])
y_new = np.concatenate([y, [6]])

X_new = sm.add_constant(np.column_stack((x1_new, x2_new)))
model_c_new = sm.OLS(y_new, X_new).fit()
print("Model (c) z nową obserwacją:\n", model_c_new.summary())

model_d_new = sm.OLS(y_new, sm.add_constant(x1_new)).fit()
print("\nModel (d) z nową obserwacją:\n", model_d_new.summary())

model_e_new = sm.OLS(y_new, sm.add_constant(x2_new)).fit()
print("\nModel (e) z nową obserwacją:\n", model_e_new.summary())