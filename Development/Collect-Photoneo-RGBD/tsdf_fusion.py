import open3d
from open3d import pipelines
import numpy as np
import os
import argparse
import yaml
import sys

parser = argparse.ArgumentParser(description='Run 3D reconstruction on a sequence of RGB-D images.')
parser.add_argument('-c', '--camera', type=str, required=True, help='path to the camera intrinsics yaml file')
parser.add_argument('-p', '--poses', type=str, required=True, help='path to the camera poses yaml file')
parser.add_argument('-r', '--rgb_folder', type=str, required=True, help='path to the folder containing the RGB images')
parser.add_argument('-d', '--depth_folder', type=str, required=True, help='path to the folder containing the depth images')
args = parser.parse_args()

camera_file = args.camera
poses_file = args.poses
rgb_folder = args.rgb_folder
depth_folder = args.depth_folder

if not os.path.exists(camera_file):
    sys.exit(f"The camera file {camera_file} does not exist.")

if not os.path.exists(poses_file):
    sys.exit(f"The poses file {poses_file} does not exist.")

if not os.path.exists(rgb_folder):
    sys.exit(f"The RGB folder {rgb_folder} does not exist.")

if not os.path.exists(depth_folder):
    sys.exit(f"The depth folder {depth_folder} does not exist.")

with open(camera_file, 'r') as f:
    camera_data = yaml.load(f, Loader=yaml.FullLoader)

# Extract the camera intrinsic parameters
fx = camera_data['camera_matrix']['data'][0]
fy = camera_data['camera_matrix']['data'][4]
cx = camera_data['camera_matrix']['data'][2]
cy = camera_data['camera_matrix']['data'][5]
image_width = camera_data['image_width']
image_height = camera_data['image_height']

# Create the PinholeCameraIntrinsic object
intrinsic = open3d.camera.PinholeCameraIntrinsic(image_width, image_height, fx, fy, cx, cy)
intrinsic.intrinsic_matrix = [[fx, 0, cx], [0, fy, cy], [0, 0, 1]]
print(intrinsic.intrinsic_matrix)
cam = open3d.camera.PinholeCameraParameters()
cam.intrinsic = intrinsic

with open(poses_file, 'r') as f:
    poses_data = yaml.load(f, Loader=yaml.FullLoader)

volume = pipelines.integration.ScalableTSDFVolume(voxel_length=0.5 / 512.0, sdf_trunc=0.005,
                                                 color_type=pipelines.integration.TSDFVolumeColorType.RGB8)

for pose_index, pose_info in poses_data.items():
    quaternion = pose_info['camera_to_world']['quaternion']
    translation = pose_info['camera_to_world']['translation']
    M = np.eye(4)

    # Note: You should normalize the quaternion before converting it to a rotation matrix
    quaternion = np.array([quaternion['w'], quaternion['x'], quaternion['y'], quaternion['z']])
    quaternion /= np.linalg.norm(quaternion)

    M[:3, :3] = open3d.geometry.get_rotation_matrix_from_quaternion(quaternion)
    M[:3, 3] = [translation['x'], translation['y'], translation['z']]

    color_path = os.path.join(rgb_folder, f'{pose_index:06d}.png')
    depth_path = os.path.join(depth_folder, f'{pose_index:06d}.png')

    if not os.path.exists(color_path) or not os.path.exists(depth_path):
        print(f"Missing data for pose {pose_index}, skipping.")
        continue

    print(M)
    color = open3d.io.read_image(color_path)
    depth = open3d.io.read_image(depth_path)
    rgbd = open3d.geometry.RGBDImage.create_from_color_and_depth(color, depth, depth_trunc=1.0, convert_rgb_to_intensity=False)
    
    volume.integrate(rgbd, cam.intrinsic, np.linalg.inv(M))
    print(f"Processed pose {pose_index}")

mesh = volume.extract_triangle_mesh()
mesh.compute_vertex_normals()

pcd = open3d.geometry.PointCloud()
pcd.points = mesh.vertices

output_folder = os.path.dirname(camera_file)
open3d.io.write_triangle_mesh(os.path.join(output_folder, 'fusion_mesh.ply'), mesh, write_ascii=False,
                              compressed=False, write_vertex_normals=True, write_vertex_colors=True,
                              write_triangle_uvs=True, print_progress=True)
open3d.io.write_point_cloud(os.path.join(output_folder, 'fusion_pointcloud.pcd'), pcd, write_ascii=False,
                            compressed=False, print_progress=False)

open3d.visualization.draw_geometries([mesh])
