#include "CppUnitLite/Test.h"
#include "NeedleEstimatorCore.h"
#include "NeedleFitter.h"

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