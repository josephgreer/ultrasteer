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
	VideoCapture cap(1);
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
