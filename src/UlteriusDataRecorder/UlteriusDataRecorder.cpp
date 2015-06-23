#include <conio.h>
#include <stdio.h>
#include "math.h"
#include "RPFileReader.h"
#include "RPUlterius.h"
#include "RPFileWriter.h"
#include "RPProcess.h"

using namespace Nf;

static void printUsage()
{
  printf("UlteriusDataRecorder.exe -ip xxx.xxx.xxx.xxx -d directory -n name -t typeMask -mpp mpp -ox origin_x -oy origin_y -f framerate\n");
}

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

static RPFileHeader RPFileHeader_uDataDesc(const uDataDesc &desc)
{
  RPFileHeader res;  memset(&res, 0, sizeof(RPFileHeader));
  res.ul[0] = desc.roi.ulx;
  res.ul[1] = desc.roi.uly;
  res.bl[0] = desc.roi.blx;
  res.bl[1] = desc.roi.bly;
  res.br[0] = desc.roi.brx;
  res.br[1] = desc.roi.bry;
  res.ur[0] = desc.roi.urx;
  res.ur[1] = desc.roi.ury;
  res.width = desc.w;
  res.height = desc.h;
  res.frames = 0;
  return res;
}

int main( int argc, const char* argv[] )
{
  if(argc < 17) {
    printUsage();
    return 0;
  }

  bool run = true;
  bool freeze = true;

  char ip[100] = {0};
  strcpy(ip, argv[2]);

  char dir[100];  char filename[100];  char path[100];
  strcpy(dir, argv[4]);
  strcpy(filename, argv[6]);
  sprintf(path, "%s/%s", dir, filename);

  s32 mask = atoi(argv[8]);

  f64 mpp = atof(argv[10]);

  f64 ox = atof(argv[12]);
  f64 oy = atof(argv[14]);

  f32 framerate = atof(argv[16]);

  RPUlteriusReaderCollection rpReaders(ip,mpp, Vec2d(ox,oy));
  //RPUlteriusProcessManager rpReaders(ip,mpp,Vec2d(ox,oy),framerate);
  //if(mask&RPF_BPOST32) {
  //  printf("Enabling bpost32\n");
  //  rpReaders.EnableType(RPF_BPOST32, 1);
  //}
  //if(mask&RPF_BPOST8) {
  //  printf("Enabling bpost8\n");
  //  rpReaders.EnableType(RPF_BPOST8, 1);
  //}
  //if(mask&RPF_GPS) {
  //  printf("Enabling gps\n");
  //  rpReaders.EnableType(RPF_GPS, 1);
  //}
  rpReaders.EnableMask(mask);

  RPFileWriterCollection *writers = NULL;

  s32 frame = 0;
  RPFileHeader header;  memset(&header, 0, sizeof(RPFileHeader));

  printf("Connected to %s\n", ip);
  printf("Press q to quit and f to freeze\n");

  while(run) {
    if(kbhit()) {
      char input = getch();
      switch(input) {
        case 'q':
          run = false;
          break;
        case 'f':
          freeze = !freeze;
          break;
        default:
          break;
      }
    }

    if(freeze) {
      //printf("\rFrozen                                                                ");
      continue;
    }

    RPData rp = rpReaders.GetNextRPData();
    if(((mask&RPF_BPOST32) && !rp.color) || ((mask&RPF_BPOST8)&&!rp.b8) || ((mask&RPF_GPS) && !rp.gps.valid)) {
      Sleep(20);
      releaseRPData(&rp);
      continue;
    }
    if(frame == 0) {
      uDataDesc desc = rpReaders.GetImageDesc(RPF_COLOR);
      header = RPFileHeader_uDataDesc(desc);
      writers = new RPFileWriterCollection(path, &header);

      if(rp.b8) {
        if(writers->AddWriter(RPF_BPOST8) < 0) {
          printf("Error adding b8 writer\n");
          return -1;
        }
      }
      if(rp.color){
        if(writers->AddWriter(RPF_BPOST32) < 0) {
          printf("Error adding b32 writer\n");
          return -1;
        }
      }
      if(rp.gps.valid) {
        if(writers->AddGPSWriter() < 0) {
          printf("Error adding gps writer\n");
          return -1;
        }
      }
    }

    writers->WriteNextRPData(&rp);

    releaseRPData(&rp);

    frame++;
    printf("Frame %d                                                   \n", frame);     

  }

  printf("\nCleaning up...\n");
  header.frames = frame;
  header.dr = (s32)(mpp+0.5);
  header.ld = (s32)(ox+0.5);
  header.extra = (s32)(oy+0.5);
  if(writers) {
    writers->Cleanup(&header);
    delete writers;
  }

  return 0;
}