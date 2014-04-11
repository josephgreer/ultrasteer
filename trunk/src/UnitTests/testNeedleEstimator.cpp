#include "CppUnitLite/Test.h"
#include "SegmentCore.h"
#include "Segment.h"
#include "RPFileReader.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

typedef struct {
	char bFile[100];
	char dFile[100];
	char gFile[100];
} BasicsNeedleEstimatorTest;

using namespace Nf;

static void releaseRPData(RPData *rp)
{
	if(rp->b8)
		cvReleaseImage(&rp->b8);
	if(rp->color)
		cvReleaseImage(&rp->color);
	if(rp->sig)
		cvReleaseImage(&rp->sig);
	if(rp->var)
		cvReleaseImage(&rp->var);
}

class RPTransform : ImageCoordTransform
{
protected:
	Vec2d m_mpp;
	Vec2d m_start;
	cv::Mat m_cal;
	GPS_Data m_gps;

public:
	RPTransform(Vec2d mpp, Vec2d start, cv::Mat &calibration, GPS_Data &gps)
		: m_mpp(mpp)
		, m_start(start)
		, m_gps(gps)
    , m_cal(4,4,CV_64F)
	{
    calibration.copyTo(m_cal);
	}

	Vec3d Transform(const Vec2d &image) const
	{
		Vec2d scale(m_mpp.x/1000.0, m_mpp.y/1000.0);
		
		f64 coordData[4] = {1, (image.y-m_start.y)*scale.y, (image.x-m_start.x)*scale.x, 1};
		cv::Mat imCoord(4, 1, CV_64F,coordData);
		
		cv::Mat sensor = m_cal*imCoord;
		cv::Vec4d world = (cv::Vec4d &)(m_gps.pose*sensor);

		world = world+cv::Vec4d(m_gps.pos.x, m_gps.pos.y, m_gps.pos.z, 0);

		return Vec3d(world(0), world(1), world(2));
	}
};

TEST(Basics, NeedleEstimator)
{
	BasicsNeedleEstimatorTest tests[] = {
		//filenames are of form:
		//Bmode:	bDirectory/%0xd.extension 
		//Doppler:  dDirectory/%0xd.extension 
		//where x = padding

		//Bmode file										//Doppler File											//GPS File	
		{"D:/Users/Joey Greer/NeedleTests/Scan 5/scan.b8",	"D:/Users/Joey Greer/NeedleTests/Scan 5/scan.b32",		"D:/Users/Joey Greer/NeedleTests/Scan 5/scan.gps1"},
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++){
		RPFileReaderCollection reader;

		reader.AddReader(RPF_BPOST8, (RPReader *)new RPFileReader(tests[n].bFile));
		reader.AddReader(RPF_BPOST32, (RPReader *)new RPFileReader(tests[n].dFile));
		reader.AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(tests[n].gFile));

		RPData curr = reader.GetNextRPData();
		s32 window = cvNamedWindow("Segment test");

    NeedleSegmenter segmenter(curr.color->width, curr.color->height);
    IplImage *display = cvCreateImage(cvSize(curr.color->width, curr.color->height),curr.color->depth, curr.color->nChannels);

    f64 calibData[16] = {14.8449, 15.0061, .1638, 0, .9477, .0016, .0166, 0, -.0018, 1, .0052, 0, 0, 0, 0, 1};
    cv::Mat calibration(4,4,CV_64F,calibData);
    PolyCurve model;

		while(curr.color) {
      RPTransform transform(Vec2d(curr.mpp, curr.mpp), Vec2d(curr.roi.ul.x, curr.roi.ul.y),
        calibration, curr.gps);
      segmenter.UpdateModel(&model, display, curr.color, curr.b8, (ImageCoordTransform *)&transform);

      cvShowImage("Segment test", display);

			releaseRPData(&curr);
			curr = reader.GetNextRPData();
		}

    cvReleaseImage(&display);
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