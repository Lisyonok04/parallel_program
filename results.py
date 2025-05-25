import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path


def read_matrix_from_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
        matrix = np.array([list(map(int, line.split())) for line in lines[1:]])
    return matrix


def check_matrix_multiplication(size):
    folder_name = str(size)
    matrix_1 = read_matrix_from_file(f"{folder_name}/matrix_A.txt")
    matrix_2 = read_matrix_from_file(f"{folder_name}/matrix_B.txt")
    result_numpy = np.dot(matrix_1, matrix_2)
    result_file = read_matrix_from_file(f"{folder_name}/result.txt")
    return np.array_equal(result_numpy, result_file)


def write_results_to_file(results, filename):
    with open(filename, 'w') as file:
        for size, is_correct in results:
            if is_correct:
                file.write(f"Matrix size {size}x{size}: Correct\n")
            else:
                file.write(f"Matrix size {size}x{size}: Incorrect\n")


# def plot_results(filename):
#     sizes, times = [], []
    
#     with open(filename, 'r') as file:
#         for line in file:
#             parts = line.split(", ")
#             size_part = parts[0].split(": ")[1]
#             time_part = parts[1].split(": ")[1].replace(" milliseconds", "")
#             sizes.append(int(size_part.split("x")[0]))
#             times.append(float(time_part))
    
#     plt.figure(figsize=(10, 6))
#     plt.plot(sizes, times, marker='o')
#     plt.title("График зависимости времени от размера матрицы")
#     plt.xlabel("Размер матрицы (n x n)")
#     plt.ylabel("Время")
#     plt.grid()
#     plt.savefig("graph.png")
#     plt.show()

def parse_results_file(filename):
    """Извлечение данных о времени из файла результатов"""
    sizes, times = [], []
    with open(filename, 'r') as f:
        for line in f:
            if "Matrix size" in line and "Time" in line:
                parts = line.split(", ")
                size = int(parts[0].split(": ")[1].split("x")[0])
                time = float(parts[1].split(": ")[1].replace(" milliseconds", ""))
                sizes.append(size)
                times.append(time)
    return sizes, times

def plot_comparison(base_dir):
    """Построение сводного графика производительности"""
    plt.figure(figsize=(12, 8))
    colors = ['g', 'y', 'r', 'b']
    thread_counts = [1, 2]
    paral = ["mpi4", "mpi6"]
    for parallel in paral:
        result_file = base_dir / f"results_{parallel}.txt"
        if not result_file.exists():
            continue
            
        sizes, times = parse_results_file(result_file)
        plt.plot(sizes, times, 'o-', label=f'{parallel}', color=colors.pop(0))

    plt.title('Производительность умножения матриц (разные потоки)')
    plt.xlabel('Размер матрицы (N x N)')
    plt.ylabel('Время выполнения (милисекунды)')
    plt.grid(True)
    plt.legend()
    plt.savefig(base_dir / "performance_comparison.png")
    plt.show()


if __name__ == "__main__":
   # matrix_sizes = [50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1500, 1750, 2000, 3000]
   # results = []

   # for size in matrix_sizes:
   #     is_correct = check_matrix_multiplication(size)
    #    results.append((size, is_correct))

   # write_results_to_file(results, "report.txt")
    base_dir = Path("D:/parallel_programming/lab_3/")
    plot_comparison(base_dir)