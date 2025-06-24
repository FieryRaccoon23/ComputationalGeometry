#include <torch/script.h> // TorchScript API
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_2 = K::Point_2;

int main() {
    // Load the TorchScript model
    torch::jit::script::Module model;
    try {
        model = torch::jit::load("model/point_transformer_edge_model.pt");
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model\n";
        return -1;
    }

    const int N = 50; // Number of points
    std::vector<Point_2> points;
    CGAL::Random_points_in_square_2<Point_2> gen(1.0);

    std::vector<float> point_data;
    for (int i = 0; i < N; ++i) {
        Point_2 p = *gen++;
        points.push_back(p);
        point_data.push_back(p.x());
        point_data.push_back(p.y());
    }

    // Create [N,2] tensor from points
    torch::Tensor points_tensor = torch::from_blob(point_data.data(), {N, 2}).clone();

    // Normalize if needed (match training normalization)
    torch::Tensor mean = points_tensor.mean(0, true);
    torch::Tensor std = points_tensor.std(0, true) + 1e-6;
    points_tensor = (points_tensor - mean) / std;

    // Prepare input features: [1,N,64]
    torch::Tensor zero_pad = torch::zeros({N, 62});
    torch::Tensor feats = torch::cat({points_tensor, zero_pad}, 1).unsqueeze(0);

    // Prepare positions: [1,N,3]
    torch::Tensor z_col = torch::zeros({N, 1});
    torch::Tensor pos = torch::cat({points_tensor, z_col}, 1).unsqueeze(0);

    // Run model
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(feats);
    inputs.push_back(pos);

    at::Tensor output = model.forward(inputs).toTensor(); // [N,N]

    // Threshold and print edges
    auto acc = output.accessor<float, 2>();
    float threshold = 0.5; // Adjust if needed

    std::cout << "Predicted edges:\n";
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) { // avoid duplicates
            if (acc[i][j] > threshold) {
                std::cout << (i+1) << " -- " << (j+1) << " (prob=" << acc[i][j] << ")\n";
            }
        }
    }

    return 0;
}
