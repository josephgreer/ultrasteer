#include "CppUnitLite/Test.h"
#include "SegmentCore.h"
#include "RPFileReader.h"
#include "RPUlterius.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

typedef struct {
  char ip[100];
  u32 typeMask;
  u32 nframes;
} BasicsRPUlteriusTest;

using namespace Nf;

TEST(Ulterius, CaptureRate)
{
	BasicsRPUlteriusTest tests[] = {
    {"192.168.1.129", RPF_BPOST8, 1000}
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++){
		RPUlteriusReaderCollection reader(tests[n].ip);

    reader.EnableMask(tests[n].typeMask);

    for(s32 frame=0; frame<tests[n].nframes; frame++) {
      BEGIN_TIMING(UlteriusCaptureRate, 1000);
        RPData curr = reader.GetNextRPData();
      END_TIMING(UlteriusCaptureRate, 1000);
    }
  }
}