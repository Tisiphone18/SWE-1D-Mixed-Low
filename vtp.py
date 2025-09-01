import os
import xml.etree.ElementTree as ET
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons

enabled_vars = {"h": True, "hu": True, "b": True}

# ---------------- I/O ----------------
def parse_vtp_collection(file_path):
    try:
        tree = ET.parse(file_path)
        root = tree.getroot()
    except Exception as e:
        print(f"[ERROR] Failed to parse VTP file: {e}")
        return []
    datasets = []
    for i, ds in enumerate(root.find("Collection").findall("DataSet")):
        ts_str = ds.attrib.get("timestep", "0")
        file = ds.attrib.get("file", f"unknown_{i}.vtr")
        try:
            timestep = float(ts_str)
            if not np.isfinite(timestep):
                raise ValueError()
        except Exception:
            timestep = i
        datasets.append((timestep, file))
    return datasets

def parse_vtr_file(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()

    coords = root.find(".//Coordinates")
    x_data = coords.findall("DataArray")[0].text.strip().split()
    x = np.array([float(val) for val in x_data])

    cell_data = root.find(".//CellData")
    h = hu = b = None
    for data_array in cell_data.findall("DataArray"):
        name = data_array.attrib.get("Name")
        values = np.array([float(val) for val in data_array.text.strip().split()])
        if name == "h":
            h = values
        elif name == "hu":
            hu = values
        elif name == "b":
            b = values
    return x, h, hu, b

# ------------- Mass helpers -------------
def total_water_volume(x, h):
    if h is None or x is None:
        return np.nan
    dx = np.diff(x)
    n = min(len(h), len(dx))
    return float(np.sum(h[:n] * dx[:n]))
# -----------------------------------------

# ---- MAIN ----
# XXX Set actual path here:
vtp_file = "SWE1D.vtp"
base_path = os.path.dirname(vtp_file) or "."

datasets = parse_vtp_collection(vtp_file)
if not datasets:
    raise RuntimeError("No datasets found in VTP collection.")

# Establish baseline and precompute totals for all frames
x0, h0, hu0, b0 = parse_vtr_file(os.path.join(base_path, datasets[0][1]))
initial_total = total_water_volume(x0, h0)

totals = []
for _, vtr_file in datasets:
    x_i, h_i, _, _ = parse_vtr_file(os.path.join(base_path, vtr_file))
    totals.append(total_water_volume(x_i, h_i))
totals = np.array(totals, dtype=float)

current_index = 0
playing = False

# ---------- Figure layout: bar at left + main plot at right ----------
fig = plt.figure(figsize=(10, 5))
plt.subplots_adjust(bottom=0.25, left=0.1, right=0.97, top=0.9)
gs = fig.add_gridspec(1, 2, width_ratios=[1, 3])
ax_bar = fig.add_subplot(gs[0, 0])  # left: single bar
ax = fig.add_subplot(gs[0, 1])      # right: time slice plot

# Slider & buttons
ax_slider = plt.axes([0.15, 0.1, 0.7, 0.03])
slider = Slider(ax_slider, 'Timestep', 0, len(datasets) - 1, valinit=0, valstep=1)
ax_button = plt.axes([0.8, 0.025, 0.1, 0.04])
play_button = Button(ax_button, 'Play')

# CheckButtons
ax_check = plt.axes([0.01, 0.4, 0.08, 0.15])
labels = ["h", "hu", "b"]
visibility = [enabled_vars[lbl] for lbl in labels]
check = CheckButtons(ax_check, labels, visibility)

# Figure-level info text (NOT on the graph)
info_text = fig.text(0.12, 0.92, "", ha="left", va="top")

# Initialize the single bar (percentage lost)
bar_container = ax_bar.bar(["Lost %"], [0])
ax_bar.set_ylim(0, 100)
ax_bar.set_title("Water Lost (%) vs t₀")
ax_bar.grid(True, axis="y")

def update_info_and_bar(idx):
    total_now = totals[idx]
    delta = total_now - initial_total
    lost = max(-delta, 0.0)
    pct_lost = (lost / initial_total * 100.0) if initial_total > 0 else np.nan

    # Update bar height & y-limit
    bar_container.patches[0].set_height(0 if not np.isfinite(pct_lost) else pct_lost)
    ax_bar.set_ylim(0, max(100, (pct_lost if np.isfinite(pct_lost) else 0) * 1.1))

    info_text.set_text(
        f"Total @ t0: {initial_total:.6g}   |   "
        f"Total now: {total_now:.6g}   |   "
        f"Lost: {lost:.6g} ({pct_lost:.2f}%)   |   "
        f"Δ vs t0: {delta:+.6g}"
    )

def update_plot(index):
    idx = int(index)
    timestep, vtr_file = datasets[idx]
    full_path = os.path.join(base_path, vtr_file)
    x, h, hu, b = parse_vtr_file(full_path)

    ax.clear()
    if enabled_vars["h"] and h is not None:
        ax.plot(x[:-1], h, label="h")
    if enabled_vars["hu"] and hu is not None:
        ax.plot(x[:-1], hu, label="hu")
    if enabled_vars["b"] and b is not None:
        ax.plot(x[:-1], b, label="b")

    ax.set_title(f"Frame #{idx} | Time ≈ {timestep:.2f}")
    ax.set_xlabel("x")
    ax.set_ylabel("Values")
    ax.grid(True)
    ax.legend()

    update_info_and_bar(idx)
    fig.canvas.draw_idle()

# --- FIXED PLAY BUTTON: single persistent timer ---
timer = fig.canvas.new_timer(interval=300)  # ms

def advance():
    global current_index
    if not playing:
        return
    current_index = (current_index + 1) % len(datasets)
    slider.set_val(current_index)  # triggers update_plot

timer.add_callback(advance)

def toggle_play(event):
    global playing
    playing = not playing
    play_button.label.set_text("Pause" if playing else "Play")
    if playing:
        timer.start()
    else:
        timer.stop()

def on_slider(val):
    global current_index
    current_index = int(val)
    update_plot(current_index)

def toggle_var(label):
    enabled_vars[label] = not enabled_vars[label]
    update_plot(current_index)

check.on_clicked(toggle_var)
slider.on_changed(on_slider)
play_button.on_clicked(toggle_play)

# Initial draw
update_plot(current_index)

plt.show()
