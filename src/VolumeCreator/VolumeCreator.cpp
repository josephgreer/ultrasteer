#include "VolumeCreator.h"

namespace Nf {
  ////////////////////////////////////////////////////////
  //Volume Class
  ////////////////////////////////////////////////////////
  Volume::Volume()
    : m_dims(0,0,0)
    , m_im(NULL)
    , m_data(NULL)
    , Nf::ParameterCollection("Volume")
    , Nf::Reinitializer()
  {
    ADD_VEC3D_PARAMETER(m_spacing, "Physical Voxel Spacing (um)", CALLBACK_POINTER(Reinitialize, Volume), this, Vec3d(83.0*4, 83.0*4, 2000.0), 
      Vec3d(83.0, 83.0, 83.0), Vec3d(2000,2000,2000), Vec3d(1,1,1));
    ADD_FLOAT_PARAMETER(m_imscale, "Image Scale", CALLBACK_POINTER(Reinitialize, Volume), this, 0.5, 0.05, 1, 0.01);
    ADD_VEC3D_PARAMETER(m_extent, "Volume Extent (cm)", CALLBACK_POINTER(Reinitialize, Volume), this, Vec3d(6,6,6), Vec3d(2,2,2), Vec3d(15,15,15), Vec3d(0.1,0.1,0.1));
    ADD_VEC2I_PARAMETER(m_cropHorz, "Horziontal Crop", NULL, this, Vec2i(0,0), Vec2i(0,0), Vec2i(500,500), Vec2i(1,1));
    ADD_VEC2I_PARAMETER(m_cropVert, "Vertical Crop", NULL, this, Vec2i(0,0), Vec2i(0,0), Vec2i(500,500), Vec2i(1,1));
  }


  static void outputMatrix(Matrix44d matrix)
  {
    NTrace("[%f %f %f %f; %f %f %f %f; %f %f %f %f; %f %f %f %f]\n", 
      matrix.m_data[0][0], matrix.m_data[0][1], matrix.m_data[0][2], matrix.m_data[0][3],
      matrix.m_data[1][0], matrix.m_data[1][1], matrix.m_data[1][2], matrix.m_data[1][3],
      matrix.m_data[2][0], matrix.m_data[2][1], matrix.m_data[2][2], matrix.m_data[2][3],
      matrix.m_data[3][0], matrix.m_data[3][1], matrix.m_data[3][2], matrix.m_data[3][3]);
  }


  static void outputMatrix(Matrix33d matrix)
  {
    NTrace("[%f %f %f; %f %f %f; %f %f %f]\n", 
      matrix.m_data[0][0], matrix.m_data[0][1], matrix.m_data[0][2], 
      matrix.m_data[1][0], matrix.m_data[1][1], matrix.m_data[1][2], 
      matrix.m_data[2][0], matrix.m_data[2][1], matrix.m_data[2][2]);
  }

  static void outputVector(Vec4d vec)
  {
    NTrace("[%f %f %f %f]\n", vec.x, vec.y, vec.z, vec.w);
  }

  static void outputVector(Vec3d vec)
  {
    NTrace("[%f %f %f]\n", vec.x, vec.y, vec.z);
  }

