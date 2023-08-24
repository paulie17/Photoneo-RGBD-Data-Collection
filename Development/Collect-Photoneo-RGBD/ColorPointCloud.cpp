#include "ColorPointCloud.h"


namespace externalCamera {

    pho::api::Mat2D<pho::api::ColorRGB_16> colorPointCloudTexture(
            pho::api::PFrame frame,
            cv::Mat extCameraImage,
            const pho::api::AdditionalCameraCalibration& calibration
            ) {
        std::cout << "The alignment of color texture to the point cloud is in progress..." << std::endl;
        
        auto TextureSize = frame->GetResolution();
        const int textureWidth = TextureSize.Width;
        const int textureHeight = TextureSize.Height;

        
        // Set the deafult value RGB(0,0,0) of the texture
        cv::Mat cvTextureRGB = cv::Mat( textureHeight, textureWidth, 
                                        CV_8UC3, cv::Scalar(0, 0, 0));

        // Parameters of computation-----------------------------------------
        cv::Mat MCWCMatrix = cv::Mat(4, 4, cv::DataType<float>::type);
        cv::Mat trans = cv::Mat::eye(4, 4, cv::DataType<float>::type);

        // Set 'trans' matrix == rotation and translation together in 4x4 matrix
        const auto& transformRotation = calibration.CoordinateTransformation.Rotation;
        for (int y = 0; y < transformRotation.Size.Height; ++y) {
            for (int x = 0; x < transformRotation.Size.Width; ++x) {
                trans.at<float>(y, x) = transformRotation[y][x];
            }
        }
        trans.at<float>(0, 3) = calibration.CoordinateTransformation.Translation.x;
        trans.at<float>(1, 3) = calibration.CoordinateTransformation.Translation.y;
        trans.at<float>(2, 3) = calibration.CoordinateTransformation.Translation.z;
        MCWCMatrix = trans.inv();

        // Set projection parameters from CameraMatrix of the external camera
        const auto& cameraMatrix = calibration.CalibrationSettings.CameraMatrix;
        const float fx = cameraMatrix[0][0];
        const float fy = cameraMatrix[1][1];
        const float cx = cameraMatrix[0][2];
        const float cy = cameraMatrix[1][2];

        // Set distortion coefficients of the external camera
        const auto& distortionCoeffs = calibration.CalibrationSettings.DistortionCoefficients;
        const float k1 = distortionCoeffs[0];
        const float k2 = distortionCoeffs[1];
        const float p1 = distortionCoeffs[2];
        const float p2 = distortionCoeffs[3];
        const float k3 = distortionCoeffs[4];

        // Set the resolution of external camera
        const float width = calibration.CameraResolution.Width;
        const float height = calibration.CameraResolution.Height;

        // End of setting the parameters--------------------------------------

        // Loop through the PointCloud
        for (int y = 0; y < frame->PointCloud.Size.Height; ++y) {
            for (int x = 0; x < frame->PointCloud.Size.Width; ++x) {

                    // Point in homogeneous coordinates
                    cv::Mat vertexMC = cv::Mat(4, 1, cv::DataType<float>::type);
                    vertexMC.at<float>(0, 0) =
                            frame->PointCloud[y][x].x;
                    vertexMC.at<float>(1, 0) =
                            frame->PointCloud[y][x].y;
                    vertexMC.at<float>(2, 0) =
                            frame->PointCloud[y][x].z;
                    vertexMC.at<float>(3, 0) = 1;

                    // Perform the transformation into the coordinates of external camera
                    cv::Mat vertexWC = MCWCMatrix * vertexMC;

                    // Projection from 3D to 2D
                    cv::Mat camPt = cv::Mat(2, 1, cv::DataType<float>::type);
                    camPt.at<float>(0, 0) = vertexWC.at<float>(0, 0) / vertexWC.at<float>(2, 0);
                    camPt.at<float>(1, 0) = vertexWC.at<float>(1, 0) / vertexWC.at<float>(2, 0);

                    // The distortion of the external camera need to be taken into account for details see e.g.
                    // https://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
                    pho::api::float32_t xx, xy2, yy, r2, r4, r6;

                    xx = camPt.at<float>(0, 0) * camPt.at<float>(0, 0);
                    xy2 = 2 * camPt.at<float>(0, 0) * camPt.at<float>(1, 0);
                    yy = camPt.at<float>(1, 0) * camPt.at<float>(1, 0);

                    r2 = xx + yy;
                    r4 = r2 * r2;
                    r6 = r4 * r2;

                    // Constant related to the radial distortion
                    pho::api::float32_t c = (1 + k1 * r2 + k2 * r4 + k3 * r6);

                    // Both radial and tangential distortion are applied
                    cv::Mat dist = cv::Mat(2, 1, cv::DataType<float>::type);
                    dist.at<float>(0, 0) = c * camPt.at<float>(0, 0) +
                                        p1 * xy2 + p2 * (r2 + 2 * xx);
                    dist.at<float>(1, 0) = c * camPt.at<float>(1, 0) +
                                        p1 * (r2 + 2 * yy) + p2 * xy2;

                    // Final film coordinates
                    const float fxPosition = (dist.at<float>(0, 0) * fx + cx);
                    const float fyPosition = (dist.at<float>(1, 0) * fy + cy);

                    const int i = std::round(fxPosition);
                    const int j = std::round(fyPosition);

                    if (i >= 0 && i < width && j >= 0 && j < height) {
                        // The loaded extCameraImage has channels ordered like BGR
                        auto yr = cvTextureRGB.ptr<uint8_t>(y);
                        auto jr = extCameraImage.ptr<uint8_t>(j);

                        // Set R - 0th channel
                        yr[3 * x + 0] = jr[3 * i + 2];
                        // Set G - 1st channel
                        yr[3 * x + 1] = jr[3 * i + 1];
                        // Set B - 2nd channel
                        yr[3 * x + 2] = jr[3 * i + 0];
                }
            }
        }

        pho::api::Mat2D<pho::api::ColorRGB_16> textureRGB(TextureSize);
        ConvertOpenCVMatToMat2D(cvTextureRGB, textureRGB);
        return textureRGB;
    }

    void saveColorPointCloud(
            const std::string& path,
            const pho::api::PFrame frame) {
        if (!frame->SaveAsPly(path, true, true))
            throw std::runtime_error(
                    "Failed saving point cloud with color texture to file " + path);

        std::cout << "Point cloud with color texture saved to " << path << std::endl;
    }

    cv::Mat loadExternalCameraImage(const std::string& path)
    {
        auto image = cv::imread(path, cv::IMREAD_COLOR);
        if (image.empty())
            throw std::runtime_error("Error loading image " + path);

        return image;
    }


} // namespace externalCamera
