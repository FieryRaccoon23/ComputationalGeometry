import torch
import numpy as np
import os
import json
import PointTransformModelDefinition
import ComputeHelpers
from point_transformer_pytorch import PointTransformerLayer

# ============================
# Global Variable
# ============================
Num_of_Points = 4

# ============================
# Utilities
# ============================
def load_points(csv_path):
    points = np.loadtxt(csv_path, delimiter=',')
    return points.astype(np.float32)

def normalize_points(points):
    mean = points.mean(axis=0)
    std = points.std(axis=0) + 1e-6
    return (points - mean) / std

def generate_edges(model, points):
    with torch.no_grad():
        device = next(model.parameters()).device
        points_tensor = torch.tensor(points, device=device)
        adj_matrix = model(points_tensor).cpu().numpy()
    return adj_matrix

def write_msh(points, adj_matrix, output_path, threshold=0.5):
    N = points.shape[0]
    edge_list = []
    all_edges_data = []

    for i in range(N):
        for j in range(i + 1, N):
            score = float(adj_matrix[i, j])
            all_edges_data.append({
                "node1": i + 1,
                "node2": j + 1,
                "score": score
            })
            if score > threshold:
                edge_list.append((i + 1, j + 1))  # 1-based indexing for Gmsh

    # Write the .msh file
    with open(output_path, 'w') as f:
        f.write("$MeshFormat\n2.2 0 8\n$EndMeshFormat\n")
        f.write(f"$Nodes\n{N}\n")
        for idx, (x, y) in enumerate(points, start=1):
            f.write(f"{idx} {x} {y} 0\n")
        f.write("$EndNodes\n")

        f.write(f"$Elements\n{len(edge_list)}\n")
        for i, (a, b) in enumerate(edge_list, start=1):
            f.write(f"{i} 1 0 {a} {b}\n")
        f.write("$EndElements\n")

    print(f"Mesh written to {output_path} with {len(edge_list)} edges")

    # Write JSON with all edges and scores
    json_path = os.path.splitext(output_path)[0] + "_edges.json"
    with open(json_path, 'w') as jf:
        json.dump(all_edges_data, jf, indent=2)

    print(f"Edge data written to {json_path}")

# ============================
# Main
# ============================
def main():
    # === CONFIG ===
    model_path = "model/point_transformer_edge_model.pt"
    input_csv = "python/points_input.csv"
    output_msh = "python/generated_out_mesh.msh"
    threshold = 0.7

    # === Load model ===
    model = PointTransformModelDefinition.EdgePredictor()
    model.load_state_dict(torch.load(model_path, map_location="cpu"))
    model.eval()

    # === Load points ===
    points = load_points(input_csv)
    # N = points.shape[0]
    # if N > Num_of_Points:
    #     print(f"Warning: Model was trained for {Num_of_Points} points, you provided {N}. May not work correctly.")
    # points_norm = normalize_points(points)

    # === Normalize and compute features (added part) ===
    # mean = points.mean(axis=0)
    # std = points.std(axis=0) + 1e-6
    # points_norm = (points - mean) / std

    points_norm = points

    dist_to_centroid = ComputeHelpers.compute_distance_to_centroid(points_norm)  # [N,1]
    local_density = ComputeHelpers.compute_local_density(points_norm)           # [N,1]
    angles = ComputeHelpers.compute_angle_from_x_axis(points_norm)              # [N,1]

    #features = np.concatenate([points_norm, dist_to_centroid, local_density, angles], axis=1)  # [N,5]
    features = np.concatenate([points_norm], axis=1)  # [N,5]

    # === Generate edges ===
    adj_matrix = generate_edges(model, features)

    # === Diagnostic: print min, max, mean ===
    print(f"Predicted adjacency matrix stats → min: {adj_matrix.min():.4f}, max: {adj_matrix.max():.4f}, mean: {adj_matrix.mean():.4f}")

    # === Write msh file ===
    write_msh(points, adj_matrix, output_msh, threshold)

if __name__ == "__main__":
    main()
