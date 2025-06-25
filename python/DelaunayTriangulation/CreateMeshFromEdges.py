import numpy as np
import os

# Ensure the directory exists
os.makedirs('data/mesh', exist_ok=True)

# Load points (assumes: index x y)
points = np.loadtxt('data/csv/points.csv', delimiter=',')
# Load edges (assumes: index1, index2)
edges = np.loadtxt('data/csv/edges.csv', delimiter=',', dtype=int)

output_path = 'data/mesh/meshFromEdges2D.msh'

with open(output_path, 'w') as f:
    # Mesh format header
    f.write("$MeshFormat\n2.2 0 8\n$EndMeshFormat\n")
    
    # Write nodes
    f.write("$Nodes\n")
    f.write(f"{len(points)}\n")
    for row in points:
        idx, x, y = int(row[0]), row[1], row[2]
        f.write(f"{idx} {x} {y} 0\n")
    f.write("$EndNodes\n")
    
    # Write elements (edges = type 1 = 2-node line)
    f.write("$Elements\n")
    f.write(f"{len(edges)}\n")
    for i, (a, b) in enumerate(edges, 1):
        f.write(f"{i} 1 0 {a} {b}\n")
    f.write("$EndElements\n")

print(f"Mesh file written to {output_path}")

#BACKUP
#save_path = os.path.join(model_dir, "point_transformer_edge_model.pt")
#torch.save(model.state_dict(), save_path)