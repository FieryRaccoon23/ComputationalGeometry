import torch
from point_transformer_pytorch import PointTransformerLayer

attn = PointTransformerLayer(
    dim = 128,
    pos_mlp_hidden_dim = 64,
    attn_mlp_hidden_mult = 4,
    num_neighbors = 16          # only the 16 nearest neighbors would be attended to for each point
)

feats = torch.randn(1, 2048, 128)
pos = torch.randn(1, 2048, 3)
mask = torch.ones(1, 2048).bool()

attn(feats, pos, mask = mask) # (1, 16, 128)