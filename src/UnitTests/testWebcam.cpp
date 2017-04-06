#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include <memory>
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>
#include <opencv2/opencv.hpp>

using namespace Nf;
using namespace cv;

TEST(Webcam, Vine)
{
	VideoCapture cap("udp://@:5004");
	if(!cap.isOpened()) {
		int x = 0;
	}

	namedWindow("Vine View",1);
	Mat frame;
	for(;;)
	{
		BEGIN_TIMING(MAIN_LOOP, 100);
		cap >> frame; // get a new frame from camera
		imshow("Vine View", frame);
		if(waitKey(10) >= 0) break;
		END_TIMING(MAIN_LOOP,100);
	}
}

TEST(Webcam, MJPEG)
{
	CvCapture* capture = 0;
	capture = cvCaptureFromFile("udp://@:5004/?dummy=param.mjpg");

	s32 x = 0;
}
