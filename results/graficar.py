"""Genera results/grafica_tiempos.png y results/grafica_umbral.png a partir de los CSV.
Uso (desde la raiz del repo): python3 results/graficar.py   (requiere matplotlib)"""
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

n, c, k, h = [], [], [], []
for r in csv.DictReader(open("results/tiempos.csv")):
    n.append(int(r["n"])); c.append(float(r["clasica_us"]))
    k.append(float(r["karatsuba_us"]) if r["karatsuba_us"] else None)
    h.append(float(r["karatsuba_hibrido_us"]))

fig, ax = plt.subplots(figsize=(7, 4.5))
ax.loglog(n, c, "o-", label="Clásica Θ(n²)")
nk = [x for x, y in zip(n, k) if y]; kk = [y for y in k if y]
ax.loglog(nk, kk, "s-", label="Karatsuba puro (caso base 1 dígito)")
ax.loglog(n, h, "^-", label="Karatsuba híbrido (caso base 128 dígitos)")
ax.axvline(4096, color="gray", ls=":", lw=1); ax.text(4300, min(c) * 3, "fin del rango\norientativo", fontsize=8, color="gray")
ax.set_xlabel("Tamaño n (dígitos)"); ax.set_ylabel("Tiempo por multiplicación (µs)")
ax.set_title("Tiempo vs. tamaño (escala log-log)"); ax.grid(True, which="both", alpha=0.3); ax.legend(fontsize=8)
fig.tight_layout(); fig.savefig("results/grafica_tiempos.png", dpi=150)

u, t = [], []
for r in csv.DictReader(open("results/umbral.csv")):
    u.append(int(r["umbral"])); t.append(float(r["karatsuba_4096_us"]))
fig, ax = plt.subplots(figsize=(6, 4))
ax.loglog(u, t, "o-"); ax.set_xlabel("Umbral del caso base (dígitos)"); ax.set_ylabel("Tiempo para n = 4096 (µs)")
ax.set_title("Efecto del caso base en Karatsuba (n = 4096)"); ax.grid(True, which="both", alpha=0.3)
fig.tight_layout(); fig.savefig("results/grafica_umbral.png", dpi=150)