  s32 Volume::InitializeVolume(Reinitializer *reinit, Matrix33d &orientation, Vec3d frameOrigin, QtEnums::VOLUME_ORIGIN_LOCATION config)
  {
    m_reinit = reinit;
    Vec3d spacing = GetSpacing();
    Vec3d extent = m_extent->GetValue()*(10.0);
    f32 imscale = m_imscale->GetValue();

    m_dims = Vec3i((s32)ceil(extent.x/spacing.x), (s32)ceil(extent.y/spacing.y), (s32)ceil(extent.z/spacing.z));
    m_data = (u16 *)_aligned_malloc(m_dims.x*m_dims.y*m_dims.z*sizeof(u16), 64);
    memset(m_data, 0, m_dims.x*m_dims.y*m_dims.z*sizeof(u16));
    m_rowStride = m_dims.x;
    m_sliceStride = m_dims.x*m_dims.y;

    //Matrix stores axes of volume in physical coordinates
    Matrix33d axes = orientation*Matrix33d::Diagonal(extent.x*0.5, extent.y*0.5, extent.z);
    f64 mag1 = axes.Col(0).magnitude();
    f64 mag2 = axes.Col(1).magnitude();
    f64 mag3 = axes.Col(2).magnitude();
    f64 xy = axes.Col(0).dot(axes.Col(1));
    f64 xz = axes.Col(0).dot(axes.Col(2));
    f64 yz = axes.Col(1).dot(axes.Col(2));
    Matrix33d modAxes;
    switch(config) {
      case QtEnums::VOLUME_ORIGIN_LOCATION::VOL_LEFT: 
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), Vec3d(0,0,0));
          break;
        }
      case QtEnums::VOLUME_ORIGIN_LOCATION::VOL_RIGHT: 
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), axes.Col(2));
          break;
        }
      case QtEnums::VOLUME_ORIGIN_LOCATION::VOL_QUARTER_LEFT: 
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), axes.Col(2)*0.25);
          break;
        }
      case QtEnums::VOLUME_ORIGIN_LOCATION::VOL_QUARTER_RIGHT: 
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), axes.Col(2)*0.75);
          break;
        }
      case QtEnums::VOLUME_ORIGIN_LOCATION::VOL_MIDDLE: 
      default:
        {
          modAxes = Matrix33d::FromCols(axes.Col(0), axes.Col(1), axes.Col(2)*0.5);
          break;
        }
    }
    m_origin = frameOrigin-modAxes.Col(0)-modAxes.Col(1)-modAxes.Col(2);
    m_orientation = orientation;
    
    m_volumeToWorld = Matrix44d::FromOrientationAndTranslation(m_orientation*Matrix33d::Diagonal(spacing),m_origin);
    m_worldToVolume = m_volumeToWorld.Inverse();

    NTrace("Frame Origin:  \n");
    outputVector(frameOrigin);
    NTrace("Orientation:  \n");
    outputMatrix(orientation);
    NTrace("axes:  \n");
    outputMatrix(axes);
    NTrace("worldToVolume:  \n");
    outputMatrix(m_worldToVolume);
    NTrace("volumeToWorld:  \n");
    outputMatrix(m_volumeToWorld);
    NTrace("Origin:  \n");
    outputVector(m_origin);

    return m_data != NULL ? 1 : 0;
  }

  void Volume::Release()
  {
    if(m_data != NULL)
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

  Vec3d Volume::GetSpacing() const
  {
    return m_spacing->GetValue()*(1.0/1e3);
  }

  Vec3d Volume::GetOrigin() const
  {
    return m_origin;
  }

  Matrix33d Volume::GetOrientation() const
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


  void Volume::AddFrame(const IplImage *image, const Matrix33d &pose, const Vec3d &pos, const Matrix44d &calibration, Vec2d &origin, Vec2d &mpp)
  {
    f32 imscale = m_imscale->GetValue();

    //TODO OPTIMIZE!!!!!
    const IplImage *im = image;
    if(imscale != 1.0) {
      if(m_im == NULL)
        m_im = cvCreateImage(cvSize((s32)(image->width*imscale+0.5), (s32)(image->height*imscale+0.5)), IPL_DEPTH_8U, 1);
      cvResize(image, m_im);
      im = m_im;
    }        
    Vec2d scale(mpp.x/1000.0, mpp.y/1000.0);
    Vec4d imc, sensor, world, grid;
    Vec3i gridI;

    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, pos);

    Vec2i cropH = m_cropHorz->GetValue();
    Vec2i cropV = m_cropVert->GetValue();

    //TODO:  Check that rectangle falls within bounds of volume

    for(s32 y=cropV.x; y<im->height-cropV.y; y++) {
      const u8 *psrc = (const u8 *)(im->imageData+y*im->widthStep);
      for(s32 x=cropH.x; x<im->width-cropH.y; x++) {
        //Map image coord to world coord
        world = rpImageCoordToWorldCoord4(Vec2d(x/imscale,y/imscale), posePos, calibration, origin, scale);

        if(x == 160 && y == 120)
          int x = 0;

        //Now map to volume coord
        grid = m_worldToVolume*world;
        gridI = Vec3i((s32)(grid.x+0.5),(s32)(grid.y+0.5),(s32)(grid.z+0.5));

        //DEBUG_ASSERT(0 <= gridI.x && gridI.x < m_dims.x && 0 <= gridI.y && gridI.y < m_dims.y 
        //  && 0 <= gridI.z && gridI.z < m_dims.z);
        if(!(0 <= gridI.x && gridI.x < m_dims.x && 0 <= gridI.y && gridI.y < m_dims.y 
          && 0 <= gridI.z && gridI.z < m_dims.z))
          continue;

        //Cast to u16 and write to volume
        *GetCoordData(gridI) = ((u16)psrc[x]);
      }
    }

#if 0
    Vec3d st = WorldCoordinatesToVolumeCoordinates(rpImageCoordToWorldCoord3(Vec2d(0/imscale,0/imscale), posePos, calibration, start, scale));
    u16 *psrc = GetSlice((s32)(st.z+0.5));
    IplImage *imtest = cvCreateImageHeader(cvSize(m_dims.x, m_dims.y), IPL_DEPTH_16U, 1);
    imtest->widthStep = m_dims.x*2;
    imtest->imageData = (s8 *)psrc;
    cvSaveImage("C:/test.bmp", imtest);
    cvReleaseImageHeader(&imtest);
    s32 x = 0;
#endif

  }

  Cubed Volume::GetCubeExtent() const
  {
    Vec3d spacing = GetSpacing();
    Vec3d extent(m_dims.x*spacing.x, m_dims.y*spacing.y, m_dims.z*spacing.z);
    Vec3d ul = m_origin;
    return Cubed(ul, m_orientation*Matrix33d::Diagonal(extent));
  }

  Cubed Volume::GetPhysicalExtent() const
  {
    Vec3d spacing = GetSpacing();
    Vec3d extent(m_dims.x*spacing.x, m_dims.y*spacing.y, m_dims.z*spacing.z);
    return Cubed(Vec3d(0,0,0), Matrix33d::Diagonal(extent));
  }

  Vec3i Volume::GetDims() const
  {
    return m_dims;
  }

  ////////////////////////////////////////////////////////
  //End Volume Class
  ////////////////////////////////////////////////////////
  
