import pandas as pd
import matplotlib.pyplot as plt
import os

summary = pd.read_csv("results/summary.csv")
os.makedirs("results/plots", exist_ok=True)

for data_type in summary["data_type"].unique():
    sub = summary[summary["data_type"] == data_type]

    std = sub[sub["algorithm"] == "merge_std"]
    plt.figure(figsize=(10,6))
    plt.plot(std["n"], std["mean_ms"], label="Standard Merge Sort")
    plt.title(f"{data_type} — Standard Merge Sort")
    plt.xlabel("Array size n")
    plt.ylabel("Time (ms)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"results/plots/{data_type}_merge_std.png")
    plt.close()

    hybrid = sub[sub["algorithm"] == "merge_hybrid"]
    plt.figure(figsize=(10,6))
    for thr in sorted(hybrid["threshold"].unique()):
        plt.plot(hybrid[hybrid["threshold"] == thr]["n"],
                 hybrid[hybrid["threshold"] == thr]["mean_ms"],
                 label=f"threshold={thr}")
    plt.title(f"{data_type} — Hybrid Merge+Insertion Sort")
    plt.xlabel("Array size n")
    plt.ylabel("Time (ms)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"results/plots/{data_type}_merge_hybrid.png")
    plt.close()

    best = hybrid.groupby("n").apply(lambda g: g.loc[g["mean_ms"].idxmin()])
    plt.figure(figsize=(10,6))
    plt.plot(std["n"], std["mean_ms"], label="Standard Merge Sort")
    plt.plot(best["n"], best["mean_ms"], label="Best Hybrid")
    plt.title(f"{data_type} — Comparison: Standard vs Best Hybrid")
    plt.xlabel("Array size n")
    plt.ylabel("Time (ms)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"results/plots/{data_type}_comparison.png")
    plt.close()

print("✅ Графики сохранены в папке results/plots/")
