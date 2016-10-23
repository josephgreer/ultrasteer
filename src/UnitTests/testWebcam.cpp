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

TEST(Webcam, Vine)
{
	cv::VideoCapture cap(1);
	if(!cap.isOpened()) {
		int x = 0;
	}
}