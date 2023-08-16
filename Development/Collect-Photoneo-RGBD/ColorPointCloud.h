#pragma once

#define PHO_IGNORE_CV_VERSION_RESTRICTION
#define PHOXI_OPENCV_SUPPORT
#define PHOXI_PCL_SUPPORT
#define OPENCV_TRAITS_ENABLE_DEPRECATED

#include <PhoXi.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <PhoXiOpenCVSupport.h>

namespace externalCamera {

/**
 * Calculate the texture for the point coud for a scanner frame from
 * an external camera image.
 *
 * @param frame the frame acquired by the Photoneo Scanner/MotionCam-3D
 * @param extCameraImage the RGB image acquired with the external camera
 * @param calibration the Photoneo Scanner calibration settings to retrieve the camera intrinsic parameters
 * @param PhoXiDevice the current phoxidevice in use
 */

pho::api::Mat2D<pho::api::ColorRGB_32f> colorPointCloudTexture(
            pho::api::PFrame frame,
            cv::Mat extCameraImage,
            const pho::api::AdditionalCameraCalibration& calibration,
            const pho::api::PPhoXi& PhoXiDevice);
/**
 * Save the color Point Cloud as a .ply file
 *
 * @param path file path for the ply file
 * @param frame the current frame containing the Point Cloud
 */

void saveColorPointCloud(
            const std::string& path,
            const pho::api::PFrame frame);

} // namespace externalCamera
