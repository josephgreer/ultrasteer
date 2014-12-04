#include <conio.h>
#include <stdio.h>
#include "RPFileReader.h"
#include "RPUlterius.h"
#include "RPProcess.h"

using namespace Nf;

static void printUsage()
{
  printf("UlteriusDataReader.exe -f filename -t typemask\n");
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


static bool checkFullSet(const RPData *data, u32 enabledTypes)
{
  //GPS is kept in another stream and assembled at the end
  enabledTypes &= ~RPF_GPS;
  if((enabledTypes&RPF_BPOST8) && !data->b8)
    return false;
  if(((enabledTypes&RPF_COLOR)||(enabledTypes&RPF_BPOST32)) && !data->color)
    return false;
  if(enabledTypes&RPF_COLOR_CVV && !data->sig)
    return false;
  if(enabledTypes&RPF_COLOR_CVV_VAR && !data->var)
    return false;
  if(enabledTypes&RPF_GPS && !data->gps.valid)
    return false;
  return true;
}

int main( int argc, const char* argv[] )
{
  if(argc < 5) {
    printUsage();
    return 0;
  }

  RPFileReaderCollection readers;
  char baseName[100] = {0};
  strcpy(baseName, argv[2]);

  char fname[100] = {0};

  s32 mask = atoi(argv[4]);

  if(mask&RPF_BPOST8) {
    char ext[] = "b8";
    sprintf(fname, "%s.%s", baseName, ext);
    readers.AddReader(RPF_BPOST8, (RPReader *)new RPFileReader(fname));
  }
  if(mask&RPF_BPOST32) {
    char ext[] = "b32";
    sprintf(fname, "%s.%s", baseName, ext);
    readers.AddReader(RPF_BPOST32, (RPReader *)new RPFileReader(fname));
  }
  if(mask&RPF_COLOR) {
    char ext[] = "col";
    sprintf(fname, "%s.%s", baseName, ext);
    readers.AddReader(RPF_COLOR, (RPReader *)new RPFileReader(fname));
  }
  if(mask&RPF_GPS) {
    char ext[] = "gps1";
    sprintf(fname, "%s.%s", baseName, ext);
    readers.AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(fname));
  }

  CvFont font;
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1);
  cvNamedWindow("Display Window");
  RPData data = readers.GetNextRPData();
  s32 i=1;
  while(checkFullSet(&data, mask)) {
    releaseRPData(&data);
    data = readers.GetNextRPData();
    IplImage *im = NULL;
    if(mask&RPF_BPOST32 || mask&RPF_COLOR)
      im = data.color;
    else
      im = data.b8;
    char txt[100] = {0};
    sprintf(txt, "Frame %d", i);
    cvPutText(im, txt, cvPoint(100,100), &font, cvScalar(255, 255, 255, 255));
    cvShowImage("Display Window", im);
    cvWaitKey(0);
    i++;
  }

  return 0;
}