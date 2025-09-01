import os
import re
import matplotlib.pyplot as plt
import imageio.v2 as imageio
import math

def safe_finite_sum(arr):
    """Return sum of finite values in arr; NaN if none are finite."""
    if not arr:
        return float('nan')
    vals = [v for v in arr if math.isfinite(v)]
    return math.fsum(vals) if vals else float('nan')


# -----------------------------
# CONFIG
# -----------------------------
base_dir = "."
folders = ["aug_out_mx", "hllc_out_mx", "fwave_out_mx", "osher_out_mx", "out_mixedBFloat", "aug_out_float"]  # set your folders here
log_filename = "timing_log.txt"
label_map = {
    "aug_out_mx": "aug",
    "hllc_out_mx": "hllc",
    "fwave_out_mx": "fwave",
    "osher_out_mx": "osher",
    "out_mixedBFloat": "rusanov",
    "aug_out_float": "baseline",
}

data_dirs = {name: os.path.join(base_dir, name) for name in folders}
log_paths = {name: os.path.join(data_dirs[name], log_filename) for name in folders}

frame_dir = os.path.join(base_dir, "video_frames_mx")
output_video_path = os.path.join(base_dir, "comparison_video.mp4")
os.makedirs(frame_dir, exist_ok=True)   # If it exists, it's reused; files with same name will be overwritten.

# OPTIONAL: wipe old frames to avoid mixing runs
# for fn in os.listdir(frame_dir):
#     if fn.lower().endswith(".png"):
#         try:
#             os.remove(os.path.join(frame_dir, fn))
#         except OSError:
#             pass

# -----------------------------
# HELPERS
# -----------------------------
def parse_log(path):
    results = {}
    if not os.path.exists(path):
        return results
    with open(path, "r") as f:
        for raw in f:
            line = raw.strip()
            if not line:
                continue
            if ", ERROR:" in line:
                key, err = line.split(", ERROR:", 1)
                results[key.strip()] = {"status": "ERROR", "error": err.strip()}
            elif ", duration=" in line:
                key, dur = line.split(", duration=", 1)
                dur = dur.strip()
                if dur.endswith("s"):
                    dur = dur[:-1]
                try:
                    results[key.strip()] = {"status": "OK", "duration": float(dur)}
                except ValueError:
                    results[key.strip()] = {"status": "ERROR", "error": f"Bad duration '{dur}'"}
    return results

def read_vtr_data(filepath):
    with open(filepath, "r") as f:
        content = f.read()
    def extract_array(name):
        pattern = fr'<DataArray Name="{name}".*?>(.*?)</DataArray>'
        m = re.search(pattern, content, re.DOTALL)
        if not m:
            return None
        return [float(x) for x in m.group(1).strip().split()]
    return {"h": extract_array("h"), "hu": extract_array("hu"), "b": extract_array("b")}

def list_basename_to_indices(folder_path):
    mapping = {}
    for fn in os.listdir(folder_path):
        if not fn.endswith(".vtr"):
            continue
        name = fn[:-4]  # strip .vtr
        m = re.match(r"^(.*)_(\d+)$", name)
        if not m:
            continue
        base, idx = m.group(1), int(m.group(2))
        mapping.setdefault(base, set()).add(idx)
    return mapping

# -----------------------------
# LOAD LOGS
# -----------------------------
logs = {name: parse_log(path) for name, path in log_paths.items()}

# -----------------------------
# DISCOVER SERIES & INDICES (UNION, not intersection)
# -----------------------------
per_folder_map = {}
active_folders = []  # ### CHANGED: only include existing folders
for name in folders:
    folder_path = data_dirs[name]
    if not os.path.isdir(folder_path):
        # Skip missing folders entirely
        continue
    per_folder_map[name] = list_basename_to_indices(folder_path)
    active_folders.append(name)

# ### CHANGED: Use UNION of basenames so a folder that lacks a basename is simply left out for that plot
all_basenames = set()
for m in per_folder_map.values():
    all_basenames |= set(m.keys())

# For each basename, use UNION of indices that exist anywhere.
series_plan = {}  # {basename: sorted_indices_present_somewhere}
for base in sorted(all_basenames):
    idx_sets = [per_folder_map[n].get(base, set()) for n in active_folders]
    union_indices = set().union(*idx_sets) if idx_sets else set()
    # usually we skip i=0 for plotting (keep it as baseline only)
    if 0 in union_indices and len(union_indices) > 1:
        indices_to_plot = sorted(i for i in union_indices if i != 0)
    else:
        indices_to_plot = sorted(union_indices)
    if indices_to_plot:
        series_plan[base] = indices_to_plot

