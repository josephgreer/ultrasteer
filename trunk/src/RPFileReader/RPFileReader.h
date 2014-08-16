#define _CRT_SECURE_NO_DEPRECATE
#pragma once
#include <Windows.h>
#include "SegmentCore.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

namespace Nf
{
  typedef enum {
    RPF_NULL_TYPE = -1,
    RPF_BPOST8 = 4,
    RPF_COLOR_PLUS_B = 5,
    RPF_BPOST32 = 8,
    RPF_COLOR = 1024,
    RPF_COLOR_CVV = 2048,
    RPF_COLOR_CVV_VAR = 2049,
    RPF_GPS = 131072,
  } RP_TYPE;

  static const std::string RP_FILE_TYPES[] = {
    "NULL",
    "Post 8 File",
    "Post 32 File",
    "Color Power File",
    "Color Variance File",
    "GPS File"
  };

  static const int RP_TYPE_LIST[] = {
    RPF_BPOST8,
    RPF_BPOST32,
    RPF_COLOR,
    RPF_COLOR_CVV,
    RPF_COLOR_CVV_VAR,
    RPF_COLOR_PLUS_B,
  };

#pragma pack(push, 1)
  typedef struct {
    int type;
    int frames;
    int width;
    int height;
    int ss;
    int ul[2];
    int ur[2];
    int br[2];
    int bl[2];
    int probe;
    int txf;
    int sf;
    int dr;
    int ld;
    int extra;
  } RPFileHeader;
#pragma pack(pop)

  struct GPS_Data {
    cv::Mat pose;
    Vec3d pos;
    Vec3d posaer;
    double offset[5];
    unsigned short quality;
    u8 valid;
    u32 tick;

    GPS_Data(const GPS_Data &rhs)
    : pos(rhs.pos)
    , posaer(rhs.posaer)
    , quality(rhs.quality)
    , valid(rhs.valid)
    , tick(rhs.tick)
    , pose(4,4,CV_64F)
    {
      rhs.pose.copyTo(pose);
      memcpy(&this->offset[0], &rhs.offset[0], sizeof(f64)*5);
    }

    GPS_Data()
      : pose(4,4,CV_64F)
    {
      this->valid = 0;
    }

    GPS_Data operator=(const GPS_Data &rhs)
    {
      rhs.pose.copyTo(this->pose);
      this->pos = rhs.pos;
      this->posaer = rhs.posaer;
      memcpy(&this->offset[0], &rhs.offset[0], sizeof(double)*5);
      this->quality = rhs.quality;
      this->valid = rhs.valid;
      this->tick = rhs.tick;
      return *this;
    }
  };

  //NOTE:  WE CO-OPT alphaEnabled to store clock tick when image arrived from ulterius.
  //alphaEnabled is ignored by openCv
  struct RPData {
    IplImage *b8;
    IplImage *color;
    IplImage *sig;
    IplImage *var;
    GPS_Data gps;
    f32 mpp;
    Squarei roi;

    RPData() 
    {
      b8 = NULL;
      color = NULL;
      sig = NULL;
      var = NULL;
      mpp = 0;
      gps.valid = 0;
    }

    RPData(const RPData &rp)
    {
      this->b8 = rp.b8;
      this->color = rp.color;
      this->sig = rp.sig;
      this->var = rp.var;
      this->mpp = rp.mpp;
      this->gps = rp.gps;
    }

    RPData operator=(const RPData &rhs)
    {
      this->b8 = rhs.b8;
      this->color = rhs.color;
      this->sig = rhs.sig;
      this->var = rhs.var;
      this->mpp = rhs.mpp;
      this->gps = rhs.gps;
      return *this;
    }

    void Release()
    {
      if(this->b8)
        cvReleaseImage(&this->b8);
      if(this->color)
        cvReleaseImage(&this->color);
      if(this->sig)
        cvReleaseImage(&this->sig);
      if(this->var)
        cvReleaseImage(&this->var);
    }
  };

  class RPGPSReaderBasic {
    virtual GPS_Data GetNextGPSDatum() = 0;
  };

  class RPGPSReader : RPGPSReaderBasic {
  private:
    int m_idx;
    FILE *m_file;
    RPFileHeader m_header;
    int m_bytesPerDatum;

  public:
    RPGPSReader(const char *path);
    virtual ~RPGPSReader();

    //Responsibility of recipient to free image
    GPS_Data GetNextGPSDatum();
    GPS_Data GetPreviousGPSDatum();
    GPS_Data GetGPSDatum(int frame);
    int GetCurrentFrame();
  };

  class RPReader {
    virtual RPData GetNextRPData() = 0;
    virtual RP_TYPE GetType() = 0;
  }; 

  class RPFileReader : RPReader {
  private:
    int m_idx;
    FILE *m_file;
    int m_type;
    RPFileHeader m_header;
    int m_bytesPerIm;

  public:
    RPFileReader(const char *path);
    virtual ~RPFileReader();

    //Responsibility of recipient to free image
    RPData GetNextRPData();
    RPData GetPreviousRPData();
    RPData GetRPData(int frame);
    int GetCurrentFrame();
    RPFileHeader GetHeader();

    RP_TYPE GetType();
  };

  class RPReaderCollection {

  public:
    virtual RPData GetNextRPData() = 0;
  };

  class RPFileReaderCollection : RPReaderCollection {

  public:
    void AddReader(RP_TYPE type, RPReader * reader);
    void AddGPSReader(RPGPSReaderBasic *gps);

    RPData GetNextRPData();
    RPData GetPreviousRPData();
    RPData GetRPData(int frame);

    RPFileReaderCollection();

    s32 GetCurrentFrame();

  protected:
    RP_TYPE m_type;
    std::map < RP_TYPE, RPFileReader * > m_readers;
    RPGPSReader *m_gps;
  };

};