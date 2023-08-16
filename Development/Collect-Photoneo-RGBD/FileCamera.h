#ifndef  _EXTERNALCAMERA_EXAMPLE_FILE_CAMERA_H
#define  _EXTERNALCAMERA_EXAMPLE_FILE_CAMERA_H 1

#include <PhoXi.h>
#include <PhoXiAdditionalCamera.h>
#include <string>
#include <vector>


namespace externalCamera {

/**
 * A RAII class to correctly handle FileCamera detach.
 */
class AttachedFileCamera {
public:
    AttachedFileCamera(
            pho::api::PhoXiFactory& factory,
            const std::vector<std::string>& filePaths);
    ~AttachedFileCamera();

    /**
     * Connect to the FileCamera and return a PhoXi device.
     *
     * @returns a PhoXi device representing the attached FileCamera
     */
    pho::api::PPhoXi connect();

private:
    std::string name;
    pho::api::PhoXiFactory& factory;
    const std::string FileCameraHwIdentification = "ExampleFileCamera";
};

} // namespace externalCamera
#endif //_EXTERNALCAMERA_EXAMPLE_FILE_CAMERA_H
