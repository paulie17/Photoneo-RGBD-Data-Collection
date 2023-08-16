import os
import numpy as np
import quaternion

# Function to read a transformation matrix from a file
def read_transform(file_path):
    with open(file_path, 'r') as f:
        lines = f.readlines()
    transform = np.zeros((4, 4))
    for i, line in enumerate(lines):
        vals = line.strip().split(' ')
        transform[i, :] = [float(val) for val in vals]
    return transform

# Function to convert a transformation matrix to a pose in the format "id tx ty tz qx qy qz qw"
def transform_to_pose(transform, pose_id):
    translation = transform[:3, 3]
    rotation = quaternion.from_rotation_matrix(transform[:3, :3])
    pose = np.concatenate(([pose_id], translation, rotation.components))
    return ' '.join([str(val) for val in pose])

# Loop through all files with names like "000000_pose.txt", "000001_pose.txt", etc.
poses = []
for i in range(100000):
    file_path = f'{i:06d}_pose.txt'
    if os.path.exists(file_path):
        transform = read_transform(file_path)
        pose = transform_to_pose(transform, i)
        poses.append(pose)

# Write the poses to a file named "poses.txt"
with open('poses.txt', 'w') as f:
    f.write('\n'.join(poses))

