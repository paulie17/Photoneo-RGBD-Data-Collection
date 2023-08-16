#include "DepthMap.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <PhoXiAdditionalCamera.h>
#include <PhoXiOpenCVSupport.h>
#include "Calibration.h"
#include "FileCamera.h"
#include "Scanner.h"
#include "Util.h"

namespace externalCamera {


pho::api::DepthMap32f getDepthMap(
        pho::api::PPhoXi device,
        const pho::api::AdditionalCameraCalibration& calibration) {
    pho::api::DepthMap32f depthMap;

    pho::api::AdditionalCamera::Aligner aligner(device, calibration);
    if (!aligner.GetAlignedDepthMap(depthMap))
        throw std::runtime_error("Computation of aligned depth map failed!");

    return depthMap;
}

void saveDepthMap(
        const std::string& path,
        const pho::api::DepthMap32f& depthMap) {
    cv::Mat cvDepthMap;
    ConvertMat2DToOpenCVMat(depthMap, cvDepthMap);

    if (!imwrite(path, cvDepthMap))
        throw std::runtime_error("Failed saving depth map to file " + path);

    std::cout << "Depth map saved to " << path << std::endl;
}


void depthMapFromFile(
        pho::api::PhoXiFactory& factory,
        const std::string& projectFolderPath,
        int argc,
        char* argv[]) {
    std::vector<std::string> prawNames;
    std::string outputPath = "fileCamera.jpg";

    if (argc > 0)
        prawNames.push_back(argv[0]);

    if (argc > 1)
        outputPath = argv[1];

    // Use Data/1.praw if no praw file specified
    if (!prawNames.size())
        prawNames.push_back(
                path::join(path::dataFolder(projectFolderPath), "1.praw"));

    // Load calibration info
    auto calibration = loadCalibration(projectFolderPath);
    printCalibration(calibration);

    // Attach praw file as FileCamera
    AttachedFileCamera fileCamera{factory, prawNames};
    auto device = fileCamera.connect();

    // We just need to trigger a scan, but don't need the result, because the
    // aligner will use the data present in PhoXiControl
    triggerScanAndGetFrame(device);

    auto depthMap = getDepthMap(device, calibration);
    saveDepthMap(outputPath, depthMap);

    // Log out the device from PhoXi Control
    device->Disconnect(true);
}

void depthMapInteractive(
        pho::api::PhoXiFactory& factory,
        const std::string& projectFolderPath) {
    std::string filePrefix = "device_";
    int count = 0;

    // Load calibration info
    auto calibration = loadCalibration(projectFolderPath);
    printCalibration(calibration);

    // Connect to a scanner
    auto device = selectAndConnectDevice(factory);

    auto shouldContinue = []() {
        return  1 == ask<int>("Do you want to continue?", {
            {1, "Yes, trigger a new scan and compute depth map"},
            {2, "No, disconnect the scanner"}
        });
    };

    while (shouldContinue()) {
        // We just need to trigger a scan, but don't need the result, because
        // the aligner will use the data present in PhoXiControl
        triggerScanAndGetFrame(device);
        auto depthMap = getDepthMap(device, calibration);

        auto outputPath = filePrefix + std::to_string(++count) + ".jpg";
        saveDepthMap(outputPath, depthMap);
    }

    disconnectOrLogOut(device);
}

} // namespace externalCamera
