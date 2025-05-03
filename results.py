import numpy as np
import matplotlib.pyplot as plt


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


def plot_results(filename):
    sizes, times = [], []
    
    with open(filename, 'r') as file:
        for line in file:
            parts = line.split(", ")
            size_part = parts[0].split(": ")[1]
            time_part = parts[1].split(": ")[1].replace(" seconds", "")
            sizes.append(int(size_part.split("x")[0]))
            times.append(float(time_part))
    
    plt.figure(figsize=(10, 6))
    plt.plot(sizes, times, marker='o')
    plt.title("График зависимости времени от размера матрицы")
    plt.xlabel("Размер матрицы (n x n)")
    plt.ylabel("Время")
    plt.grid()
    plt.savefig("graph.png")
    plt.show()


if __name__ == "__main__":
    matrix_sizes = [10, 50, 100, 200, 300, 400, 500, 1000, 1500, 1750, 2000]
    results = []

    for size in matrix_sizes:
        is_correct = check_matrix_multiplication(size)
        results.append((size, is_correct))

    write_results_to_file(results, "report.txt")
    plot_results("results.txt")