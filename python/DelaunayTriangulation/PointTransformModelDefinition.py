from point_transformer_pytorch import PointTransformerLayer
import torch
# ============================
# Model definition
# ============================
class EdgePredictor(torch.nn.Module):
    def __init__(self, dim=64, input_dim=2):
        super().__init__()
        self.input_proj = torch.nn.Linear(input_dim, dim)
        self.layer1 = PointTransformerLayer(dim=dim, pos_mlp_hidden_dim=64)
        self.layer2 = PointTransformerLayer(dim=dim, pos_mlp_hidden_dim=64)
        self.layer3 = PointTransformerLayer(dim=dim, pos_mlp_hidden_dim=64)
        self.fc = torch.nn.Linear(dim, dim)

    def forward(self, x):
        N = x.shape[0]
        feats = self.input_proj(x).unsqueeze(0)  # [1,N,64]
        pos = x[:, :2].unsqueeze(0)  # Assuming first two features are normalized x,y
        pos = torch.cat([pos, torch.zeros(pos.shape[0], pos.shape[1], 1, device=pos.device)], dim=-1)  # [1,N,3]

        feats = self.layer1(feats, pos)
        feats = self.layer2(feats, pos)
        feats = self.layer3(feats, pos)
        feats = feats.squeeze(0)

        f1 = self.fc(feats)
        f2 = self.fc(feats)
        logits = torch.sigmoid(f1 @ f2.t())
        return logits