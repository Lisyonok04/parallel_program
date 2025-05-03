#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <filesystem>
#include <chrono>
#include <sstream>

namespace fs = std::filesystem;

std::vector<std::vector<int>> generateRandomMatrix(size_t size) {
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

void writeMatrixTxt(const std::vector<std::vector<int>>& matrix, const std::string& filename) {
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

std::vector<std::vector<int>> readMatrixTxt(const std::string& filename) {
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

double multiplyAndMeasure(const std::vector<std::vector<int>>& a,
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

void writeResultsToCSV(const std::vector<size_t>& sizes,
    const std::vector<double>& times,
    const std::string& filename = "results.csv") {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Cannot write to CSV file");

    file << "Matrix Size,Time (seconds)\n";
    for (size_t i = 0; i < sizes.size(); ++i) {
        file << sizes[i] << "," << times[i] << "\n";
    }
}

int main() {
    try {
        std::vector<size_t> matrix_sizes = { 100, 200, 300, 400, 500 };
        std::vector<double> execution_times;

        const fs::path base_path = "D:/parallel_programming/laba_1/";

        for (size_t size : matrix_sizes) {
            std::cout << "Testing matrix size: " << size << "x" << size << std::endl;

            // Создаем путь к папке с именем = размеру матрицы
            fs::path folder_path = base_path / std::to_string(size);

            // Создаем папку (если не существует)
            if (!fs::exists(folder_path)) {
                fs::create_directory(folder_path);
            }

            // Генерируем матрицы
            auto matrixA = generateRandomMatrix(size);
            auto matrixB = generateRandomMatrix(size);
            std::vector<std::vector<int>> result;

            // Формируем полные пути к файлам
            fs::path fileA = folder_path / "matrix_A.txt";
            fs::path fileB = folder_path / "matrix_B.txt";
            fs::path result_file = folder_path / "result.txt";

            // Записываем матрицы в файлы
            writeMatrixTxt(matrixA, fileA.string());
            writeMatrixTxt(matrixB, fileB.string());

            // Умножаем и замеряем время
            double time = multiplyAndMeasure(matrixA, matrixB, result);
            writeMatrixTxt(result, result_file.string());

            execution_times.push_back(time);
            std::cout << "Time taken: " << time << " seconds\n\n";
        }

        // Записываем результаты в CSV в текущую директорию
        writeResultsToCSV(matrix_sizes, execution_times, "results.csv");
        return 0;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}