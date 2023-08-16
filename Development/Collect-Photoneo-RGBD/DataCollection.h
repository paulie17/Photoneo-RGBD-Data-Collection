#define PHO_IGNORE_CV_VERSION_RESTRICTION
#define PHOXI_OPENCV_SUPPORT
#define PHOXI_PCL_SUPPORT
#define OPENCV_TRAITS_ENABLE_DEPRECATED

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <PhoXiAdditionalCamera.h>
#include <PhoXiOpenCVSupport.h>
#include "Calibration.h"
#include "ExternalCamera.h"
#include "ColorPointCloud.h"
#include "FileCamera.h"
#include "Scanner.h"
#include "Util.h"
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <Eigen/Geometry>
#include "yaml-cpp/yaml.h"
#include <experimental/filesystem>
#include <Python.h>

#include <string>
#include <locale>
#include <codecvt>

namespace fs = std::experimental::filesystem;


// basic file operations
#include <iostream>
#include <fstream>


namespace externalCamera {

/**
 * Interactively get frames / images from a connected scanner and external
 * camera and calculate and save different files: 
            1. RGB image
            2. Depth map
            3. Colorized Depth map
            4. Normal Map
            5. Point Cloud
            6. Camera poses
 *
 * @param factory the PhoXi Factory used to create devices
 * @param projectFolderPath path to the project folder with settings and data
 * @param currentScenePath path to the current scene being recorded
 */

void DataCollectionInteractive(
    pho::api::PhoXiFactory& factory,
    const std::string& projectFolderPath,
    const std::string& currentScenePath
);

void DataCollectionAutomatic(
    pho::api::PhoXiFactory& factory,
    const std::string& projectFolderPath,
    const std::string& currentScenePath
);

} // namespace externalCamera