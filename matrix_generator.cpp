#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <sstream>
#include <chrono>
#include <mpi.h>

using namespace std;

vector<vector<int>> generate_random(size_t size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 99);

    vector<vector<int>> matrix(size, vector<int>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
    return matrix;
}

void write_matrix_in_txt(const vector<vector<int>>& matrix, const string& path) {
    ofstream out(path);
    for (const auto& row : matrix) {
        for (int val : row) {
            out << val << " ";
        }
        out << endl;
    }
}

vector<vector<int>> read_matrix_from_txt(const string& path) {
    ifstream in(path);
    vector<vector<int>> matrix;
    string line;

    while (getline(in, line)) {
        istringstream iss(line);
        vector<int> row;
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        if (!row.empty()) {
            matrix.push_back(row);
        }
    }
    return matrix;
}

vector<vector<int>> multiply_matrixes(const vector<vector<int>>& A,
    const vector<vector<int>>& B,
    int rank, int size) {
    int n = A.size();
    vector<vector<int>> local_result(n, vector<int>(n, 0));
    vector<int> global_result(n * n, 0);

    int rows_per_process = n / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? n : start_row + rows_per_process;

    for (int i = start_row; i < end_row; i++) {
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < n; j++) {
                local_result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    vector<int> flat_local(n * n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            flat_local[i * n + j] = local_result[i][j];

    MPI_Reduce(flat_local.data(), global_result.data(), n * n,
        MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    vector<vector<int>> result;
    if (rank == 0) {
        result.resize(n, vector<int>(n));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                result[i][j] = global_result[i * n + j];
    }

    return result;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    string base_path = "D:/parallel_programming/lab_3/data/";
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
  
    vector<int> matrix_sizes = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1500, 2000, 3000};
    vector<double> times(matrix_sizes.size(), 0.0);

    if (rank == 0) {
        for (const auto& count : matrix_sizes) {
            for (int i = 1; i < 3; ++i) {
                vector<vector<int>> matrix = generate_random(count);
                string path = base_path + to_string(i) + "_" + to_string(count) + ".txt";
                write_matrix_in_txt(matrix, path);
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    for (size_t i = 0; i < matrix_sizes.size(); ++i) {
        int count = matrix_sizes[i];
        string path_1 = base_path + "matrix_A_" + to_string(count) + ".txt";
        string path_2 = base_path + "matrix_B_" + to_string(count) + ".txt";
        string result_path = base_path + "result_" + to_string(count) + ".txt";

        vector<vector<int>> matrixA, matrixB;

        if (rank == 0) {
            matrixA = read_matrix_from_txt(path_1);
            matrixB = read_matrix_from_txt(path_2);


            for (int p = 1; p < size; p++) {
                MPI_Send(&count, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
            }

            for (int p = 1; p < size; p++) {
                for (const auto& row : matrixA) {
                    MPI_Send(row.data(), count, MPI_INT, p, 1, MPI_COMM_WORLD);
                }
                for (const auto& row : matrixB) {
                    MPI_Send(row.data(), count, MPI_INT, p, 2, MPI_COMM_WORLD);
                }
            }
        }
        else {
            MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            matrixA.resize(count, vector<int>(count));
            matrixB.resize(count, vector<int>(count));

            for (auto& row : matrixA) {
                MPI_Recv(row.data(), count, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            for (auto& row : matrixB) {
                MPI_Recv(row.data(), count, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        auto start_time = chrono::steady_clock::now();

        vector<vector<int>> result = multiply_matrixes(matrixA, matrixB, rank, size);

        auto end_time = chrono::steady_clock::now();

        if (rank == 0) {
            write_matrix_in_txt(result, result_path);
            times[i] = chrono::duration<double, milli>(end_time - start_time).count();
        }
    }

    if (rank == 0) {
        ofstream out("results.txt");
        for (size_t i = 0; i < matrix_sizes.size(); ++i) {
            out << "Matrix size: " << matrix_sizes[i] << "x" << matrix_sizes[i] << ", " << "Time: " << times[i] << " milliseconds\n";
        }
    }
    
    MPI_Finalize();
    return 0;
}