#define PHO_IGNORE_CV_VERSION_RESTRICTION
#define PHOXI_OPENCV_SUPPORT
#define PHOXI_PCL_SUPPORT
#define OPENCV_TRAITS_ENABLE_DEPRECATED

#include "ExternalCamera.h"
#include <PhoXiOpenCVSupport.h>


#include "ueye.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"

SENSORINFO sInfo;

HWND hWndDisplay;

char* pcImageMemory;

int DisplayWidth, DisplayHeight;

cv::Mat colorIDS;

HIDS hCam = 0;


void initializeIDS(){


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Starts the driver and establishes the connection to the camera

    is_InitCamera(&hCam, hWndDisplay);

    //You can query information about the sensor type used in the camera

    is_GetSensorInfo(hCam, &sInfo);

    //Saving the information about the max. image proportions in variables

    DisplayWidth = sInfo.nMaxWidth;

    DisplayHeight = sInfo.nMaxHeight;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Need to find out the memory size of the pixel and the colour mode

    int nColorMode;

    int nBitsPerPixel = 24;

    if (sInfo.nColorMode == IS_COLORMODE_BAYER)

    {

        // For color camera models use RGB24 mode

        nColorMode = IS_CM_BGR8_PACKED;

        nBitsPerPixel = 24;

    }

    else if (sInfo.nColorMode == IS_COLORMODE_CBYCRY)

    {

        // For CBYCRY camera models use RGB32 mode

        nColorMode = IS_CM_BGRA8_PACKED;

        nBitsPerPixel = 32;

    }

    else

    {

        // For monochrome camera models use Y8 mode

        nColorMode = IS_CM_MONO8;

        nBitsPerPixel = 24;

    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int nMemoryId;

    // Assigns a memory for the image and sets it active

    is_AllocImageMem(hCam, DisplayWidth, DisplayHeight, nBitsPerPixel, &pcImageMemory, &nMemoryId);

    is_SetImageMem(hCam, pcImageMemory, nMemoryId);

    // Acquires a single image from the camera

    //is_FreezeVideo(hCam, IS_WAIT);

    // Parameter definition for saving the image file

    /*
    IMAGE_FILE_PARAMS ImageFileParams;
    ImageFileParams.pwchFileName = L"./TestImage.bmp";   /// <-- Insert name and location of the image
    ImageFileParams.pnImageID = NULL;
    ImageFileParams.ppcImageMem = NULL;
    ImageFileParams.nQuality = 0;
    ImageFileParams.nFileType = IS_IMG_BMP;
    */
    {

        double enable = 1;

        double disable = 0;

        is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_GAIN, &enable, 0);

        is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &enable, 0);

        is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &enable, 0);

        is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SHUTTER, &enable, 0);

        is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);

        is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE,&enable,0);


        double parameter = 40.;
	    int error = is_Exposure(hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&parameter, sizeof(parameter));
        double factor = 0.5;
	    INT Color = is_SetColorCorrection(hCam, IS_CCOR_ENABLE_NORMAL, &factor);

        //simon experiment
        //is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SHUTTER, &disable, 0);
        //is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &enable, 0);//this anyway is not supported by every sensor. i think we al

        uint pcRange[3];
        pcRange[0]=0;
        pcRange[1]=0;
        pcRange[2]=0;
        is_PixelClock(hCam,IS_PIXELCLOCK_CMD_GET_RANGE,pcRange,sizeof(pcRange));
        std::cout << "pixel clock" << pcRange[0] << " to " << pcRange[1] << " and " << pcRange[2] << std::endl;

        is_PixelClock(hCam,IS_PIXELCLOCK_CMD_SET,&pcRange[1],sizeof(uint));


        //is_SetFrameRate(hCam,60,0);
        //double currentFps=0;
        //is_SetFrameRate(hCam,IS_GET_FRAMERATE,&currentFps);
        //cout << "currentFps" << currentFps << endl;

        double reachedFps;
        is_GetFramesPerSecond(hCam,&reachedFps);
        std::cout << "reachedFps" << reachedFps << std::endl;
        //set pixel clock to max
        //is_PixelClock(hCam,IS_PIXELCLOCK_CMD_SET,&pcRange[1],sizeof(pcRange));

        //whats the difference between auto gain and auto sensor gain?
        //IS_SET_


    }


}










namespace externalCamera {





pho::api::Texture32f ExternalCamera::getCalibrationImage()
{
    // TODO implement me
    cv::Mat cvImage;  /* = cv::imread(...)  or similar */


        if (is_FreezeVideo(hCam, IS_WAIT) == IS_SUCCESS) {

            void *pMemVoid; //pointer to where the image is stored

            is_GetImageMem(hCam, &pMemVoid);

            IplImage *img;

            img = cvCreateImage(cvSize(DisplayWidth, DisplayHeight), 8, 3); //8,3

            img->imageData = (char *) pMemVoid;
            colorIDS = cv::cvarrToMat(img);

        } else {

            int nReturn;

            char *pErr = 0;

            is_GetError(hCam, &nReturn, &pErr);

            printf("IDS_uEye: failed capturing an image, error %d: %s\n", nReturn, pErr);

        }

    // Convert to Texture32f
    pho::api::Texture32f image;
    cv::Mat gray;
    //cv::copyMakeBorder(colorIDS,colorIDS, 0, 0, 40, 40, cv::BORDER_CONSTANT);
    cv::cvtColor(colorIDS, gray, cv::COLOR_BGR2GRAY);
    cvImage = gray;

    cv::imshow("color", cvImage);
    cv::imwrite("color.png", cvImage);

    cv::waitKey(0);
    
    pho::api::ConvertOpenCVMatToMat2D(cvImage, image);
    return image;
}

cv::Mat ExternalCamera::getColorImage()
{
    cv::Mat image;
    // TODO implement me
    if(is_FreezeVideo(hCam, IS_WAIT) == IS_SUCCESS){

        void *pMemVoid; //pointer to where the image is stored

        is_GetImageMem (hCam, &pMemVoid);

        IplImage *img;

        img=cvCreateImage(cvSize(DisplayWidth, DisplayHeight), 8, 3); //8,3

        img->imageData=(char*) pMemVoid;
        colorIDS = cv::cvarrToMat(img);

    }

    else{

        int nReturn;

        char *pErr = 0;

        is_GetError(hCam, &nReturn, &pErr);

        printf("IDS_uEye: failed capturing an image, error %d: %s\n", nReturn, pErr);

    }
    // Convert to Texture32f


    return colorIDS;
};

} // namespace externalCamera
