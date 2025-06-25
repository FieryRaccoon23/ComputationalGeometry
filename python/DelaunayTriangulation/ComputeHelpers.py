import numpy as np

# ============================
# Dataset helper functions
# ============================
def compute_distance_to_centroid(points):
    centroid = points.mean(axis=0)
    distances = np.linalg.norm(points - centroid, axis=1, keepdims=True)
    return distances  # shape [N,1]

def compute_local_density(points, radius=0.1):
    N = points.shape[0]
    density = np.zeros((N,1), dtype=np.float32)
    for i in range(N):
        dists = np.linalg.norm(points - points[i], axis=1)
        density[i] = np.sum(dists < radius) - 1  # exclude self
    return density

def compute_angle_from_x_axis(points):
    angles = np.arctan2(points[:,1], points[:,0])
    angles = angles.reshape(-1,1).astype(np.float32)
    return angles