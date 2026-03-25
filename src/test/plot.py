import numpy as np
import matplotlib.pyplot as plt
import os
import sys

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
    plt.plot(data[25:], linestyle='-', marker='.', markersize=2, color='blue')

    plt.title(f"Visualizing CSV Data: {os.path.basename(file_path)}")
    plt.xlabel("Sample Index")
    plt.ylabel("Value (float)")
    plt.grid(True, which='both', linestyle='--', alpha=0.5)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot.py <path_to_csv_file>")
        sys.exit(1)
    plot_csv_float(sys.argv[1])