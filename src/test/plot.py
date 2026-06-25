import numpy as np
import matplotlib.pyplot as plt
import os
import sys

import matplotlib

matplotlib.rcParams.update({'font.size': 22})

def plot_csv_float(file_path):
    if not os.path.exists(file_path):
        print(f"Error: File {file_path} not found.")
        return

    try:
        data = np.loadtxt(file_path, delimiter=",", skiprows=1, dtype=np.float32)
    except Exception as e:
        print(f"Failed to read file: {e}")
        return

    if data.size == 0:
        print("CSV contains no samples.")
        return

    print(f"Successfully read {len(data)} samples.")

    # 3. Plotting
    plt.figure(figsize=(10, 6))
    plt.plot(data[25:], linestyle='-', marker='.', markersize=2, color='blue', linewidth=2.0)

    plt.title(f"NDS-TMPE Acquisition Results")
    plt.xlabel("Sample Index")
    plt.ylabel("Amplitude (V)")
    plt.grid(True, which='both', linestyle='--', alpha=0.5)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot.py <path_to_csv_file>")
        sys.exit(1)
    plot_csv_float(sys.argv[1])
