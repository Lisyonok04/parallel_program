#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <filesystem>
#include <chrono>
#include <sstream>

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

std::vector<std::vector<int>> read_from_txt(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Cannot open file: " + filename);

    size_t size;
    file >> size;
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            if (!(file >> matrix[i][j])) {
                throw std::runtime_error("Invalid matrix data in file");
            }
        }
    }

    if (matrix.size() != matrix[0].size()) {
        throw std::runtime_error("Matrix is not square in file: " + filename);
    }

    return matrix;
}

double multiply_matrixes(const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    std::vector<std::vector<int>>& matrix_result) {
    size_t rows_1 = a.size();
    size_t cols_1 = a[0].size();
    size_t rows_2 = b.size();
    size_t cols_2 = b[0].size();

    if (cols_1 != rows_2) {
        throw std::runtime_error("Matrix dimensions mismatch for multiplication");
    }

    matrix_result.resize(rows_1, std::vector<int>(cols_2, 0));

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < rows_1; ++i) {
        for (size_t j = 0; j < cols_2; ++j) {
            for (size_t k = 0; k < cols_1; ++k) {
                matrix_result[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    try {
        std::vector<size_t> matrix_sizes = { 10, 50, 100, 200, 300, 400, 500, 1000, 1500, 1750, 2000 };
        std::vector<double> execution_times;

        const fs::path base_path = "D:/parallel_programming/laba_1/";
        std::ofstream results("D:/parallel_programming/laba_1/results.txt");
        for (size_t size : matrix_sizes) {
            results << "Testing matrix size: " << size << "x" << size << ", ";
            std::cout << "Testing matrix size: " << size << "x" << size << ", ";
            fs::path folder_path = base_path / std::to_string(size);
            if (!fs::exists(folder_path)) {
                fs::create_directory(folder_path);
            }
            auto matrixA = generate_random(size);
            auto matrixB = generate_random(size);
            std::vector<std::vector<int>> result;
            fs::path fileA = folder_path / "matrix_A.txt";
            fs::path fileB = folder_path / "matrix_B.txt";
            fs::path result_file = folder_path / "result.txt";
            write_matrix_in_txt(matrixA, fileA.string());
            write_matrix_in_txt(matrixB, fileB.string());
            double time = multiply_matrixes(matrixA, matrixB, result);
            write_matrix_in_txt(result, result_file.string());

            execution_times.push_back(time);
            results << "Time taken: " << time << " seconds\n";
            std::cout << "Time taken: " << time << " seconds\n";
        }

        results.close();
        std::cout << "That's all for now";
        return 0;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}