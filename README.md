# RGBD Data Collection Pipeline for Photoneo MotionCam-3D 

This repository contains an application that allows to capture RGBD data with a Photoneo MotionCam-3D camera together with a ueye industrial camera. \
For each capture the application will save:
- an RGB image
- a depth map
- a normal map
- a point cloud.

The application can be executed in two modalities:
1. Interactive Mode: The application will ask the user whether they want to trigger a scan or end the execution.
2. Automatic Mode: This modality is meant to be used with a rotating table that can be controlled using the libximc library. The application will ask the user how many views of the current user are required and once the input is given the collection will start automatically. Each view corresponds to an angle of rotation of the table equal to 360 divided by number of views.
