#pragma once
#include "SegmentCore.h"

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

    void Reinitialize();

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
    s32 InitializeVolume(Matrix33d &orientation, Vec3d frameOrigin, VOLUME_ORIGIN_LOCATION config, Vec3d extent, Vec3d spacing);
    void Release();

    Vec3d GetSpacing();
    Vec3d GetOrigin();
    Matrix33d GetOrientation();
  };

  class VolumeCreator 
  {
  public:
    VolumeCreator();
  };
};