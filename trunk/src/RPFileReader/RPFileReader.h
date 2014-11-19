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
    int dr;           //co-opted for mpp
    int ld;           //co-opted for origin_x
    int extra;        //co-opted for origin_y
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

  inline Vec3d rpImageCoordToWorldCoord3(const Vec2d &image, const Matrix44d &posePos, const Matrix44d &calibration, Vec2d &start, const Vec2d &scale)
  {
    Vec4d world =  posePos*calibration*Vec4d(1, (image.y-start.y)*scale.y, (image.x-start.x)*scale.x, 1.0);
    return Vec3d(world.x, world.y, world.z);
  }

  inline Vec4d rpImageCoordToWorldCoord4(const Vec2d &image, const Matrix44d &posePos, const Matrix44d &calibration, Vec2d &start, const Vec2d &scale)
  {
    return posePos*calibration*Vec4d(1, (image.y-start.y)*scale.y, (image.x-start.x)*scale.x, 1.0);
  }

  //NOTE:  WE CO-OPT alphaEnabled to store clock tick when image arrived from ulterius.
  //alphaEnabled is ignored by openCv
  struct RPData {
    IplImage *b8;
    IplImage *color;
    IplImage *sig;
    IplImage *var;
    GPS_Data gps;
    f32 mpp;
    Vec2d origin;
    Squarei roi;

    RPData() 
    {
      b8 = NULL;
      color = NULL;
      sig = NULL;
      var = NULL;
      mpp = 0;
      gps.valid = 0;
      origin = Vec2d(0,0);
    }

    RPData(const RPData &rp)
    {
      this->b8 = rp.b8;
      this->color = rp.color;
      this->sig = rp.sig;
      this->var = rp.var;
      this->mpp = rp.mpp;
      this->gps = rp.gps;
      this->origin = rp.origin;
    }

    RPData operator=(const RPData &rhs)
    {
      this->b8 = rhs.b8;
      this->color = rhs.color;
      this->sig = rhs.sig;
      this->var = rhs.var;
      this->mpp = rhs.mpp;
      this->gps = rhs.gps;
      this->origin = rhs.origin;
      return *this;
    }

    Cubed GetFrameBoundaries(Matrix44d cal)
    {
      Matrix44d tPose = Matrix44d::FromCvMat(this->gps.pose);
      Matrix33d pose = tPose.GetOrientation();
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, this->gps.pos);
      Vec2d scale(this->mpp/1000.0, this->mpp/1000.0);
      Vec3d ul = rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, cal, this->origin, scale);
      Vec3d ur = rpImageCoordToWorldCoord3(Vec2d(this->b8->width,0), posePos, cal, this->origin, scale); 
      Vec3d bl = rpImageCoordToWorldCoord3(Vec2d(0,this->b8->height), posePos, cal, this->origin, scale); 

      Vec3d xaxis = (ur-ul);
      Vec3d yaxis = (bl-ul);
      Vec3d zaxis = Vec3d(0,0,0);

      return Cubed(ul, Matrix33d::FromCols(xaxis, yaxis, zaxis));
    }

    RPData Clone()
    {
      RPData rv;
      if(this->b8)
        rv.b8 = cvCloneImage(this->b8);
      if(this->color)
        rv.b8 = cvCloneImage(this->color);
      if(this->sig)
        rv.sig = cvCloneImage(this->sig);
      if(this->var)
        rv.var = cvCloneImage(this->var);

      rv.gps = this->gps;
      rv.mpp = this->mpp;

      return rv;
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
    ~RPFileReaderCollection();

    s32 GetCurrentFrame();

  protected:
    RP_TYPE m_type;
    std::map < RP_TYPE, RPFileReader * > m_readers;
    RPGPSReader *m_gps;
  };

};