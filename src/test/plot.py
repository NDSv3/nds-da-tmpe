import numpy as np
import matplotlib.pyplot as plt
import os
import sys

def plot_binary_uint64(file_path):
    if not os.path.exists(file_path):
        print(f"Error: File {file_path} not found.")
        return

    try:
        data = np.fromfile(file_path, dtype=np.uint32) * 10.24 / (2**(32 - 16))
    except Exception as e:
        print(f"Failed to read file: {e}")
        return

    print(f"Successfully read {len(data)} samples.")

    # 3. Plotting
    plt.figure(figsize=(10, 6))
    plt.plot(data[25:], linestyle='-', marker='.', markersize=2, color='blue')

    plt.title(f"Visualizing Binary Data: {os.path.basename(file_path)}")
    plt.xlabel("Sample Index")
    plt.ylabel("Value (uint32)")
    plt.grid(True, which='both', linestyle='--', alpha=0.5)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot.py <path_to_binary_file>")
        sys.exit(1)
    plot_binary_uint64(sys.argv[1])