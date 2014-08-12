#include "VolumeCreator.h"

namespace Nf {
  ////////////////////////////////////////////////////////
  //Volume Class
  ////////////////////////////////////////////////////////
  Volume::Volume(s32 width, s32 height, s32 depth, f32 wdist, f32 hdist, f32 ddist)
    : m_width(width)
    , m_height(height)
    , m_depth(depth)
    , m_wdist(wdist)
    , m_hdist(hdist)
    , m_ddist(ddist)
  {
  }

  s32 Volume::Initialize()
  {
    m_data = (u16 *)_aligned_malloc(m_width*m_height*m_depth*sizeof(u16), 64);
    m_rowStride = m_width;
    m_sliceStride = m_width*m_height;
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

  void Volume::SetSpacing(f32 wdist, f32 hdist, f32 ddist)
  {
    m_wdist = wdist;
    m_hdist = hdist;
    m_ddist = ddist;
  }

  void Volume::SetSpacing(Vec3f spacing)
  {
    SetSpacing(spacing.x, spacing.y, spacing.z);
  }

  ////////////////////////////////////////////////////////
  //End Volume Class
  ////////////////////////////////////////////////////////
};
