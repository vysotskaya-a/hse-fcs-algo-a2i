#!/usr/bin/env python3
import csv
import sys
import math
from collections import defaultdict
import matplotlib.pyplot as plt

# Usage: python3 plot_results.py results.csv
if len(sys.argv) < 2:
    print("Usage: python3 plot_results.py results.csv")
    sys.exit(1)

fn = sys.argv[1]

# CSV columns:
# array_type,n,algorithm,threshold,mean_usec,median_usec,runs_usec
data = []
with open(fn, newline='') as f:
    reader = csv.DictReader(f)
    for row in reader:
        row['n'] = int(row['n'])
        row['mean_usec'] = float(row['mean_usec'])
        row['median_usec'] = float(row['median_usec'])
        row['threshold'] = None if row['threshold'] == 'NA' else int(row['threshold'])
        data.append(row)

# Group by array_type and algorithm/threshold
by_type = defaultdict(list)
for r in data:
    by_type[r['array_type']].append(r)

for arr_type, rows in by_type.items():
    # prepare series: standard and each hybrid threshold
    series = defaultdict(list)
    ns_set = set()
    for r in rows:
        key = "standard" if r['algorithm'] == 'standard' else f"hybrid_{r['threshold']}"
        series[key].append((r['n'], r['mean_usec']))
        ns_set.add(r['n'])
    ns = sorted(list(ns_set))

    plt.figure(figsize=(10,6))
    for k, points in series.items():
        points_sorted = sorted(points, key=lambda x: x[0])
        xs = [p[0] for p in points_sorted]
        ys = [p[1] / 1000.0 for p in points_sorted]  # ms
        plt.plot(xs, ys, label=k)
    plt.xlabel("n")
    plt.ylabel("mean time (ms)")
    plt.title(f"Тип массива: {arr_type}")
    plt.legend()
    plt.grid(True)
    out_png = f"plot_{arr_type}.png"
    plt.savefig(out_png, dpi=200)
    print("Wrote", out_png)
    plt.close()