#if 1
  ////////////////////////////////////////////////////////
  //VolumeCreator Class
  ////////////////////////////////////////////////////////
  VolumeCreator::VolumeCreator()
    : ParameterCollection("Volume Creator")
    , Reinitializer()
  {

  }

  VolumeCreator::~VolumeCreator()
  {
  }
  ////////////////////////////////////////////////////////
  //End VolumeCreator Class
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  //RPVolumeCreator Class
  ////////////////////////////////////////////////////////
  RPVolumeCreator::RPVolumeCreator()
    : m_index(-1)
    , VolumeCreator()
    , m_newData(false)
    , m_cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0)
    //, m_cal(12.4790, 0.9476, -0.0022, 0.0, 15.1253, 0.0019, 1.00, 0.0, 0.0213, 0.0202, 0.0064, 0.0, 0.0, 0.0, 0.0, 1.0)
    , m_init(false)
    , m_importer(NULL)
  {
    m_importer = vtkSmartPointer<vtkImageImport>::New();
    ADD_ENUM_PARAMETER(m_originLoc, "Start Loc.", CALLBACK_POINTER(Reinitialize, RPVolumeCreator), this, QtEnums::VOLUME_ORIGIN_LOCATION::VOL_RIGHT, "VOLUME_ORIGIN_LOCATION");
    ADD_CHILD_COLLECTION(&m_volume);
  }

  RPVolumeCreator::~RPVolumeCreator()
  {
  }

  Volume *RPVolumeCreator::GetNew()
  {
    m_newData = false;
    return m_newData ? &m_volume : NULL;
  }

  s32 RPVolumeCreator::Initialize(RPData rp, Reinitializer *reinit)
  {
    m_reinit = reinit;

    s32 rv = 1;
    if(rp.gps.valid) {
      Matrix44d tPose = Matrix44d::FromCvMat(rp.gps.pose);
      Matrix33d pose = tPose.GetOrientation();

      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, rp.gps.pos);
      Vec2d mpp(rp.mpp,rp.mpp);
      Vec2d origin = rp.origin;
      Vec2d mppScale(mpp.x/1000.0, mpp.y/1000.0);

      Matrix44d yep = Matrix44d::I();

      Vec3d x_axis = rpImageCoordToWorldCoord3(Vec2d(1.0,0.0), posePos, m_cal, origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0.0,0.0), posePos, m_cal, origin, mppScale);  //tPose*calibration*Vec4d(1.0, 0.0, (1.0)*mpp.x, 0.0)-tPose*calibration*Vec4d(1.0, 0.0, (0.0)*mpp.x, 0.0);
      Vec3d y_axis = rpImageCoordToWorldCoord3(Vec2d(0.0,1.0), posePos, m_cal, origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0.0,0.0), posePos, m_cal, origin, mppScale);//tPose*calibration*Vec4d(1.0, 1.0*mpp.y, 0.0, 0.0)-tPose*calibration*Vec4d(1.0, 0.0, 0.0, 0.0);
      x_axis = x_axis.normalized();
      y_axis = y_axis.normalized();
      Vec3d z_axis = x_axis.cross(y_axis);

      rv = m_volume.InitializeVolume(this, Matrix33d::FromCols(x_axis,y_axis,z_axis), 
        rpImageCoordToWorldCoord3(Vec2d(rp.b8->width/2.0, rp.b8->height/2.0), posePos, m_cal, origin, mppScale),
        (QtEnums::VOLUME_ORIGIN_LOCATION)m_originLoc->GetValue());
    }

    //vtkImageData importer
    Vec3d spacing = m_volume.GetSpacing();
    Vec3i dims = m_volume.GetDims();
    if(m_importer == NULL)
      m_importer = vtkSmartPointer<vtkImageImport>::New();
    m_importer->SetDataSpacing(spacing.x, spacing.y, spacing.z);
    m_importer->SetDataOrigin(0,0,0);
    m_importer->SetWholeExtent(0, dims.x-1, 0, dims.y-1, 0, dims.z-1);
    m_importer->SetDataExtentToWholeExtent();
    m_importer->SetDataScalarTypeToUnsignedShort();
    m_importer->SetNumberOfScalarComponents(1);
    m_importer->SetImportVoidPointer(this->GetVolumeOriginData());
    m_importer->Update();
    m_importer->Modified();

    m_init = true;

    return rv;
  }
  
  void RPVolumeCreator::Start()
  {
    assert(0);
  }

  void RPVolumeCreator::AddRPData(RPData rp)
  {
    if(!rp.gps.valid)
      return;

    if(!m_init && !Initialize(rp, m_reinit))
      assert(0);

    Matrix44d tPose = Matrix44d::FromCvMat(rp.gps.pose);
    Matrix33d pose = tPose.GetOrientation();

    Nf::NTrace("ROI x y w h %d %d %d %d\n", rp.roi.ul.x, rp.roi.ul.y, (rp.roi.lr-rp.roi.ul).x, (rp.roi.lr-rp.roi.ul).y);
    if(rp.roi.lr.x > rp.roi.ul.x && rp.roi.lr.y > rp.roi.ul.y)
      cvSetImageROI(rp.b8, cvRect(rp.roi.ul.x, rp.roi.ul.y, (rp.roi.lr-rp.roi.ul).x, (rp.roi.lr-rp.roi.ul).y)); 
    m_volume.AddFrame(rp.b8, pose, rp.gps.pos, m_cal,rp.origin,Vec2d(rp.mpp, rp.mpp));
    m_importer->Modified();
  }

  void RPVolumeCreator::Release()
  {
    m_volume.Release();
    m_init = false;
  }

  Vec3i RPVolumeCreator::GetVolumeDims()
  {
    return m_volume.GetDims();
  }

  u16 * RPVolumeCreator::GetVolumeOriginData()
  {
    return m_volume.GetCoordData(Vec3i(0,0,0));
  }

  Cubed RPVolumeCreator::GetVolumeCubeExtent() const
  {
    return m_volume.GetCubeExtent();
  }
  
  Cubed RPVolumeCreator::GetVolumePhysicalExtent() const
  {
    return m_volume.GetPhysicalExtent();
  }

  Matrix33d RPVolumeCreator::GetVolumeOrientation() const
  {
    return m_volume.GetOrientation();
  }

  vtkSmartPointer<vtkImageImport> RPVolumeCreator::GetImporter()
  {
    return m_importer;
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
    ADD_BOOL_PARAMETER(m_initialize, "Initialize", CALLBACK_POINTER(Reinitialize, RPVolumeCreator), this, false);
    ADD_OPEN_FILE_PARAMETER(m_usFile, "US File Data", CALLBACK_POINTER(Reinitialize, RPVolumeCreator), this, "V:/NeedleScan/test4/scan.b8", "Any File (*.*)");
  }

  RPFullVolumeCreator::~RPFullVolumeCreator()
  {
  }

  s32 RPFullVolumeCreator::Initialize(Reinitializer *reinit)
  {
    //TODO RPReader pointers created below are leaked.
    std::string fname = m_usFile->GetValue();
    fname = fname.substr(0,fname.find_last_of("."));

    char temp[200];
    sprintf(temp,"%s.b8",fname.c_str());
    m_rpReaders.AddReader(RPF_BPOST8, (RPReader *)new RPFileReader(temp));
    sprintf(temp,"%s.gps1",fname.c_str());
    m_rpReaders.AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(temp));

    RPData rp = m_rpReaders.GetNextRPData();
    if(!rp.gps.valid) {
      //TODO:  Release readers to avoid a leak!

      return -1;
    }

    s32 rv = RPVolumeCreator::Initialize(rp, reinit);
    rp.Release();

    return rv;
  }

  void RPFullVolumeCreator::AddRPData(RPData rp)
  {
    assert(0);
  }

  void RPFullVolumeCreator::Start()
  {
    if(m_initialize->GetValue()) {
      RPData rp = m_rpReaders.GetRPData(1);

      Matrix44d tPose;
      Matrix33d pose;
      //Since this is full volume creator, iterate through all frames.
      while(rp.gps.valid) {
        tPose = Matrix44d::FromCvMat(rp.gps.pose);
        pose = tPose.GetOrientation();

        m_volume.AddFrame(rp.b8, pose, rp.gps.pos, m_cal, rp.origin, Vec2d(rp.mpp, rp.mpp));
        rp.Release();

        rp = m_rpReaders.GetNextRPData();
      }

    }
    
  }

  ////////////////////////////////////////////////////////
  //End RPFullVolumeCreator Class
  ///////////////////////////////////////////////////////
#endif
};

