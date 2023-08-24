#include "DataCollection.h"

namespace externalCamera {
    using namespace std;    
    
    bool charuco_marker_pose_estimation(cv::Mat &bw, 
                                        pho::api::PhoXiCalibrationSettings &CalibrationSettings,
                                        const std::string& currentScenePath,
                                        int posecount) {
    // Charuco marker detection & pose estimation
    cv::Vec3d rvec, tvec;
    bw.convertTo(bw,CV_8UC1);
    
    float cameraMatrixData[9] = {
        CalibrationSettings.CameraMatrix[0][0], CalibrationSettings.CameraMatrix[0][1], CalibrationSettings.CameraMatrix[0][2],
        CalibrationSettings.CameraMatrix[1][0], CalibrationSettings.CameraMatrix[1][1], CalibrationSettings.CameraMatrix[1][2],
        0, 0, 1
    };
    cv::Mat cameraMatrix(3, 3, CV_32F, cameraMatrixData);

    std::vector<double> distCoeffs = CalibrationSettings.DistortionCoefficients;
    float distCoeffsData[14] = {
        distCoeffs[0], distCoeffs[1], distCoeffs[2], distCoeffs[3], distCoeffs[4], 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    cv::Mat distCoeffsMat(1, 14, CV_32F, distCoeffsData);

    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<cv::Point2f> charucoCorners;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(11, 7, 0.035f, 0.025f, dictionary);
    cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
    std::vector<int> markerIds;

    cv::aruco::detectMarkers(bw, board->dictionary, markerCorners, markerIds, params);

    if (!markerIds.empty()) {
        std::vector<int> charucoIds;
        cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, bw, board, charucoCorners, charucoIds, cameraMatrix, distCoeffsMat);

        if (!charucoIds.empty()) {
            bool valid = cv::aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distCoeffsMat, rvec, tvec);

            if (valid) {
                cv::cvtColor(bw,bw,cv::COLOR_GRAY2RGB);
                cv::aruco::drawAxis(bw, cameraMatrix, distCoeffsMat, rvec, tvec, 0.1f);

                // // Extract rotation matrix from the rotation vector
                // cv::Mat rotationMatrix;
                // cv::Rodrigues(rvec, rotationMatrix);

                // // Create a 4x4 transformation matrix [R | t; 0 0 0 1]
                // cv::Mat transformationMatrix(4, 4, CV_32F);
                // cv::Mat translationVector = cv::Mat::zeros(3, 1, CV_32F);
                // for (int i = 0; i < 3; ++i) {
                //     translationVector.at<float>(i,0) = tvec[i];
                // }                 
                // // Extract translation vector

                // // Compose the transformation matrix
                // cv::Mat upperLeft = transformationMatrix(cv::Rect(0, 0, 3, 3));
                // rotationMatrix.copyTo(upperLeft);
                // translationVector.copyTo(transformationMatrix(cv::Rect(3, 0, 1, 3)));

                // // Add the [0, 0, 0, 1] row to complete the transformation matrix
                // transformationMatrix.at<float>(3, 3) = 1.0f;

                // // Invert the transformation matrix
                // cv::Mat invertedTransformationMatrix = transformationMatrix.inv();

                // // Extract inverted translation vector and quaternion from the inverted matrix

                // // Extract the inverted translation vector
                // cv::Mat invertedTranslationVector_mat = invertedTransformationMatrix(cv::Rect(3, 0, 1, 3));
                // cv::Vec3d invertedTranslationVector_vec(invertedTranslationVector_mat);

                // // Extract the inverted rotation matrix from the upper-left 3x3 block
                // cv::Mat invertedRotationMatrix = invertedTransformationMatrix(cv::Rect(0, 0, 3, 3));

                // // Convert the inverted rotation matrix to an Eigen matrix for quaternion conversion
                // Eigen::Matrix3d invertedEigenRotationMatrix;
                // cv::cv2eigen(invertedRotationMatrix, invertedEigenRotationMatrix);
                
                // // Compute the inverted quaternion
                // Eigen::Quaterniond invertedQuaternion = Eigen::Quaterniond(invertedEigenRotationMatrix);

                // YAML::Emitter emitter;
                // emitter << YAML::Key << YAML::BeginMap;
                // emitter << YAML::Key << posecount << YAML::Value << YAML::BeginMap;
                // emitter << YAML::Key << "camera_to_world" << YAML::Value << YAML::BeginMap;
                // emitter << YAML::Key << "quaternion" << YAML::Value << YAML::BeginMap;
                // emitter << YAML::Key << "w" << YAML::Value << invertedQuaternion.w();
                // emitter << YAML::Key << "x" << YAML::Value << invertedQuaternion.x();
                // emitter << YAML::Key << "y" << YAML::Value << invertedQuaternion.y();
                // emitter << YAML::Key << "z" << YAML::Value << invertedQuaternion.z();
                // emitter << YAML::EndMap;
                // emitter << YAML::Key << "translation" << YAML::Value << YAML::BeginMap;
                // emitter << YAML::Key << "x" << YAML::Value << invertedTranslationVector_vec[0];
                // emitter << YAML::Key << "y" << YAML::Value << invertedTranslationVector_vec[1];
                // emitter << YAML::Key << "z" << YAML::Value << invertedTranslationVector_vec[2];
                // emitter << YAML::EndMap;

                //pose manipulation
                cv::Mat R;
                cv::Rodrigues(rvec, R);
                Eigen::Matrix3d mat;
                cv::Mat M(3,4, CV_32F);
                cv::Mat add = (cv::Mat_<double>(1,4) << 0, 0, 0, 1);
                cv::hconcat(R, tvec, M);
                cv::Mat M4(4,4, CV_32F),M4inv(4,4, CV_32F);
                cv::vconcat(M,add,M4);
                M4inv = M4.inv();
                cv::Mat rvecinv,tvecinv;
                tvecinv = M4inv(cv::Range(0,3),cv::Range(3,4));
                cv::Vec3d TVECINV(tvecinv);
                rvecinv = M4inv(cv::Range(0,3), cv::Range(0,3));
                cv2eigen(rvecinv, mat);
                Eigen::Quaterniond EigenQuat(mat);
                Eigen::Quaterniond quatinv;
                quatinv = EigenQuat;

                YAML::Emitter emitter;
                emitter << YAML::Key << YAML::BeginMap;
                emitter << YAML::Key << posecount << YAML::Value << YAML::BeginMap;
                emitter << YAML::Key << "camera_to_world" << YAML::Value << YAML::BeginMap;
                emitter << YAML::Key << "quaternion" << YAML::Value << YAML::BeginMap;
                emitter << YAML::Key << "w" << YAML::Value << quatinv.w();
                emitter << YAML::Key << "x" << YAML::Value << quatinv.x();
                emitter << YAML::Key << "y" << YAML::Value << quatinv.y();
                emitter << YAML::Key << "z" << YAML::Value << quatinv.z();
                emitter << YAML::EndMap;
                emitter << YAML::Key << "translation" << YAML::Value << YAML::BeginMap;
                emitter << YAML::Key << "x" << YAML::Value << TVECINV[0];
                emitter << YAML::Key << "y" << YAML::Value << TVECINV[1];
                emitter << YAML::Key << "z" << YAML::Value << TVECINV[2];
                emitter << YAML::EndMap;

                ofstream myfile;

                myfile.open(currentScenePath + "/pose_data.yaml", ios::app);
                myfile << emitter.c_str() << endl;
                myfile.close();

                std::ifstream src("camera_info.yaml", std::ios::binary);
                std::ofstream dst(currentScenePath + "/camera_info.yaml", std::ios::binary);
                dst << src.rdbuf();

                return true;
            } else {
                return false;
            }
        }
    }

