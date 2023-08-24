# RGBD Data Collection Pipeline for Photoneo MotionCam-3D 

This repository contains an application that allows to capture RGBD data with a Photoneo MotionCam-3D camera together with a ueye industrial camera. \
For each capture the application will save:
- an RGB image
- a depth map
- a normal map
- a point cloud.
- the camera pose.

The application can be executed in two modalities:
1. Interactive Mode: The application will ask the user whether they want to trigger a scan or end the execution.
2. Automatic Mode: This modality is meant to be used with a rotating table that can be controlled using the libximc library. The application will ask the user how many views of the current scene are required and once the input is given the collection will start automatically. Each view corresponds to an angle of rotation of the table equal to 360 divided by number of views.

## Requirements

- [docker](https://docs.docker.com/install/)
- [nvidia-docker2](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html#docker)
- [docker-compose](https://docs.docker.com/compose/install/)
- [PhoXi Control installer](https://www.photoneo.com/downloads/phoxi-control/)
- [phoxi_docker](https://github.com/paulie17/phoxi_docker)

Tested on Ubuntu 20.04 with NVIDIA driver version 530 using a Photoneo MotionCam-3D.

## Instructions
- Place the ueye driver installer file ueye_4.95.0.1134_amd64.run in the folder Development/ueye/ .
- Make sure to have already built the docker image phoxi_docker from the phoxi_docker repo. The Dockerfile in this repo builds on top of phoxi_docker.
- Run, in the same directory  as the Dockerfile and docker-compose.yml.

        $ docker compose build phoxi_data_collection

    to build the phoxi_data_collection docker image.
- Run
  
        $ xhost +local:docker
        
    to make sure the GUI of PhoXiControl can be visualized when running it from a docker container.
- Run, in the same directory
  
        $ docker compose run phoxi_data_collection

    to create and run a container from the phoxi_data_collection image.
- Run, when the container is running:
  
        $ cd Collect-Photoneo-RGBD
        $ ./build/PhotoneoDataCollection

    to run the application. The execution will wait for PhoXiControl to be running and to be connected to the camera.
- In a separate terminal run:

        $ docker exec -it container_name bash
  
  replacing 'container_name' with the name of the container running the phoxi_data_collection image. This will run bash from the running container.
  Afterwards you can run

        $ PhoXiControl

  to run the PhoXiControl GUI. Select the device.
- In the terminal where PhotoneoDataCollection is running follow the instructions on screen to choose automatic or interactive mode and continue with data recording.
- The data that is being recorded will be saved following the structure:

        Development/
          data/
            scene_0/
              rgb/
                000000.png
                000001.png
                  ...
              depth/
                000000.png
                000001.png
                  ...
              normals/
                000000.png
                000001.png
                  ...
              pointclouds/
                000000.ply
                000001.ply
                  ...
              camera_info.yaml
              pose_data.yaml
            scene_1/
              rgb/
                000000.png
                000001.png
                  ...
              depth/
                000000.png
                000001.png
                  ...
              normals/
                000000.png
                000001.png
                  ...
              pointclouds/
                000000.ply
                000001.ply
                  ...
              camera_info.yaml
              pose_data.yaml
            ...
          




  
