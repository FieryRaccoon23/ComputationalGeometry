import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
import glob
import os
import sys
from point_transformer_pytorch import PointTransformerLayer
import math
import PointTransformModelDefinition
import ComputeHelpers

# ============================
# Global Variable
# ============================
Num_of_Points = 4
Num_of_Epochs = 200
Learning_Rate = 1e-4

# ============================
# Dataset class
# ============================
class EdgeDataset(torch.utils.data.Dataset):
    def __init__(self, data_dir):
        self.point_files = sorted(glob.glob(os.path.join(data_dir, "points_*.csv")))
        self.edge_files = [pf.replace("points_", "edges_") for pf in self.point_files]

    def __len__(self):
        return len(self.point_files)

    def __getitem__(self, idx):
        points = np.loadtxt(self.point_files[idx], delimiter=',')
        edges = np.loadtxt(self.edge_files[idx], delimiter=',')
    
        N = points.shape[0]
        adj = np.zeros((N, N), dtype=np.float32)

        for edge in edges:
            x1, y1, x2, y2 = edge
            # Find indices of points that match these coords
            idx1 = np.where(np.all(np.isclose(points, [x1, y1], atol=1e-6), axis=1))[0][0]
            idx2 = np.where(np.all(np.isclose(points, [x2, y2], atol=1e-6), axis=1))[0][0]

            adj[idx1, idx2] = 1
            adj[idx2, idx1] = 1  # undirected

        # Normalize points
        # mean = points.mean(axis=0)
        # std = points.std(axis=0) + 1e-6
        # points_norm = (points - mean) / std
        points_norm = points

        # Compute extra features
        dist_to_centroid = ComputeHelpers.compute_distance_to_centroid(points_norm)  # [N,1]
        local_density = ComputeHelpers.compute_local_density(points_norm)           # [N,1]
        angles = ComputeHelpers.compute_angle_from_x_axis(points_norm)              # [N,1]

        # Combine
        #feats = np.concatenate([points_norm, dist_to_centroid, local_density, angles], axis=1)  # [N,5]
        feats = np.concatenate([points_norm], axis=1)

        return (
        torch.tensor(feats, dtype=torch.float32),
        torch.tensor(adj, dtype=torch.float32)
        )
    
        # return (
        #     torch.tensor(points_norm, dtype=torch.float32),  # [N,2]
        #     torch.tensor(adj, dtype=torch.float32)      # [N,N]
        # )

# ============================
# Wrapper for tracing
# ============================
class TracingWrapper(nn.Module):
    def __init__(self, model):
        super().__init__()
        self.model = model

    def forward(self, feats, pos):
        feats = self.model.layer1(feats, pos)
        feats = self.model.layer2(feats, pos)
        feats = feats.squeeze(0)

        f1 = self.model.fc(feats)
        f2 = self.model.fc(feats)
        logits = torch.sigmoid(f1 @ f2.t())
        return logits

# ============================
# Training setup
# ============================
def train(export_type="pytorch"):
    data_dir = "data/csv"
    model_dir = "model"
    os.makedirs(model_dir, exist_ok=True)

    dataset = EdgeDataset(data_dir)
    loader = torch.utils.data.DataLoader(dataset, batch_size=1, shuffle=True)

    model = PointTransformModelDefinition.EdgePredictor().cuda()

    # Diagnostic check
    # for name, param in model.named_parameters():
    #     if not param.is_cuda:
    #         print(f"[DEBUG] {name} is on CPU!")

    optimizer = optim.Adam(model.parameters(), lr=Learning_Rate)
    loss_fn = nn.BCELoss()

    num_epochs = Num_of_Epochs

    for epoch in range(num_epochs):
        total_loss = 0
        for points, adj in loader:
            points = points[0].cuda()  # [N,2]
            adj = adj[0].cuda()        # [N,N]

            model.train()
            optimizer.zero_grad()
            pred_adj = model(points)
            loss = loss_fn(pred_adj, adj)
            loss.backward()
            optimizer.step()

            total_loss += loss.item()

        avg_loss = total_loss / len(loader)
        print(f"Epoch {epoch+1}/{num_epochs} - Avg loss: {avg_loss:.6f}")

    save_path = os.path.join(model_dir, "point_transformer_edge_model.pt")

    if export_type == "pytorch":
        # Regular PyTorch model (state_dict)
        torch.save(model.state_dict(), save_path)
        print(f"PyTorch model state_dict saved to {save_path}")
    elif export_type == "libtorch":
        # TorchScript traced model
        wrapper = TracingWrapper(model)
        N = Num_of_Points  # Match your test input size
        example_feats = torch.randn(1, N, 64).cuda()
        example_pos = torch.randn(1, N, 3).cuda()
        traced_model = torch.jit.trace(wrapper, (example_feats, example_pos))
        torch.jit.save(traced_model, save_path)
        print(f"TorchScript model saved to {save_path}")
    else:
        print(f"Unknown export type: {export_type}")

# ============================
# Main function
# ============================
if __name__ == "__main__":
    # Choose mode via argument
    export_type = "pytorch"  # default
    if len(sys.argv) > 1:
        export_type = sys.argv[1].lower()
    train(export_type)
