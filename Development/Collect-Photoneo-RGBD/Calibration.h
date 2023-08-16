#pragma once
#include <PhoXi.h>
#include <PhoXiAdditionalCamera.h>
#include <string>
#include <vector>

#define PHO_IGNORE_CV_VERSION_RESTRICTION
#define PHOXI_OPENCV_SUPPORT
#define PHOXI_PCL_SUPPORT
#define OPENCV_TRAITS_ENABLE_DEPRECATED

namespace externalCamera {

struct CalibrationSettings {
    double focalLength = 0;
    double pixelSize = 0;
    std::string markersPositions = std::string();

    static CalibrationSettings load(
            const std::string& settingsPath = std::string());
};

struct CalibrationError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct FileError : public std::runtime_error {
    FileError(const std::string& what, const std::string& path_)
        : std::runtime_error(what)
        , path(path_)
    {
    }

    std::string path;
};

struct BadCalibrationFile : public FileError {
    using FileError::FileError;
};

struct MissingCalibrationFile : public FileError {
    using FileError::FileError;
};


void printCalibration(const pho::api::AdditionalCameraCalibration& calibration);
void printCalibrationError(const MissingCalibrationFile& e);
void printCalibrationError(const BadCalibrationFile& e);

/**
 * Load (already calculated) calibration from the project folder.
 *
 * @param projectFolderPath path to the project folder with settings and data
 *
 * @returns the loaded calibration
 *
 * Throws one Bad- or MissingCalibrationFile error according
 * to the problem with the calibration file.
 */
pho::api::AdditionalCameraCalibration loadCalibration(
        const std::string& projectFolderPath);

/**
 * Perform the actual calibration.
 *
 * @param device the PhoXi device to perform the calibration on
 * @param frames a list of frames from the scanner
 * @param images a list of images from the external camera
 * @param settings  calibration settings loaded from the Settings folder
 *
 * @returns the calculated calibration
 *
 * Throws CalibrationError if the calibration is not successful.
 */
pho::api::AdditionalCameraCalibration calibrate(
        pho::api::PPhoXi device,
        const std::vector<pho::api::Texture32f>& frames,
        const std::vector<pho::api::Texture32f>& images,
        const CalibrationSettings& settings);

/**
 * Perform calibration on files specified on commandline
 *
 * @param factory the PhoXi Factory used to create devices
 * @param projectFolderPath path to the project folder with settings and data
 * @param argc,argv the rest of the commandline containing the calibration files
 */
void calibrateFromFiles(
        pho::api::PhoXiFactory& factory,
        const std::string& projectFolderPath,
        int argc,
        char* argv[]);

/**
 * Interactively get frames / images from a connected scanner and external
 * camera and perform calibration on them.
 *
 * @param factory the PhoXi Factory used to create devices
 * @param projectFolderPath path to the project folder with settings and data
 */
void calibrateInteractive(
        pho::api::PhoXiFactory& factory,
        const std::string& projectFolderPath);

}  // namespace externalCamera
