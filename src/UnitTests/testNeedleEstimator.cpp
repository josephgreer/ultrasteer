#include "CppUnitLite/Test.h"
#include "NeedleEstimatorCore.h"
#include "NeedleFitter.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

typedef struct {
	char bDirectory[100];
	char dDirectory[100];
	u8 padding;
	char extension[10];
	s32 startFrame;
	s32 endFrame;
} BasicsNeedleEstimatorTest;

TEST(Basics, NeedleEstimator)
{
	BasicsNeedleEstimatorTest tests[] = {
		//filenames are of form:
		//Bmode:	bDirectory/%0xd.extension 
		//Doppler:  dDirectory/%0xd.extension 
		//where x = padding

		//bmode directory							//Doppler Directory								//padding	//extension		//startFrame   //endFrame			//expected result
		{"D:/Users/Joey Greer/NeedleTests/Bmode/",	"D:/Users/Joey Greer/NeedleTests/Doppler/",		6,			"png",			0,				10000000							},
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++) {

	}
}

TEST(Matrix, NeedleEstimator)
{
	f64 A[16] = {
					1, 2, 3, 4, 
					5, 6, 7, 8,
					9, 10, 11, 12,
					13, 14, 15, 16
	};

	f64 B[16] = {
					16, 15, 14, 13,
					12, 11, 10, 9,
					8, 7, 6, 5,
					4, 3, 2, 1
	};

	cv::Mat mA(4, 4, CV_64F, A);
	mA = mA.t();
	cv::Mat mB(4, 4, CV_64F, B);
	mB = mB.t();

	cv::Mat mC = mA*mB;
	
	std::stringstream temp;
	temp << "A= " << mA << std::endl;
	temp << "B= " << mB << std::endl;
	temp << "A*B= " << mC << std::endl;

	f64 dRes[64] = {386, 274, 162, 50,
					444, 316, 188, 60,
					502, 358, 214, 70,
					560, 400, 240, 80,
	};

	temp << "Desired = " << cv::Mat(4,4,CV_64F,dRes) << std::endl;

	//Match cinder matrices
	Nf::NTrace(temp.str().c_str());
}