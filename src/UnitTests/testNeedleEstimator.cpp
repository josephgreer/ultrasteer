#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "Segment.h"
#include "RPFileReader.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

typedef struct {
	char bFile[100];
	char dFile[100];
	char gFile[100];
  f32 polyX[4];
  f32 polyY[4];
  f32 polyZ[4];
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
    m_cal = m_cal.t();
	}

	Vec3d Transform(const Vec2d &image) const
	{
		Vec2d scale(m_mpp.x/1000.0, m_mpp.y/1000.0);
		
		f64 coordData[4] = {1, (image.y-m_start.y)*scale.y, (image.x-m_start.x)*scale.x, 1};
		cv::Mat imCoord(4, 1, CV_64F,coordData);
		
		cv::Mat sensor = m_cal*imCoord;
    cv::Mat wrld = (m_gps.pose*sensor);
		cv::Vec4d world(wrld.at<f64>(0),wrld.at<f64>(1),wrld.at<f64>(2),wrld.at<f64>(3));

		world = world+cv::Vec4d(m_gps.pos.x, m_gps.pos.y, m_gps.pos.z, 0);

		return Vec3d(world(0), world(1), world(2));
	}
};

#define FLOATS_EQUAL(x,y,thresh) (ABS(x-y) < thresh)

TEST(Basics, NeedleEstimator)
{
	BasicsNeedleEstimatorTest tests[] = {
		//filenames are of form:
		//Bmode:	bDirectory/%0xd.extension 
		//Doppler:  dDirectory/%0xd.extension 
		//where x = padding

		//Bmode file										//Doppler File											//GPS File	
    {"D:/Users/Joey Greer/NeedleTests/Scan 5/scan.b8",	"D:/Users/Joey Greer/NeedleTests/Scan 5/scan.b32",		"D:/Users/Joey Greer/NeedleTests/Scan 5/scan.gps1", {0,1,0,0},{115.02738f, -.084888972f,.0004524991},{-112.32117,-.022931162,.00011182049}},
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++){
		RPFileReaderCollection reader;

		reader.AddReader(RPF_BPOST8, (RPReader *)new RPFileReader(tests[n].bFile));
		reader.AddReader(RPF_BPOST32, (RPReader *)new RPFileReader(tests[n].dFile));
		reader.AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(tests[n].gFile));

		RPData curr = reader.GetNextRPData();

    NeedleSegmenter segmenter(curr.color->width, curr.color->height);

    f64 calibData[16] = {14.8449, 15.0061, .1638, 0, .9477, .0016, .0166, 0, -.0018, 1, .0052, 0, 0, 0, 0, 1};
    cv::Mat calibration(4,4,CV_64F,calibData);
    PolyCurve model;

		while(curr.color) {
      RPTransform transform(Vec2d(83, 83), Vec2d(320.0, 0),
        calibration, curr.gps);
      IplImage *display = segmenter.UpdateModel(&model, curr.color, curr.b8, (ImageCoordTransform *)&transform);

      if(display) {
        //cvShowImage("Yep", display); 
        cv::imshow("Yep", cv::Mat(display));
        cvWaitKey(5);
      }
			releaseRPData(&curr);
			curr = reader.GetNextRPData();
		}
    for(s32 i=0; i<4; i++) {
      if(!FLOATS_EQUAL(tests[n].polyX[i],model.coefX[i],1e-4f)) {
        NTrace("Fail X i: %d test:  %f  model:  %f\n", i, (f32)tests[n].polyX[i], (f32)model.coefX[i]);
        ASSERT(0);
      }
      if(!FLOATS_EQUAL(tests[n].polyY[i],model.coefY[i],1e-4f)) {
        NTrace("Fail Y i: %d test:  %f  model:  %f\n", i, (f32)tests[n].polyY[i], (f32)model.coefY[i]);
        ASSERT(0);
      }
      if(!FLOATS_EQUAL(tests[n].polyZ[i],model.coefZ[i],1e-4f)) {
        NTrace("Fail Z i: %d test:  %f  model:  %f\n", i, (f32)tests[n].polyZ[i], (f32)model.coefZ[i]);
        ASSERT(0);
      }
    }
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