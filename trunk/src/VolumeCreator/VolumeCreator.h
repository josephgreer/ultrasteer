#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "SegmentCore.h"
#include "RPFileReader.h"
#include "UICore.h"
#include <vtkImageImport.h>

namespace Nf {

  class Reinitializer
  {
  protected:
    Reinitializer *m_reinit;

  public:
    Reinitializer() 
      : m_reinit(NULL)
    {
    }

    virtual void Reinitialize()
    {
      if(m_reinit)
        m_reinit->Reinitialize();
    }
  };

  class Volume : public Nf::ParameterCollection, public Reinitializer
  {
  protected:
    u16 *m_data;
    s32 m_sliceStride;  //in pixels (not bytes)
    s32 m_rowStride;    //in pixels (not bytes)

    u16 * GetSlice(s32 z);
    u16 * GetRow(s32 z, s32 r);

    Vec3i m_dims;   //number of voxels in x,y,z axis resp.

    Vec3d m_origin; //origin of volume in units (the location of voxel index [0,0,0])
    Matrix33d m_orientation;  //Columns of matrix correspond to x-axis, y-axis, and z-axis of the volume, resp. 

    Matrix44d m_worldToVolume;    //Matrix part of transform from world coordiantes to volume indices
    Matrix44d m_volumeToWorld;    //Matrix part of transfrom from volume indices to world coordinates (m_volumeToWorld = m_worldToVolume^{-1})   

    //User Parameters
    //config:  Specifies the way the volume will be allocated around the frame. /
    //  VOL_EDGE should be used if the location of the frame marks the edge of the volume
    //  VOL_QUARTER should be used if the location of the frame is near the edge but you want some space
    //  VOL_MIDDLE should be used if the location of the frame will be in the middle of the volume
    //extent:  Specifies the extent of the volume in physical units.  It will be converted to numbers of voxels by taking into
    //  account spacing
    //spacing:  Specifiy the distance between voxels in each of the axes in phyiscal units.
    std::tr1::shared_ptr < Nf::Vec3dParameter > m_spacing;                      //spacing (physical units)
    std::tr1::shared_ptr < Nf::FloatParameter > m_imscale;                      //scale factor
    std::tr1::shared_ptr < Nf::Vec3dParameter > m_extent;                       //volume extent (physical units
    std::tr1::shared_ptr < Nf::Vec2dParameter > m_mpp;                          //Vector containing microns per pixel of b-mode image in x&y dimensions

    IplImage *m_im;

  public:
    Volume();
    //Volume represents a 3D cube (with non-equal dimensions)
    //Initialize volume from a frame and extent around the frame
    //orientation:  Columns of orientation matrix specify the directions of x, y, and z axis of the cube
    //frameOrigin: specifies the location of the middle of the frame (in physical units).  Memory for a  3D
    //  volume will be allocated around the frameOrigin.  
    s32 InitializeVolume(Reinitializer *reinit, Matrix33d &orientation, Vec3d frameOrigin, QtEnums::VOLUME_ORIGIN_LOCATION config);
    void Release();

    Vec3d GetSpacing() const;
    Vec3d GetOrigin() const;
    Matrix33d GetOrientation() const;
    Vec3i GetDims() const;
    u16 * GetCoordData(Vec3i coord);
    Cubed GetCubeExtent() const;
    Cubed GetPhysicalExtent() const;
    Vec2d GetMPP() const;

    Vec3d WorldCoordinatesToVolumeCoordinates(Vec3d worldCoords);
    Vec3d VolumeCoordinatesToWorldCoordinates(Vec3d volCoords);

    void AddFrame(const IplImage *image, const Matrix33d &pose, const Vec3d &pos, const Matrix44d &calibration, Vec2d &origin, Vec2d &mpp);

    CLASS_CALLBACK(Reinitialize, Volume);
  };


  class VolumeCreator : public ParameterCollection, public Reinitializer
  {
  public:
    VolumeCreator();
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
  class RPVolumeCreator : public VolumeCreator
  {
  protected:
    bool m_init;                                                                //Are we initialized
    s32 m_index;                                                                //Current frame index.  Valid when m_rm != RPVM_READ_ALL              
    bool m_newData;                                                             //New data since last GetNew() call?
    Volume m_volume;                                                            //Self explanatory
    Matrix44d m_cal;                                                            //Calibration Matrix for transducer (maps image coords to world coords)
    vtkSmartPointer<vtkImageImport> m_importer;

    //User Parameters
    std::tr1::shared_ptr < Nf::EnumParameter > m_originLoc;                     //location of origin with respect to first frame

  public:
    RPVolumeCreator();
    virtual ~RPVolumeCreator();
    Volume *GetNew();
    virtual void Start();
    virtual void AddRPData(RPData rp);
    virtual Vec3i GetVolumeDims();
    virtual u16 * GetVolumeOriginData();
    virtual Cubed GetVolumeCubeExtent() const;
    virtual Cubed GetVolumePhysicalExtent() const;
    virtual Matrix33d GetVolumeOrientation() const;
    virtual Vec2d GetMPP() const { return m_volume.GetMPP(); }
    virtual Matrix44d GetCal() const { return m_cal; }
    virtual s32 Initialize(RPData rp, Reinitializer *reinit);
    void Release();
    vtkSmartPointer<vtkImageImport> GetImporter();
    CLASS_CALLBACK(Reinitialize, RPVolumeCreator);
  };

  class RPFullVolumeCreator : public RPVolumeCreator
  {
  public:
    RPFullVolumeCreator();
    virtual ~RPFullVolumeCreator();
    s32 Initialize(Reinitializer *reinit);
    void Start();
    virtual void AddRPData(RPData rp);
    CLASS_CALLBACK(Reinitialize, RPFullVolumeCreator);

  protected:

    RPFileReaderCollection m_rpReaders;                                         //What will actually do the reading for us
    std::tr1::shared_ptr < Nf::FileParameter > m_usFile;                        //ultrasound file)
    std::tr1::shared_ptr < Nf::BoolParameter > m_initialize;                    //initialize
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