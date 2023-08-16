#include <PhoXi.h>
#include <thread>
#include "DataCollection.h"
#include "Util.h"
#include "ExternalCamera.h"

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

int main(int argc, char *argv[]) {


    using namespace externalCamera;
    initializeIDS();

    std::string dataFolderPath = "/root/data";
    
    // Check if "data" folder exists, and create it if not
    if (!fs::exists(dataFolderPath)) {
        if (fs::create_directory(dataFolderPath)) {
            std::cout << "Created 'data' folder." << std::endl;
        } else {
            std::cerr << "Failed to create 'data' folder." << std::endl;
            return 1;
        }
    }
    
    int maxSceneNumber = -1;
    
    // Find the highest scene number
    for (const auto& entry : fs::directory_iterator(dataFolderPath)) {
        if (fs::is_directory(entry.status()) && entry.path().filename().string().find("scene_") == 0) {
            std::string sceneName = entry.path().filename().string();
            int sceneNumber = std::stoi(sceneName.substr(6));
            maxSceneNumber = std::max(maxSceneNumber, sceneNumber);
        }
    }
    
    // Create a new scene folder
    int newSceneNumber = maxSceneNumber + 1;
    std::string newSceneFolderName = "scene_" + std::to_string(newSceneNumber);
    std::string newSceneFolderPath = dataFolderPath + "/" + newSceneFolderName;
    
    if (fs::create_directory(newSceneFolderPath)) {
        std::cout << "Created '" << newSceneFolderName << "' folder." << std::endl;
    } else {
        std::cerr << "Failed to create '" << newSceneFolderName << "' folder." << std::endl;
        return 1;
    }
    
    // Create subdirectories inside the new scene folder
    std::vector<std::string> subdirectories = {"rgb", "depth", "normals", "pointclouds"};
    
    for (const auto& subdir : subdirectories) {
        std::string subdirectoryPath = newSceneFolderPath + "/" + subdir;
        if (fs::create_directory(subdirectoryPath)) {
            std::cout << "Created '" << subdir << "' subfolder." << std::endl;
        } else {
            std::cerr << "Failed to create '" << subdir << "' subfolder." << std::endl;
            return 1;
        }
    }


    pho::api::PhoXiFactory factory;

    std::cout << "Data Collection pipeline for Photoneo Scanner/MotionCam-3D" << std::endl;
    std::cout << std::endl;

    std::cout << "Waiting for PhoXi Control" <<std::endl;
    while (!factory.isPhoXiControlRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "PhoXi Control Version: "
            << factory.GetPhoXiControlVersion() << std::endl;
    std::cout << "PhoXi API Version: "
            << factory.GetAPIVersion() << std::endl << std::endl;

    auto projectFolderPath = path::getProjectFolder(argv[0]);

    auto selectAction = []() {
        return ask<int>(
                "Please select which action would you like to perform:",
                {
                        {1, "Interactive data collection"},
                        {2, "Automatic data collection"}
                });
    };

    switch (selectAction()) {
            case 1:
                try {
                    DataCollectionInteractive(factory, projectFolderPath, newSceneFolderPath);
                }
                catch (std::runtime_error& e) {
                    std::cout << "Error occured: " << std::endl;
                    std::cout << "\t" << e.what() << std::endl;;
                }
                break;
            case 2:
                try {
                    DataCollectionAutomatic(factory, projectFolderPath, newSceneFolderPath);
                }
                catch (std::runtime_error& e) {
                    std::cout << "Error occured: " << std::endl;
                    std::cout << "\t" << e.what() << std::endl;;
                }
            }


    
    return 0;
}
