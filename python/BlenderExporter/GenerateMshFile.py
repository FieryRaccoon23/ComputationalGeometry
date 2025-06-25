import bpy

import bpy
import mathutils

def export_msh_normalized(filepath):
    obj = bpy.context.active_object
    if obj is None or obj.type != 'MESH':
        print("❌ Please select a mesh object.")
        return
    
    mesh = obj.to_mesh(preserve_all_data_layers=True, depsgraph=bpy.context.evaluated_depsgraph_get())
    mesh.calc_loop_triangles()

    coords = [v.co.copy() for v in mesh.vertices]

    # Compute bounding box center
    min_coord = mathutils.Vector((
        min(v.x for v in coords),
        min(v.y for v in coords),
        min(v.z for v in coords)
    ))
    max_coord = mathutils.Vector((
        max(v.x for v in coords),
        max(v.y for v in coords),
        max(v.z for v in coords)
    ))
    center = (min_coord + max_coord) / 2.0

    # Compute scale factor to fit into [-1, 1]
    size = max_coord - min_coord
    max_size = max(size.x, size.y, size.z)
    scale = 1.0 if max_size == 0 else 2.0 / max_size  # 2.0 because range is -1 to 1

    # Normalize coordinates: center and scale
    norm_coords = [
        (v - center) * scale
        for v in coords
    ]

    with open(filepath, 'w') as f:
        f.write("$MeshFormat\n")
        f.write("2.2 0 8\n")
        f.write("$EndMeshFormat\n")

        f.write("$Nodes\n")
        f.write(f"{len(norm_coords)}\n")
        for idx, v in enumerate(norm_coords, start=1):
            f.write(f"{idx} {v.x} {v.y} {v.z}\n")
        f.write("$EndNodes\n")

        f.write("$Elements\n")
        f.write(f"{len(mesh.loop_triangles)}\n")
        for idx, tri in enumerate(mesh.loop_triangles, start=1):
            v1 = tri.vertices[0] + 1
            v2 = tri.vertices[1] + 1
            v3 = tri.vertices[2] + 1
            f.write(f"{idx} 2 0 {v1} {v2} {v3}\n")
        f.write("$EndElements\n")

    print(f"✅ Normalized mesh exported to {filepath}")

def export_msh(filepath):
    obj = bpy.context.active_object
    
    if obj is None or obj.type != 'MESH':
        print("❌ Please select a mesh object.")
        return
    
    mesh = obj.to_mesh(preserve_all_data_layers=True, depsgraph=bpy.context.evaluated_depsgraph_get())
    mesh.calc_loop_triangles()

    with open(filepath, 'w') as f:
        # Write MeshFormat
        f.write("$MeshFormat\n")
        f.write("2.2 0 8\n")
        f.write("$EndMeshFormat\n")
        
        # Write Nodes
        f.write("$Nodes\n")
        num_verts = len(mesh.vertices)
        f.write(f"{num_verts}\n")
        for idx, v in enumerate(mesh.vertices, start=1):
            f.write(f"{idx} {v.co.x} {v.co.y} {v.co.z}\n")
        f.write("$EndNodes\n")
        
        # Write Elements (triangles)
        f.write("$Elements\n")
        num_tris = len(mesh.loop_triangles)
        f.write(f"{num_tris}\n")
        for idx, tri in enumerate(mesh.loop_triangles, start=1):
            # Mesh format: id type flags v1 v2 v3
            # We'll use type=2 (triangle), flags=0
            v1 = tri.vertices[0] + 1
            v2 = tri.vertices[1] + 1
            v3 = tri.vertices[2] + 1
            f.write(f"{idx} 2 0 {v1} {v2} {v3}\n")
        f.write("$EndElements\n")

    print(f"✅ Mesh exported to {filepath}")

# Example usage:
export_msh_normalized('D:/ComputationalGeometry/data/Blender/my_mesh.msh')
