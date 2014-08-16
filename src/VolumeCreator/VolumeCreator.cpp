#include "VolumeCreator.h"

namespace Nf {
  ////////////////////////////////////////////////////////
  //Volume Class
  ////////////////////////////////////////////////////////
  Volume::Volume()
    : m_dims(0,0,0)
    , m_spacing(0,0,0)
    , m_scale(1.0)
  {
  }

  s32 Volume::InitializeVolume(Matrix33d &orientation, Vec3d frameOrigin, VOLUME_ORIGIN_LOCATION config, Vec3d extent, Vec3d spacing, f64 imscale)
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

    m_scale = imscale;

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

  u16 * Volume::GetCoordData(Vec3i coord)
  {
    return GetSlice(coord.z)+m_rowStride*coord.y+coord.x;
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

  Vec3d Volume::WorldCoordinatesToVolumeCoordinates(Vec3d worldCoords)
  {
    Vec4d wc(worldCoords.x, worldCoords.y, worldCoords.z, 1);
    Vec4d vc = m_worldToVolume*wc;
    return Vec3d(vc.x, vc.y, vc.z);
  }

  Vec3d Volume::VolumeCoordinatesToWorldCoordinates(Vec3d volCoords)
  {
    Vec4d vc(volCoords.x, volCoords.y, volCoords.z, 1);
    Vec4d wc = m_volumeToWorld*vc;
    return Vec3d(wc.x, wc.y, wc.z);
  }

  void Volume::AddFrame(const IplImage *image, const Matrix33d &pose, const Vec3d &pos, const Matrix44d &calibration, const Vec2d &mpp)
  {
    //TODO OPTIMIZE!!!!!
    const IplImage *im = image;
    if(m_scale != 1.0) {
      IplImage *_im = cvCreateImage(cvSize((s32)(image->width*m_scale+0.5), (s32)(image->height*m_scale+0.5)), IPL_DEPTH_8U, 1);
      cvResize(image, _im);
      im = _im;
    }        
    Vec2d start(320,0);
    Vec2d scale(mpp.x/1000.0, mpp.y/1000.0);
    Vec4d imc, sensor, world, grid;
    Vec3i gridI;

    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, pos);

    for(s32 y=0; y<im->height; y++) {
      const u8 *psrc = (const u8 *)(image->imageData+y*image->widthStep);
      for(s32 x=0; x<im->width; x++) {
        //Map image coord to world coord
        imc = Vec4d(1, (y-start.y)*scale.y, (x-start.x)*scale.x, 1);
        sensor = calibration*imc;
        world = posePos*sensor;

        //Now map to volume coord
        grid = m_worldToVolume*world;
        gridI = Vec3i((s32)(grid.x+0.5),(s32)(grid.y+0.5),(s32)(grid.z+0.5));

        //Cast to u16 and write to volume
        *GetCoordData(gridI) = (u16)psrc[x];
      }
    }
  }

  ////////////////////////////////////////////////////////
  //End Volume Class
  ////////////////////////////////////////////////////////
  
#if 1
  ////////////////////////////////////////////////////////
  //RPVolumeCreator Class
  ////////////////////////////////////////////////////////
  RPVolumeCreator::RPVolumeCreator()
    : m_index(-1)
    , m_newData(false)
  {
  }

  RPVolumeCreator::~RPVolumeCreator()
  {
  }

  Volume *RPVolumeCreator::GetNew()
  {
    m_newData = false;
    return m_newData ? &m_volume : NULL;
  }

  void RPVolumeCreator::Release()
  {
    m_volume.Release();
  }

  ////////////////////////////////////////////////////////
  //End RPVolumeCreator Class
  ////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////
  //RPFullVolumeCreator Class
  ////////////////////////////////////////////////////////
  RPFullVolumeCreator::RPFullVolumeCreator()
    : RPVolumeCreator()
  {
  }

  RPFullVolumeCreator::~RPFullVolumeCreator()
  {
  }

  s32 RPFullVolumeCreator::Initialize(const char *path, Matrix44d &calibration, Vec2d &mpp, VOLUME_ORIGIN_LOCATION config, Vec3d &extent, Vec3d &spacing, f64 imscale)
  {
    //TODO RPReader pointers created below are leaked.
    char temp[200];
    sprintf(temp,"%s/scan.b8",path);
    m_rpReaders.AddReader(RPF_BPOST8, (RPReader *)new RPFileReader(temp));
    sprintf(temp,"%s/scan.gps1",path);
    m_rpReaders.AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(temp));

    m_cal = calibration;
    m_mpp = mpp;



    RPData rp = m_rpReaders.GetNextRPData();
    if(!rp.gps.valid) {
      //TODO:  Release readers to avoid a leak!

      return -1;
    }

    Matrix44d tPose = Matrix44d::FromCvMat(rp.gps.pose);
    Matrix33d pose = tPose.GetOrientation();
    s32 rv = m_volume.InitializeVolume(pose, rp.gps.pos, config, extent, spacing, imscale);
    if(rv < 0)
      return rv;

    //Since this is full volume creator, iterate through all frames.
    while(rp.gps.valid) {
      tPose = Matrix44d::FromCvMat(rp.gps.pose);
      pose = tPose.GetOrientation();

      m_volume.AddFrame(rp.b8, pose, rp.gps.pos, m_cal, m_mpp);

      rp.Release();
    }


    return 0;
  }

  ////////////////////////////////////////////////////////
  //End RPFullVolumeCreator Class
  ///////////////////////////////////////////////////////
#endif
};

