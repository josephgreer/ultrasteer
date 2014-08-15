#include "VolumeCreator.h"

namespace Nf {
  ////////////////////////////////////////////////////////
  //Volume Class
  ////////////////////////////////////////////////////////
  Volume::Volume()
    : m_dims(0,0,0)
    , m_spacing(0,0,0)
  {
  }

  s32 Volume::InitializeVolume(Matrix33d &orientation, Vec3d frameOrigin, VOLUME_ORIGIN_LOCATION config, Vec3d extent, Vec3d spacing)
  {
    m_dims = Vec3i((s32)ceil(extent.x/spacing.x), (s32)ceil(extent.y/spacing.y), (s32)ceil(extent.z/spacing.z));
    m_data = (u16 *)_aligned_malloc(m_dims.x*m_dims.y*m_dims.z*sizeof(u16), 64);
    m_rowStride = m_dims.x;
    m_sliceStride = m_dims.x*m_dims.y;

    //Matrix stores axes of volume in physical coordinates
    Matrix33d axes = Matrix33d::Diaganol(extent*0.5)*orientation;
    Matrix33d modAxes;
    switch(config) {
      case VOL_EDGE: 
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), Vec3d(0,0,0));
          break;
        }
      case VOL_QUARTER: 
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), axes.Col(2)*0.25);
          break;
        }
      case VOL_MIDDLE:
      default:
        {
          modAxes = axes;
          break;
        }
    }
    m_origin = frameOrigin-modAxes.Col(0)-modAxes.Col(1)-modAxes.Col(2);
    m_orientation = orientation;
    
    m_volumeToWorld = Matrix44d::FromOrientationAndTranslation(m_orientation*Matrix33d::Diaganol(m_spacing),m_origin);
    m_worldToVolume = m_volumeToWorld.Inverse();

    return m_data != NULL ? 1 : 0;
  }

  void Volume::Release()
  {
    _aligned_free(m_data);
    m_data = NULL;
    m_rowStride = -1;
    m_sliceStride = -1;

  }

  u16 * Volume::GetSlice(s32 z)
  {
    return m_data+m_sliceStride*z;
  }


  u16 * Volume::GetRow(s32 z, s32 r)
  {
    return GetSlice(z)+m_rowStride*r;
  }

  Vec3d Volume::GetSpacing()
  {
    return m_spacing;
  }

  Vec3d Volume::GetOrigin()
  {
    return m_origin;
  }

  Matrix33d Volume::GetOrientation()
  {
    return m_orientation;
  }

  ////////////////////////////////////////////////////////
  //End Volume Class
  ////////////////////////////////////////////////////////
};