# -----------------------------
# CONSISTENT COLORS PER FOLDER (only for active folders)
# -----------------------------
#cycle_colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
#folder_color = {name: cycle_colors[i % len(cycle_colors)] for i, name in enumerate(active_folders)}

cycle_colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
custom_palette = {
    "aug_out_mx":           "#00bcd4",   # fp64  = turquoise
    "hllc_out_mx":           "tab:green", # fp32  = green
    "fwave_out_mx":            "tab:red",   # bf16  = red
    "osher_out_mx":             "#fc97ba",   # fp16  = pink
    "out_mixedBFloat":"#9b59b6",   # (rescaled) = purple
    "aug_out_double":      "#f1c40f",   # mixed bf16/fp32 = yellow
}
folder_color = {}
for i, name in enumerate(active_folders):
    folder_color[name] = custom_palette.get(name, cycle_colors[i % len(cycle_colors)])

# -----------------------------
# GENERATE FRAMES
# -----------------------------
frame_paths = []

for basename, indices in series_plan.items():
    # Preload baseline (i=0) from any available folder for initial water
    # Preload baseline (i=0) from any available folder for initial water
    initial_total = None
    baseline_path_found = None
    for name in active_folders:
        candidate = os.path.join(data_dirs[name], f"{basename}_0.vtr")
        if os.path.exists(candidate):
            try:
                baseline = read_vtr_data(candidate)
                if baseline["h"]:
                    it = safe_finite_sum(baseline["h"])
                    if math.isfinite(it):
                        initial_total = it
                        baseline_path_found = candidate
                        break
            except Exception:
                pass


    for i in indices:
        # Load only folders that actually have this file
        data_by_folder = {}
        for name in active_folders:
            vtr_path = os.path.join(data_dirs[name], f"{basename}_{i}.vtr")
            if os.path.exists(vtr_path):
                try:
                    data_by_folder[name] = read_vtr_data(vtr_path)
                except Exception as e:
                    # treat as unavailable for this step
                    continue

        # ### CHANGED: If no folder has this step, skip; otherwise only use the ones that do.
        if not data_by_folder:
            continue

        # Determine x coords from first available h length
        x_len = None
        for d in data_by_folder.values():
            if d.get("h"):
                x_len = len(d["h"])
                break
        if x_len is None:
            continue
        x_coords = list(range(x_len))

        # Create figure with 4 subplots
        fig, axs = plt.subplots(1, 4, figsize=(20, 4.5))
        fig.suptitle(f"{basename} | i={i}", fontsize=16)

        # 1–3: lines for h, hu, b — ONLY for folders present at this index
        fields = ["h", "hu", "b"]
        for k, field in enumerate(fields):
            ax = axs[k]
            has_any = False
            for name, d in data_by_folder.items():
                arr = d.get(field)
                if arr:
                    ax.plot(x_coords, arr, label=label_map.get(name, name), color=folder_color[name])
                    has_any = True
            ax.set_title(field)
            ax.set_xlabel("x")
            ax.set_ylabel(field)
            if has_any:
                ax.legend()  # legend will only include present folders

        # 4: total water bars — ONLY for folders present at this index
        ax4 = axs[3]
        labels = list(data_by_folder.keys())

        # true totals (may be NaN if arrays contain NaN/Inf)
        totals = [
            safe_finite_sum(d["h"]) if d.get("h") else float('nan')
            for d in data_by_folder.values()
        ]

        # values sent to bar(): replace non-finite by 0 so plotting never fails
        plot_totals = [t if math.isfinite(t) else 0.0 for t in totals]

        colors = [folder_color[n] for n in labels]
        bars = ax4.bar(labels, plot_totals, color=colors)
        ax4.set_title("Total water (∑h)")
        ax4.set_ylabel("∑ h")

        # annotate only finite values, and use the true total for the label
        for j, b in enumerate(bars):
            val = totals[j]
            if math.isfinite(val):
                ax4.annotate(f"{val:.2f}",
                             xy=(b.get_x() + b.get_width()/2, b.get_height()),
                             xytext=(0, 3), textcoords="offset points",
                             ha="center", va="bottom", fontsize=9)


        # Baseline line from i=0 if we have it
        if initial_total is not None and math.isfinite(initial_total):
            ax4.axhline(initial_total, linestyle="--", linewidth=1)
            ax4.text(
                0.98, 0.95, f"baseline(i=0): {initial_total:.2f}",
                transform=ax4.transAxes, ha="right", va="top", fontsize=9,
                bbox=dict(facecolor="white", alpha=0.75, edgecolor="none", pad=2),
            )

        # Headroom (only if at least one finite value exists)
        ymax_candidates = [t for t in totals if math.isfinite(t)]
        if initial_total is not None and math.isfinite(initial_total):
            ymax_candidates.append(initial_total)

        if ymax_candidates:
            top = max(ymax_candidates)
            if not math.isfinite(top) or top <= 0:
                top = 1.0
            ax4.set_ylim(0, top * 1.10)  # +10% headroom


    # Footer info: durations from logs — ONLY for folders present at this index
        # ---------- BUILD INFO LINES FIRST ----------
        info_lines = []
        durations = {}
        for name in data_by_folder.keys():
            entry = logs.get(name, {}).get(basename)
            if entry is None:
                info_lines.append(f"{name}: no log entry")
                durations[name] = None
            elif entry.get("status") == "ERROR":
                info_lines.append(f"{name}: {entry['error']}")
                durations[name] = None
            else:
                d = entry.get("duration")
                durations[name] = d
                info_lines.append(f"{name}: {d:.3f}s")

        present = {n: d for n, d in durations.items() if d is not None}
        if present:
            fastest = min(present, key=present.get)
            fval = present[fastest]
            info_lines.append(f"Fastest: {fastest}")
            for n, d in present.items():
                if n != fastest:
                    pct = 100.0 * (d - fval) / fval if fval > 0 else 0.0
                    info_lines.append(f"{n}: {pct:.1f}% slower vs {fastest}")

        if baseline_path_found is None:
            info_lines.append("Baseline i=0 missing in all folders")

        # ---------- 1×5 SUBPLOTS (last one is text panel) ----------
        fig, axs = plt.subplots(
            1, 5,
            figsize=(24, 4.8),
            gridspec_kw={"width_ratios": [1, 1, 1, 1, 1.25]},
            constrained_layout=True  # avoids overlaps automatically
        )
        fig.suptitle(f"{basename} | i={i}", fontsize=16)

        # 1–3: h, hu, b
        fields = ["h", "hu", "b"]
        for k, field in enumerate(fields):
            ax = axs[k]
            has_any = False
            for name, d in data_by_folder.items():
                arr = d.get(field)
                if arr:
                    ax.plot(x_coords, arr, label=label_map.get(name, name), color=folder_color[name])
                    has_any = True
            ax.set_title(field)
            ax.set_xlabel("x")
            ax.set_ylabel(field)
            if has_any:
                leg = ax.legend()
                if leg is not None:
                    leg.set_frame_on(True)
                    leg.set_alpha(0.85)
                    leg.get_frame().set_linewidth(0)

        # 4: total water bars
        ax4 = axs[3]
        labels = list(data_by_folder.keys())
        totals = [safe_finite_sum(d["h"]) if d.get("h") else float('nan') for d in data_by_folder.values()]
        plot_totals = [t if math.isfinite(t) else 0.0 for t in totals]
        colors = [folder_color[n] for n in labels]
        bars = ax4.bar(labels, plot_totals, color=colors)
        ax4.set_title("Total water (∑h)")
        ax4.set_ylabel("∑ h")
        for j, b in enumerate(bars):
            val = totals[j]
            if math.isfinite(val):
                ax4.annotate(f"{val:.2f}",
                             xy=(b.get_x() + b.get_width()/2, b.get_height()),
                             xytext=(0, 3), textcoords="offset points",
                             ha="center", va="bottom", fontsize=9)

        if initial_total is not None and math.isfinite(initial_total):
            ax4.axhline(initial_total, linestyle="--", linewidth=1)
            ax4.text(
                0.98, 0.95, f"baseline(i=0): {initial_total:.2f}",
                transform=ax4.transAxes, ha="right", va="top", fontsize=9,
                bbox=dict(facecolor="white", alpha=0.75, edgecolor="none", pad=2),
            )

        ymax_candidates = [t for t in totals if math.isfinite(t)]
        if initial_total is not None and math.isfinite(initial_total):
            ymax_candidates.append(initial_total)
        if ymax_candidates:
            top = max(ymax_candidates)
            if not math.isfinite(top) or top <= 0:
                top = 1.0
            ax4.set_ylim(0, top * 2.5)

        # 5: TEXT PANEL (no axes; never overlaps)
        ax5 = axs[4]
        ax5.axis("off")
        ax5.text(
            0.0, 0.5, "\n".join(info_lines),
            ha="left", va="center", fontsize=10, family="monospace",
            bbox=dict(facecolor="white", alpha=0.95, edgecolor="0.8", pad=6),
            transform=ax5.transAxes,
        )

        # Slight title padding
        for ax in axs[:4]:
            ax.set_title(ax.get_title(), pad=6)

        frame_path = os.path.join(frame_dir, f"{basename}_i{i}.png")
        plt.savefig(frame_path, dpi=120)
        plt.close(fig)
        frame_paths.append(frame_path)
