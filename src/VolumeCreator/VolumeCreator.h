#pragma once
#include "SegmentCore.h"

namespace Nf {
  class Volume
  {
  protected:
    u16 *m_data;
    s32 m_sliceStride;  //in pixels (not bytes)
    s32 m_rowStride;    //in pixels (not bytes)

    u16 * GetSlice(s32 z);
    u16 * GetRow(s32 z, s32 r);

    Vec3f m_origin;  //origin coordinates in phyiscal units


  public:
    //Volume represents a 3D cube (with non-equal dimensions)
    //width,height,depth represent the number of voxels in each direction
    //wdist = physical horizontal distance between voxels in unit of choice
    //hdist = physical vertical distance between voxels in unit of choice
    //ddist = physical depth distance between voxels in unit of choice
    Volume(s32 width, s32 height, s32 depth, f32 wdist = -1, f32 hdist = -1, f32 ddist = -1);
    s32 Initialize();
    void Release();

    void SetSpacing(f32 wdist, f32 hdist, f32 ddist);
    void SetSpacing(Vec3f spacing);
    void SetOrigin(Vec3f origin);

    s32 m_width;  //number of voxels in volume in x direction
    s32 m_height; //""                         in y direction
    s32 m_depth;  //""                         in z direction

    f32 m_wdist;  //number of units between each voxel in x direction
    f32 m_hdist;  //number of units between each voxel in y direction
    f32 m_ddist;  //number of units between each voxel in z direction

    Vec3f origin; //origin of volume in units (the location of voxel index [0,0,0])
  };

  class VolumeCreator 
  {
  public:
    VolumeCreator();
  };
};