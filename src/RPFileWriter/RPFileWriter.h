#define _CRT_SECURE_NO_DEPRECATE
#pragma once
#include <Windows.h>
#include "RTCore.h"
#include "RPFileReader.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "RTCore.h"

namespace Nf
{
  class RPGPSWriter {
  private:
    FILE *m_file;
    RPFileHeader m_header;
    int m_bytesPerDatum;

  public:
    RPGPSWriter(const char *path, const RPFileHeader *header);
    virtual ~RPGPSWriter();
    s32 Cleanup(const RPFileHeader *header);
  
    s32 WriteNextGPSDatum(const GPS_Data *gps);
  };

  class NSCommandWriter {
  private:
    FILE *m_file;
    int m_bytesPerDatum;

  public:
    NSCommandWriter(const char *path);
    virtual ~NSCommandWriter();
    s32 Cleanup();

    s32 WriteNextNSCommand(const NSCommand *u);
  };

  class RPFileWriter {
  private:
    FILE *m_file;
    int m_type;
    RPFileHeader m_header;
    int m_bytesPerIm;

  public:
    RPFileWriter(const char *path, const RPFileHeader *header, int type);
    virtual ~RPFileWriter();
    s32 Cleanup(const RPFileHeader *header);
  
    //Responsibility of recipient to free image
    s32 WriteNextImage(const IplImage *im);

    RP_TYPE GetType();
  };


  class RPFileWriterCollection {

  public:
    s32 AddWriter(RP_TYPE type);
    s32 AddGPSWriter();
    s32 AddGPSWriter2();
    s32 AddNSCommandWriter();

    s32 WriteNextRPData(const RPData *data);
    s32 Cleanup(const RPFileHeader *header);

    RPFileWriterCollection(const char *path, const RPFileHeader *header);

  protected:
    std::map < RP_TYPE, RPFileWriter * > m_writers;
    RPGPSWriter *m_gps;
    RPGPSWriter *m_gps2;
    NSCommandWriter *m_u;
    RPFileHeader m_header;
    char m_path[100];
    bool m_writeOver;
  };

};