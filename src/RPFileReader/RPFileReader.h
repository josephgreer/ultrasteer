#define _CRT_SECURE_NO_DEPRECATE
#pragma once
#include <Windows.h>
#include "RTCore.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define NOMINAL_SOS 1540.0

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
    RPF_GPS2 = 262144,
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
    int sf;           //co-opted for speed of sound
    int dr;           //co-opted for mpp horizontal
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

  inline Vec3d rpImageCoordToWorldCoord3(const Vec2d &image, const Matrix44d &posePos, const Matrix44d &calibration, const Vec2d &start, const Vec2d &scale)
  {
    Vec4d world =  posePos*calibration*Vec4d(1, (image.y-start.y)*scale.y, (image.x-start.x)*scale.x, 1.0);
    return Vec3d(world.x, world.y, world.z);
  }

  inline Vec3d rpWorldCoord3ToImageCoord(const Vec3d &pt3_w, const Matrix44d &posePos, const Matrix44d &calibration, Vec2d &start, const Vec2d &scale)
  {
    Vec4d pt4_w( pt3_w.x, pt3_w.y, pt3_w.z, 1.0 );                    // World (EM-tracker) frame point
    Vec4d pt4_us = posePos.Inverse()*pt4_w;                           // Transducer frame point
    Matrix33d cal_part(-0.0018, 0.9477, 0, 1.00, 0.0016, 0, 0, 0, 1); // Hard-coded values taken from Sonix calibration matrix

    // Invert a square portion of the calibration matrix, scale and shift image point to VTK origin
    Vec3d pt3_im = cal_part.Inverse()* ( Vec3d(pt4_us.x,pt4_us.y,0) - Vec3d(14.8449,15.0061,0) );
    pt3_im.z = pt4_us.z;
    return Vec3d( (pt3_im.x/scale.x) + start.x, (pt3_im.y/scale.y) + start.y, pt3_im.z/scale.y);
  }

  inline Vec3d rpImageCoordToRobotCoord3(const Vec2d &pt2_image, const Matrix44d &Ttrans2em, const Matrix44d &Tref2em, const Matrix44d &Tref2robot, const Matrix44d &calibration, const Vec2d &start, const Vec2d &scale)
  {
    Vec4d p4_em = Ttrans2em*calibration*Vec4d(1, (pt2_image.y-start.y)*scale.y, (pt2_image.x-start.x)*scale.x, 1.0);
    Vec4d p4_robot = Tref2robot*Tref2em.Inverse()*p4_em;
    return Vec3d(p4_robot.x, p4_robot.y, p4_robot.z);
  }

  inline Vec3d rpRobotCoord3ToImageCoord(const Vec3d &p3_robot, const Matrix44d &Ttrans2em, const Matrix44d &Tref2em, const Matrix44d &Trobot2ref, const Matrix44d &calibration, const Vec2d &start, const Vec2d &scale)
  {
    Vec4d p4_robot( p3_robot.x, p3_robot.y, p3_robot.z, 1.0 );        // point in the robot frame
    Vec4d p4_em = Tref2em*Trobot2ref*p4_robot;                        // point in EM tracker frame
    Vec4d p4_us = Ttrans2em.Inverse()*p4_em;                          // Transducer frame point
    Matrix33d cal_part(calibration.m_data[0][2], calibration.m_data[0][1], 0, 1.00, calibration.m_data[1][1], 0, 0, 0, 1); // values taken from Sonix calibration 
    // Invert a square portion of the calibration matrix, scale and shift image point to VTK origin
    Vec3d p3_im = cal_part.Inverse()* ( Vec3d(p4_us.x,p4_us.y,0) - Vec3d(calibration.m_data[0][0],calibration.m_data[1][0],0) );
    p3_im.z = p4_us.z;
    return Vec3d( (p3_im.x/scale.x) + start.x, (p3_im.y/scale.y) + start.y, p3_im.z/scale.y);
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
    GPS_Data gps2;
    Vec2d mpp;
    Vec2d origin;
    Squarei roi;

    RPData() 
    {
      b8 = NULL;
      color = NULL;
      sig = NULL;
      var = NULL;
      mpp = Vec2d(0,0);
      gps.valid = 0;
      gps2.valid = 0;
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
      this->gps2 = rp.gps2;
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
      this->gps2 = rhs.gps2;
      this->origin = rhs.origin;
      return *this;
    }

    Cubed GetFrameBoundaries(Matrix44d cal) const
    {
      Matrix44d tPose = Matrix44d::FromCvMat(this->gps.pose);
      Matrix33d pose = tPose.GetOrientation();
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, this->gps.pos);
      Vec2d scale(this->mpp.x/1000.0, this->mpp.y/1000.0);
      Vec3d ul = rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, cal, this->origin, scale);
      Vec3d ur = rpImageCoordToWorldCoord3(Vec2d(this->b8->width,0), posePos, cal, this->origin, scale); 
      Vec3d bl = rpImageCoordToWorldCoord3(Vec2d(0,this->b8->height), posePos, cal, this->origin, scale); 

      Vec3d xaxis = (ur-ul);
      Vec3d yaxis = (bl-ul);
      Vec3d zaxis = Vec3d(0,0,0);

      return Cubed(ul, Matrix33d::FromCols(xaxis, yaxis, zaxis));
    }

    void GetGPS1Relative(arma::mat33 &R1_rel, arma::vec3 &p1_rel) const
    {
      using namespace arma;

      // get position and orientation of GPS1 frame 
      Vec3d p_GPS1 = this->gps.pos;
      Matrix44d pose_GPS1 = Matrix44d::FromCvMat(this->gps.pose);
      pose_GPS1.SetPosition(p_GPS1); // current implementation of rp.gps.pose does not include position in 4x4
      mat44 T_GPS1 = pose_GPS1.ToArmaMatrix4x4();

      // get position and orientation of GPS2 frame
      Vec3d p_GPS2 = this->gps2.pos;
      Matrix44d pose_GPS2 = Matrix44d::FromCvMat(this->gps2.pose);
      pose_GPS2.SetPosition(p_GPS2); // current implementation of rp.gps.pose does not include position in 4x4
      mat44 T_GPS2 = pose_GPS2.ToArmaMatrix4x4();

      // get relative position and orientation
      mat44 T_rel = inv( T_GPS2 )*T_GPS1;    

      // pass position and orientation to the stylus calibration object
      R1_rel = T_rel.submat(0,0,2,2);
      p1_rel = T_rel.submat(0,3,2,3);
    }

    void GetGPS1RelativeT(arma::mat44 &T1_rel)
    {
      using namespace arma;
      mat33 R1_rel;
      vec3 t1_rel;
      this->GetGPS1Relative(R1_rel,t1_rel);
      T1_rel.eye();
      T1_rel.submat(0,0,2,2) = R1_rel;
      T1_rel.submat(0,3,2,3) = t1_rel;
    }

    RPData Clone() const
    {
      RPData rv;
      if(this->b8)
        rv.b8 = cvCloneImage(this->b8);
      if(this->color)
        rv.color = cvCloneImage(this->color);
      if(this->sig)
        rv.sig = cvCloneImage(this->sig);
      if(this->var)
        rv.var = cvCloneImage(this->var);

      rv.gps = this->gps;
      rv.gps2 = this->gps2;
      rv.mpp = this->mpp;
      rv.origin = this->origin;

      return rv;
    }

    bool FullSet(u32 mask)
    {
      if(mask&RPF_BPOST8 && !this->b8)
        return false;
      if(((mask&RPF_COLOR)||(mask&RPF_BPOST32)) && !this->color)
          return false;
      if(mask&RPF_COLOR_CVV && !this->sig)
        return false;
      if(mask&RPF_COLOR_CVV_VAR && !this->var)
        return false;
      if(mask&RPF_GPS && !this->gps.valid)
        return false;
      if(mask&RPF_GPS2 && !this->gps2.valid)
        return false;
      return true;
    }

    const IplImage * GetType(RP_TYPE type) const
    {
      if(type == RPF_BPOST8)
        return this->b8;
      else if(type == RPF_BPOST32 || type == RPF_COLOR)
        return this->color;
      return NULL;
    }

    IplImage * GetType(RP_TYPE type)
    {
      if(type == RPF_BPOST8)
        return this->b8;
      else if(type == RPF_BPOST32 || type == RPF_COLOR)
        return this->color;
      return NULL;
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
    RPFileHeader GetHeader() { return m_header; }
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
    void AddGPSReader2(RPGPSReaderBasic *gps2);

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
    RPGPSReader *m_gps2;
  };

};