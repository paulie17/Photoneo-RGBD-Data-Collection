#pragma once
#include <PhoXi.h>
#include <string>

#define PHO_IGNORE_CV_VERSION_RESTRICTION
#define PHOXI_OPENCV_SUPPORT
#define PHOXI_PCL_SUPPORT
#define OPENCV_TRAITS_ENABLE_DEPRECATED

namespace externalCamera {

/**
 * Calculate and save the depth map from the viewpoint of the external camera
 * from a frame (a praw file) specified on the commandline.
 *
 * Uses the example data in Data folder if no files are specified.
 *
 * @param factory the PhoXi Factory used to create devices
 * @param projectFolderPath path to the project folder with settings and data
 * @param argc,argv the rest of the commandline containing the praw file and
 *        output file
 */
void depthMapFromFile(
        pho::api::PhoXiFactory& factory,
        const std::string& projectFolderPath,
        int argc,
        char* argv[]);

/**
 * Interactively get frames from a connected scanner
 * and the calculated depth map for them.
 *
 * @param factory the PhoXi Factory used to create devices
 * @param projectFolderPath path to the project folder with settings and data
 */
void depthMapInteractive(
        pho::api::PhoXiFactory& factory,
        const std::string& projectFolderPath);

} // namespace externalCamera
