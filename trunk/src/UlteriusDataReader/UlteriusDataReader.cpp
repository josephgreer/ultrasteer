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
  if(enabledTypes&RPF_BPOST8 && !data->b8)
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
    char ext[] = "col";
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

  cvNamedWindow("Display Window");
  RPData data = readers.GetNextRPData();
  while(checkFullSet(&data, mask)) {
    RPData data = readers.GetNextRPData();
    cvShowImage("Display Window", data.color);
    releaseRPData(&data);
    s32 x = 0;
  }

  return 0;
}