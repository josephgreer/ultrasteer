#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "RPFileReader.h"
#include "RPFileWriter.h"
#include "RPUlterius.h"
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

using namespace Nf;

typedef struct {
  char ip[100];
  u32 typeMask;
  u32 nframes;
  Vec2d mpp;
  Vec2d origin;
} BasicsRPUlteriusTest;

TEST(IO, UlteriusCaptureRate)
{
	BasicsRPUlteriusTest tests[] = {
    {"192.168.1.129", RPF_BPOST8|RPF_GPS, 200, Vec2d(87.0,87.0), Vec2d(330.0, 78.0)}
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++){
		RPUlteriusReaderCollection reader(tests[n].ip, tests[n].mpp, tests[n].origin);

    reader.EnableMask(tests[n].typeMask);

    u32 t0 = GetTickCount();
    s32 i=0;
    printf("\n");
    while(i < tests[n].nframes) {
      RPData curr = reader.GetNextRPData();
      if(curr.FullSet(tests[n].typeMask)) {
        curr.Release();
        i++;
      }
      printf("\rFrame %d", i);
    }
    u32 t1 = GetTickCount();

    f32 msPerFrame = (f32)(t1-t0)/(f32)tests[n].nframes;
    f32 sPerFrame = msPerFrame/1000.0f;
    f32 fps = 1/sPerFrame;

    NTrace("Average fps %f over %d frames\n", fps, tests[n].nframes);
  }
}

TEST(Scratch, ReverseIterator)
{
  std::map < int, int > yep;

  for(s32 i=5; i<20; i++) {
    yep[i] = i;
  }

  for(std::map<int,int>::reverse_iterator i = yep.rbegin(); i!=yep.rend(); i++)
    NTrace("curr %d\n", i->first);
 
  s32 del = 8;

  std::map<int,int>::reverse_iterator i = yep.rbegin();
  for(; i!=yep.rend(); i++) {
    if(i->first == del)
      break;
  }

  
  yep.erase(std::next(i).base());

  NTrace("After deletion of %d\n", del);
  for(std::map<int,int>::reverse_iterator i = yep.rbegin(); i!=yep.rend(); i++)
    NTrace("curr %d\n", i->first);

  for(s32 i=5; i<20; i++) {
    if(i == del)
      assert(yep.find(i) == yep.end());
    else
      assert(yep.find(i) != yep.end());
  }
}

TEST(GPS, Smooth)
{
  using ::s32;

  std::string basePath = PATH_CAT("Trial2/Insertion/scan");

  char temp[200] = {0};
  sprintf(temp, "%s.gps2", basePath.c_str());

  RPGPSReader *gpsReader = new RPGPSReader(temp);
  RPFileHeader header = gpsReader->GetHeader();

  sprintf(temp, "%ssmoothed.gps2", basePath.c_str());
  RPGPSWriter *writer = new RPGPSWriter(temp, &header);

  sprintf(temp, "%ssmoothed.dat", basePath.c_str());
  arma::mat smoothedPoints; smoothedPoints.load(temp);

  for(s32 i=0; i<smoothedPoints.n_rows; i++) {
    arma::vec3 point = (arma::vec3)(smoothedPoints.row(i).t());
    GPS_Data datum = gpsReader->GetNextGPSDatum();
    datum.pos = Vec3d::FromArmaVec(point);
    writer->WriteNextGPSDatum(&datum);
  }

  writer->Cleanup(&header);

  delete writer;
  delete gpsReader;
}