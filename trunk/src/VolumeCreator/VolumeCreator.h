#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "SegmentCore.h"
#include "RPFileReader.h"

namespace Nf {

  //used for specifying origin location when setting a volume from a base frame
  enum VOLUME_ORIGIN_LOCATION
  {
    VOL_EDGE = 0,
    VOL_QUARTER,
    VOL_MIDDLE,
  };

  class Volume
  {
  protected:
    u16 *m_data;
    s32 m_sliceStride;  //in pixels (not bytes)
    s32 m_rowStride;    //in pixels (not bytes)

    u16 * GetSlice(s32 z);
    u16 * GetRow(s32 z, s32 r);

    Vec3i m_dims;   //number of voxels in x,y,z axis resp.
    Vec3d m_spacing;  //spacing between voxels in x,y,z axis resp. (physical units)

    Vec3d m_origin; //origin of volume in units (the location of voxel index [0,0,0])
    Matrix33d m_orientation;  //Columns of matrix correspond to x-axis, y-axis, and z-axis of the volume, resp. 

    Matrix44d m_worldToVolume;    //Matrix part of transform from world coordiantes to volume indices
    Matrix44d m_volumeToWorld;    //Matrix part of transfrom from volume indices to world coordinates (m_volumeToWorld = m_worldToVolume^{-1})   

    f64 m_scale;  //scale factor to apply to images before adding to the volume

    void Reinitialize();

    IplImage *m_im;

  public:
    Volume();
    //Volume represents a 3D cube (with non-equal dimensions)
    //Initialize volume from a frame and extent around the frame
    //orientation:  Columns of orientation matrix specify the directions of x, y, and z axis of the cube
    //frameOrigin: specifies the location of the middle of the frame (in physical units).  Memory for a  3D
    //  volume will be allocated around the frameOrigin.  
    //config:  Specifies the way the volume will be allocated around the frame. /
    //  VOL_EDGE should be used if the location of the frame marks the edge of the volume
    //  VOL_QUARTER should be used if the location of the frame is near the edge but you want some space
    //  VOL_MIDDLE should be used if the location of the frame will be in the middle of the volume
    //extent:  Specifies the extent of the volume in physical units.  It will be converted to numbers of voxels by taking into
    //  account spacing
    //spacing:  Specifiy the distance between voxels in each of the axes in phyiscal units.
    s32 InitializeVolume(Matrix33d &orientation, Vec3d frameOrigin, VOLUME_ORIGIN_LOCATION config, Vec3d extent, Vec3d spacing, f64 imscale);
    void Release();

    Vec3d GetSpacing();
    Vec3d GetOrigin();
    Matrix33d GetOrientation();
    Vec3i GetDims();
    u16 * GetCoordData(Vec3i coord);

    Vec3d WorldCoordinatesToVolumeCoordinates(Vec3d worldCoords);
    Vec3d VolumeCoordinatesToWorldCoordinates(Vec3d volCoords);

    void AddFrame(const IplImage *image, const Matrix33d &pose, const Vec3d &pos, const Matrix44d &calibration, const Vec2d &mpp);
  };


  class VolumeCreator 
  {
  public:
    virtual ~VolumeCreator();
    virtual Volume *GetNew() = 0;
  };

  typedef enum
  {
    RPVM_READ_ALL = 0,              //Read all b-mode frames and put them into the volume at once
    RPVM_READ_RANGE = 1,            //Read range of b-mode frames and update pipeline sequentially
    RPVM_READ_SEQUENTIALLY = 2,     //Read all b-mode frames and update pipeline sequentially
  } RP_VOLUME_READ_MODE;

  //Create vtkImageData objects from RP b8 files
  class RPVolumeCreator : VolumeCreator
  {
  protected:
    s32 m_index;                            //Current frame index.  Valid when m_rm != RPVM_READ_ALL              
    bool m_newData;                         //New data since last GetNew() call?
    Volume m_volume;                        //Self explanatory
    RPFileReaderCollection m_rpReaders;     //What will actually do the reading for us
    Matrix44d m_cal;                        //Calibration Matrix for transducer (maps image coords to world coords)
    Vec2d m_mpp;                            //Vector containing microns per pixel of b-mode image in x&y dimensions

  public:
    RPVolumeCreator();
    virtual ~RPVolumeCreator();
    Volume *GetNew();
    virtual void Start() = 0;
    virtual Vec3i GetVolumeDims();
    virtual u16 * GetVolumeOriginData();
    void Release();
  };

  class RPFullVolumeCreator : public RPVolumeCreator
  {
  public:
    RPFullVolumeCreator();
    virtual ~RPFullVolumeCreator();
    s32 Initialize(const char *path, Matrix44d &calibration, Vec2d &mpp, VOLUME_ORIGIN_LOCATION config, Vec3d &extent, Vec3d &spacing, f64 imscale);
    void Start();
  };

#if 0
  class RPSequentialVolumeCreator : RPVolumeCreator
  {
  };

  class RPRangeVolumeCreator : RPVolumeCreator
  {
  protected:
    Vec2i m_range;                          //Frame range.  Valid when m_rm == RPVM_READ_RANGE
  };
#endif
};