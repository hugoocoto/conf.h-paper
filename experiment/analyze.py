import pandas as pd
import matplotlib.pyplot as plt

nest = pd.read_csv("nesting.csv")
loop = pd.read_csv("loop.csv")

nest_perf = nest[nest.type == "perf"].copy()
nest_func = nest[nest.type == "function"]
loop_perf = loop[loop.type == "perf"].copy()
loop_func = loop[loop.type == "function"]

# Only measurement regions (exclude Open/Close)
nest_meas = nest_perf[nest_perf.region.str.startswith("Get Int:")].copy()
nest_meas["depth"] = nest_meas["region"].str.lower().str.count("table")

# per-access time: total thread_ms / number of iterations
nest_meas["per_access"] = nest_meas["thread_ms"] / nest_meas["iters"]
loop_perf["per_access"] = loop_perf["thread_ms"] / loop_perf["iters"]

def trimmed_mean(s, lower=0.01, upper=0.99):
    if len(s) < 10:
        return s.mean()
    lo = s.quantile(lower)
    hi = s.quantile(upper)
    return s[(s >= lo) & (s <= hi)].mean()

def trimmed_std(s, lower=0.01, upper=0.99):
    if len(s) < 10:
        return s.std()
    lo = s.quantile(lower)
    hi = s.quantile(upper)
    return s[(s >= lo) & (s <= hi)].std()

# ========== 1. Nesting depth plot ==========
plt.figure(1, figsize=(8, 4))
for lib in ["confh", "lua"]:
    sub = nest_meas[nest_meas.lib == lib].groupby("depth")["per_access"]
    depths = sub.apply(trimmed_mean).index
    means = sub.apply(trimmed_mean).values
    stds = sub.apply(trimmed_std).values
    plt.errorbar(depths, means, yerr=stds, marker="o", capsize=3, label=lib)
plt.xlabel("Nesting depth")
plt.ylabel("Per-access thread time (ms)")
plt.title("Access time vs nesting depth")
plt.legend()
plt.grid(ls=":")
plt.tight_layout()
plt.savefig("nesting.png")
print("Saved nesting.png")

# ========== 2. Loop: cached vs uncached ==========
plt.figure(2, figsize=(6, 4))
sub = loop_perf[loop_perf.region.str.contains("%d", na=False)]
bars = sub.groupby("lib")["per_access"].agg([trimmed_mean, trimmed_std])
plt.bar(bars.index, bars["trimmed_mean"], yerr=bars["trimmed_std"], capsize=5, width=0.4)
plt.ylabel("Per-access thread time (ms)")
plt.title("Deep-table access: re-traverse vs cached ref")
plt.tight_layout()
plt.savefig("loop.png")
print("Saved loop.png")

# ========== 3. Function-level summary ==========
print("\n=== Function summary: total CPU / faults ===")
summary = pd.concat([nest_func, loop_func])
for _, row in summary.iterrows():
    print(f"  {row.lib:6s}  {row.test_group:8s}  "
          f"trial {int(row.trial):2d}  "
          f"CPU {float(row.total_cpu_ms):8.3f}ms  "
          f"soft {int(row.soft_faults):4d}  "
          f"hard {int(row.hard_faults):4d}")

# ========== 4. Tables (trimmed stats) ==========
print("\n=== Nesting: trimmed mean ± std (per-access ms) ===")
nest_stat = nest_meas.groupby(["lib", "depth"])["per_access"].agg([trimmed_mean, trimmed_std])
nest_stat.columns = ["mean", "std"]
nest_stat["fmt"] = nest_stat.apply(lambda r: f"{r['mean']:.4f} ± {r['std']:.4f}", axis=1)
print(nest_stat["fmt"].unstack(0).to_string())

print("\n=== Loop: trimmed mean ± std (per-access ms) ===")
loop_stat = loop_perf[loop_perf.region.str.contains("%d", na=False)] \
    .groupby("lib")["per_access"].agg([trimmed_mean, trimmed_std])
loop_stat.columns = ["mean", "std"]
print(loop_stat.apply(lambda r: f"{r['mean']:.4f} ± {r['std']:.4f}", axis=1).to_string())

c = loop_stat.loc["confh", "mean"]
l = loop_stat.loc["lua", "mean"]
print(f"\nconfh / lua ratio: {c / l:.2f}x" if l > 0 else "")

# ========== 5. Speedup plots ==========
nest_speed = nest_meas.groupby(["depth", "lib"])["per_access"].apply(trimmed_mean).unstack()
nest_speed["speedup"] = nest_speed["confh"] / nest_speed["lua"]

loop_trial = loop_perf[loop_perf.region.str.contains("%d", na=False)] \
    .groupby(["trial", "lib"])["per_access"].apply(trimmed_mean).unstack()
loop_trial["speedup"] = loop_trial["confh"] / loop_trial["lua"]

plt.figure(3, figsize=(8, 4))
plt.plot(nest_speed.index, nest_speed["speedup"], "o-")
plt.axhline(1, color="gray", ls=":")
plt.xlabel("Nesting depth")
plt.ylabel("Speedup (confh / lua)")
plt.title("Nesting speedup: confh path traversal vs cached lua ref")
plt.grid(ls=":")
plt.tight_layout()
plt.savefig("speedup_nesting.png")
print("Saved speedup_nesting.png")

plt.figure(4, figsize=(6, 4))
step = max(1, len(loop_trial) // 10)
ticks = loop_trial.index[::step]
plt.bar(loop_trial.index, loop_trial["speedup"], width=0.5)
plt.xticks(ticks, ticks.astype(str), rotation=45)
plt.axhline(1, color="gray", ls=":")
plt.xlabel("Trial")
plt.ylabel("Speedup (confh / lua)")
plt.title("Loop speedup per trial: re-traverse vs cached ref")
plt.grid(ls=":", axis="y")
plt.tight_layout()
plt.savefig("speedup_loop.png")
print("Saved speedup_loop.png")