    return false;
    }
    

    void saveDepth( pho::api::PFrame frame,
                    const std::string& currentScenePath,
                    int posecount){
        
        cv::Mat depth;
        char filenamedepth[2048];

        // Depth Map capture from Photoneo
        ConvertMat2DToOpenCVMat(frame->DepthMap, depth);

        double dNaN = std::numeric_limits<double>::quiet_NaN();
        depth.setTo(dNaN, depth == 0);

        depth.convertTo(depth, CV_16UC1);

        sprintf(filenamedepth, "%s/depth/%06d.png", currentScenePath.c_str(), posecount);
        cv::imwrite(filenamedepth, depth);
    }

    void saveRGB(   pho::api::PFrame frame,
                    const std::string& currentScenePath,
                    int posecount){
        cv::Mat color;
        char filenamergb[2048];

        ConvertMat2DToOpenCVMat(frame->TextureRGB, color);
        cv::cvtColor( color, color, cv::COLOR_RGB2BGR );

        cv::Mat image8;

        color.convertTo(image8, CV_8U);

        sprintf(filenamergb, "%s/rgb/%06d.png", currentScenePath.c_str(), posecount);
        cv::imwrite(filenamergb, image8);

    }

    void saveNormalMap( pho::api::PFrame frame,
                        const std::string& currentScenePath,
                        int posecount){
        cv::Mat normals;
        char filenamenormals[2048];
        ConvertMat2DToOpenCVMat(frame->NormalMap, normals);
        
        cv::cvtColor( normals, normals, cv::COLOR_BGR2RGB );
        int width = normals.cols;
        int height = normals.rows;

        // Create a blank image to store the normals
        cv::Mat normal_map(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Get the 3D normal vector from the Mat
                cv::Vec3f normal = normals.at<cv::Vec3f>(y, x);

                // Map the unit normal to RGB color values (scale it to [0, 255])
                uchar r = static_cast<uchar>((normal[0] + 1.0) * 127.5);
                uchar g = static_cast<uchar>((normal[1] + 1.0) * 127.5);
                uchar b = static_cast<uchar>((normal[2] + 1.0) * 127.5);

                // Set the pixel in the image with the color value mapped from the unit normal vector
                normal_map.at<cv::Vec3b>(y, x) = cv::Vec3b(r, g, b);
            }
        }

        sprintf(filenamenormals, "%s/normals/%06d.png", currentScenePath.c_str(), posecount);
        cv::imwrite(filenamenormals, normal_map);

    }

    void DataCollectionInteractive(
            pho::api::PhoXiFactory& factory,
            const std::string& projectFolderPath,
            const std::string& currentScenePath) {

        int capture_id = 0;
        char filenamepcd[2048];

        // Lotad calibration info
        auto calibration = loadCalibration(projectFolderPath);
        printCalibration(calibration);

        // Connect to a scanner
        auto device = selectAndConnectDevice(factory);
        pho::api::PhoXiCalibrationSettings CalibrationSettings = device->CalibrationSettings;

        // Iinitialize external camera
        auto extCamera = ExternalCamera();

        auto shouldContinue = []() {
            return  1 == ask<int>("Do you want to continue?", {
                {1, "Yes, trigger new scan and save "},
                {2, "No, disconnect the scanner"}
            });
        };

        while (shouldContinue()) {
            // Acquire a frame and load a corresponding image from ext. camera
            auto frame = triggerScanAndGetFrame(device);
            auto extImage = extCamera.getColorImage();

            // Add the calculated color texture
            frame->TextureRGB = colorPointCloudTexture(frame, extImage, calibration);

            cv::Mat bw_texture;

            ConvertMat2DToOpenCVMat(frame->Texture, bw_texture);

            if(charuco_marker_pose_estimation(bw_texture,CalibrationSettings,currentScenePath,capture_id)){
                std::cout << "The Charuco marker board has been correctly detected!" << std::endl;
                
                cv::imshow("Marker board pose on current frame.",bw_texture);
                // Wait for a key press and then close the window
                cv::waitKey(0);
                cv::destroyWindow("Marker board pose on current frame.");

                std::cout << "Saving collected data..." << std::endl;

                saveDepth(frame,currentScenePath,capture_id);
                saveRGB(frame,currentScenePath,capture_id);
                saveNormalMap(frame,currentScenePath,capture_id);

                sprintf(filenamepcd, "%s/pointclouds/%06d.ply", currentScenePath.c_str(), capture_id);

                saveColorPointCloud(filenamepcd, frame);

                std::cout << "Done!" << std::endl;
                capture_id++;

            }
            else{
                std::cout << "The Charuco marker board is not visible!" << std::endl;
            }
                
        }

        disconnectOrLogOut(device);
    }

    void DataCollectionAutomatic(
            pho::api::PhoXiFactory& factory,
            const std::string& projectFolderPath,
            const std::string& currentScenePath) {

        char filenamepcd[2048];

        int n_views;
        int steps;
        float angle;

        // Initialize Python interpreter
        Py_Initialize();
        // PySys_SetPath(L"/usr/local/lib/python3.7/site-packages/");

        PyRun_SimpleString(
        "import sys\n"
        "sys.path.append('/usr/local/lib/python3.7/site-packages/')\n"
        );


        // Open the script file
        const char* scriptPath = "rotate_table.py";

        FILE* scriptFile; 

        // Set command-line arguments for Python
        wchar_t* argv[] = { L"rotate_table.py", L"", L"", nullptr };

        // Load calibration info
        auto calibration = loadCalibration(projectFolderPath);
        printCalibration(calibration);

        // Connect to a scanner
        auto device = selectAndConnectDevice(factory);
        pho::api::PhoXiCalibrationSettings CalibrationSettings = device->CalibrationSettings;

        // Iinitialize external camera
        auto extCamera = ExternalCamera();

        // Ask the user for input
        std::cout << "How many viewpoints do you want to capture? ";
        std::cin >> n_views;

        angle = (360/n_views);
        steps = 67*angle;

        std::string arg1Value = std::to_string(angle);
        // Convert the narrow string to wide string
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring arg1ValueW = converter.from_bytes(arg1Value);

        argv[1] = const_cast<wchar_t*>(arg1ValueW.c_str());
        PySys_SetArgv(3, argv);

        for(int i = 0; i < n_views; i++) {

            if (i>0){

            std::cout << "Rotating the table..." << std::endl;            

            scriptFile = _Py_fopen(scriptPath, "r");
            if (!scriptFile) {
                PyErr_Print();
                return;
            }

            // Run the script
            int result = PyRun_SimpleFile(scriptFile, scriptPath);
            if (result != 0) {
                PyErr_Print();
                return;
            }

            // Close the script file
            fclose(scriptFile);

            std::cout << "Position reached!" << std::endl;
            }
            std::cout << "Triggering scan number " << i+1 << std::endl;

            auto frame = triggerScanAndGetFrame(device);
            auto extImage = extCamera.getColorImage();

            // Add the calculated color texture
            frame->TextureRGB = colorPointCloudTexture(frame, extImage, calibration);

            cv::Mat bw_texture;

            ConvertMat2DToOpenCVMat(frame->Texture, bw_texture);

            if(charuco_marker_pose_estimation(bw_texture,CalibrationSettings,currentScenePath,i)){
                std::cout << "The Charuco marker board has been correctly detected!" << std::endl;
                
                cv::imshow("Marker board pose on current frame.",bw_texture);
                //Wait for a key press and then close the window
                cv::waitKey(0);
                cv::destroyWindow("Marker board pose on current frame.");

                std::cout << "Saving collected data..." << std::endl;

                saveDepth(frame,currentScenePath,i);
                saveRGB(frame,currentScenePath,i);
                saveNormalMap(frame,currentScenePath,i);

                sprintf(filenamepcd, "%s/pointclouds/%06d.ply", currentScenePath.c_str(), i);

                saveColorPointCloud(filenamepcd, frame);

                std::cout << "Done!" << std::endl;

            }
            else{
                std::cout << "The Charuco marker board is not visible!" << std::endl;
                disconnectOrLogOut(device);
                return;
            }

        }

        std::cout << "Data collection completed!" << std::endl;
        std::cout << "Going back to 0 position" << std::endl;

        angle = 0;
        arg1Value = std::to_string(angle);
        arg1ValueW = converter.from_bytes(arg1Value);
        argv[1] = const_cast<wchar_t*>(arg1ValueW.c_str());
        argv[2] = L"--return-to-zero";
        PySys_SetArgv(3, argv);

        scriptFile = _Py_fopen(scriptPath, "r");
            if (!scriptFile) {
                PyErr_Print();
                return;
            }

        // Run the script
        int result = PyRun_SimpleFile(scriptFile, scriptPath);
        if (result != 0) {
            PyErr_Print();
        }

        // Close the script file
        fclose(scriptFile);

        // Finalize Python interpreter
        Py_Finalize();

        disconnectOrLogOut(device);

    }

} // namespace externalCamera
