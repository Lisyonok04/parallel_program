#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <omp.h>

namespace fs = std::filesystem;

std::vector<std::vector<int>> generate_random(size_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 99);

    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
    return matrix;
}

void write_matrix_in_txt(const std::vector<std::vector<int>>& matrix, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot write to file: " + filename);
    }

    int size = matrix.size();
    file << size << std::endl;
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            file << matrix[i][j] << " ";
        }
        file << std::endl;
    }
}

double multiply_matrixes(const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    std::vector<std::vector<int>>& matrix_result,
    int num_threads) {
    size_t rows_1 = a.size();
    size_t cols_1 = a[0].size();
    size_t rows_2 = b.size();
    size_t cols_2 = b[0].size();

    if (cols_1 != rows_2) {
        throw std::runtime_error("Matrix dimensions mismatch for multiplication");
    }

    matrix_result.resize(rows_1, std::vector<int>(cols_2, 0));
    omp_set_num_threads(num_threads);

    auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < rows_1; ++i) {
        for (int j = 0; j < cols_2; ++j) {
            int temp = 0;
            for (int k = 0; k < cols_1; ++k) {
                temp += a[i][k] * b[k][j];
            }
            matrix_result[i][j] = temp;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double>(end - start).count() * 1000;
    return total_time;
}

void run_test_for_threads(const std::vector<int>& thread_counts, const fs::path& base_path) {
    std::vector<size_t> matrix_sizes = { 10, 50, 100, 200, 300, 400, 500, 1000, 1500, 1750, 2000 };

    for (int threads : thread_counts) {
        std::ofstream results(base_path / ("results_" + std::to_string(threads) + ".txt"));
        results << "Testing with " << threads << " threads\n";
        std::cout << "Testing with " << threads << " threads\n";

        for (size_t size : matrix_sizes) {
            results << "Matrix size: " << size << "x" << size << ", ";
            std::cout << "Matrix size: " << size << "x" << size << ", ";

            fs::path folder_path = base_path / std::to_string(size);
            if (!fs::exists(folder_path)) {
                fs::create_directory(folder_path);
            }

            auto matrixA = generate_random(size);
            auto matrixB = generate_random(size);
            std::vector<std::vector<int>> result;

            fs::path fileA = folder_path / ("matrix_A_" + std::to_string(threads) + ".txt");
            fs::path fileB = folder_path / ("matrix_B_" + std::to_string(threads) + ".txt");
            fs::path result_file = folder_path / ("result_" + std::to_string(threads) + ".txt");

            write_matrix_in_txt(matrixA, fileA.string());
            write_matrix_in_txt(matrixB, fileB.string());

            double time = multiply_matrixes(matrixA, matrixB, result, threads);
            write_matrix_in_txt(result, result_file.string());

            results << "Time: " << time << " milliseconds\n";
            std::cout << "Time: " << time << " milliseconds\n";
        }
        results.close();
    }
}

int main() {
#ifdef _OPENMP
    std::cout << "OpenMP supported! Max threads: " << omp_get_max_threads() << std::endl;
#else
    std::cerr << "OpenMP NOT SUPPORTED! Recompile with -fopenmp" << std::endl;
    return 1;
#endif
    try {
        const fs::path base_path = "D:/parallel_programming/laba_2/";
        std::vector<int> thread_counts = { 1, 2, 4, 6, 10 };

        run_test_for_threads(thread_counts, base_path);

        std::cout << "All tests completed successfully!\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
