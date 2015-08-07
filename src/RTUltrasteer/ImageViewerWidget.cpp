#include "ImageViewerWidget.h"
#include "MouseInteractorStylePP.h"
#include <vtkCamera.h>
#include <vtkDataObject.h>
#include <Teleoperation2DWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkWorldPointPicker.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkObjectFactory.h>
#include <vtkPointPicker.h>
#include <stdio.h>
#include <vtkProperty.h>

#define MAX_VAR_NORM 10;

namespace Nf
{
  ////////////////////////////////////////////////////////
  // Begin ImageViwerWidget
  ////////////////////////////////////////////////////////
  ImageViewerWidget::ImageViewerWidget(QWidget *parent)
    : ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , Nf::ParameterCollection("Image Viewer Widget")
    , m_rp()
    , m_init(false)
    , m_useTrackball(false)
  {
    m_mapper = vtkSmartPointer<vtkImageMapper>::New();
    m_importer = vtkSmartPointer<vtkImageImport>::New();
    m_imageActor = vtkSmartPointer<vtkImageActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();

    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    m_flip = vtkSmartPointer<vtkImageFlip>::New();
  }

  ImageViewerWidget::~ImageViewerWidget()
  {
    m_rp.Release();
  }

  void ImageViewerWidget::SetImage(const RPData *rp)
  {
    if(rp->color != NULL) {
      m_rp.Release();
      m_rp = rp->Clone();
    }

    if(m_rp.color) {
      IplImage *orig = m_rp.color;
      CvSize s = cvSize(orig->width, orig->height);
      int d = orig->depth;
      
      // THIS IS A HACK TO GET THE DOPPLER TO DISPLAY, NEED TO TALK TO JOEY ABOUT THIS
      IplImage* R = cvCreateImage(s, d, 1);
      IplImage* G = cvCreateImage(s, d, 1);
      IplImage* B = cvCreateImage(s, d, 1);
      IplImage* X = cvCreateImage(s, d, 1);
      IplImage* im = cvCreateImage(s, d, 4);
      cvSplit(orig, B, G, R, X);
      cvMerge(R, G, B, R, im);

      // Set up importer
      m_importer->SetDataOrigin(0,0,0);
      m_importer->SetDataSpacing(1,1,1);
      //m_importer->SetDataSpacing(rp->mpp/1000.0,rp->mpp/1000.0,1);
      m_importer->SetWholeExtent(0, im->width-1, 0, im->height-1, 0, 0);
      m_importer->SetDataExtentToWholeExtent();
      m_importer->SetDataScalarTypeToUnsignedChar();
      m_importer->SetNumberOfScalarComponents(im->nChannels);
      m_importer->SetImportVoidPointer(im->imageData);
      m_importer->Modified();
      m_importer->Update();
    }

    // Initialize
    if(!m_init) {

      m_flip->SetFilteredAxes(1);
      m_flip->SetInputData((vtkDataObject *)m_importer->GetOutput());
      m_flip->Update();
      m_imageActor->SetInputData(m_flip->GetOutput());
      m_renderer->AddActor2D(m_imageActor);

      //m_imageActor->RotateZ(180);
      //m_imageActor->RotateY(180);
      //m_imageActor->SetInputData(m_importer->GetOutput());
      //m_renderer->AddActor(m_imageActor);

      this->GetRenderWindow()->AddRenderer(m_renderer);

      // Set up style      
      if(!m_useTrackball) {
        vtkSmartPointer<vtkInteractorStyleImage> style = 
          vtkSmartPointer<vtkInteractorStyleImage>::New();
        m_interactor->SetInteractorStyle(style);
      } else {
        vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
          vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        m_interactor->SetInteractorStyle(style);
      }

      // Render and start interaction
      m_interactor->SetRenderWindow(this->GetRenderWindow());
      m_interactor->Initialize();
      f64 *bounds = m_imageActor->GetBounds();
      m_renderer->ResetCamera(bounds);
      vtkCamera *cam = m_renderer->GetActiveCamera();
      m_renderer->SetActiveCamera(cam);
    } 

    if(m_rp.color)
      QVTKWidget::update();
    m_init = true;
  }
  ////////////////////////////////////////////////////////
  // End ImageViwerWidget
  ////////////////////////////////////////////////////////

 
  ////////////////////////////////////////////////////////
  // Begin Image3DImagePlaneWidget
  ////////////////////////////////////////////////////////
  Image3DImagePlaneWidget::Image3DImagePlaneWidget(QWidget *parent)
    : ImageViewerWidget(parent)
    , SphereContainer()
    , m_cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0)
    , m_frameBoundaries((CubeVisualizer *)NULL)
  {
    m_useTrackball = true;

    ADD_ACTION_PARAMETER(m_setViewXY, "Set View XY", CALLBACK_POINTER(onSetViewXY, Image3DImagePlaneWidget), this, true); 
    ADD_ACTION_PARAMETER(m_setViewXZ, "Set View XZ", CALLBACK_POINTER(onSetViewXZ, Image3DImagePlaneWidget), this, true); 
    ADD_ACTION_PARAMETER(m_setViewYZ, "Set View YZ", CALLBACK_POINTER(onSetViewYZ, Image3DImagePlaneWidget), this, true); 
    ADD_BOOL_PARAMETER(m_showFrameBoundaries, "Show Frame Boundary", CALLBACK_POINTER(onShowExtrasChanged, Image3DImagePlaneWidget), this, false);
  }

  Image3DImagePlaneWidget::~Image3DImagePlaneWidget()
  {
  }

  void Image3DImagePlaneWidget::SetImage(const RPData *rp)
  {
    ImageViewerWidget::SetImage(rp);
    if(m_init) {
      Matrix44d tPose = Matrix44d::FromCvMat(m_rp.gps.pose);
      Matrix33d pose = tPose.GetOrientation();

      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_rp.gps.pos);
      Vec2d mpp(m_rp.mpp,m_rp.mpp);
      Vec2d origin = m_rp.origin;
      Vec2d mppScale(mpp.x/1000.0, mpp.y/1000.0);

      Vec3d x_axis = rpImageCoordToWorldCoord3(Vec2d(1.0,0.0), posePos, m_cal, origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0.0,0.0), posePos, m_cal, origin, mppScale);  
      Vec3d y_axis = rpImageCoordToWorldCoord3(Vec2d(0.0,1.0), posePos, m_cal, origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0.0,0.0), posePos, m_cal, origin, mppScale);
      x_axis = x_axis.normalized();
      y_axis = y_axis.normalized();
      Vec3d z_axis = x_axis.cross(y_axis);

      m_imageActor->PokeMatrix(Matrix44d::FromOrientationAndTranslation(Matrix33d::FromCols(x_axis,y_axis,z_axis), rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, m_cal, origin, mppScale)).GetVTKMatrix());
      m_imageActor->Update();

      if(m_frameBoundaries)
        m_renderer->RemoveActor(m_frameBoundaries->GetActor());
      u8 cubeColor[3] = {128, 0, 0};
      m_frameBoundaries = std::tr1::shared_ptr < CubeVisualizer > (new CubeVisualizer(rp->GetFrameBoundaries(m_cal), cubeColor));
      UpdateFrameBoundaries();
    }
    QVTKWidget::update();
  }

  void Image3DImagePlaneWidget::UpdateFrameBoundaries()
  {
    if(m_showFrameBoundaries->GetValue() && m_frameBoundaries) {
      m_renderer->AddActor(m_frameBoundaries->GetActor());
    } else if(m_frameBoundaries) {
      m_renderer->RemoveActor(m_frameBoundaries->GetActor());
    }
    if(m_rp.gps2.valid) {
      if(m_sphereVis == NULL) {
        CreateSphere(m_rp.gps2.pos, 1.0);
        m_renderer->AddActor(m_sphereVis->GetActor());
      } else {
        m_sphereVis->SetCenter(m_rp.gps2.pos);
        m_sphereVis->SetRadius(1.0);
      }
    }
  }

  void Image3DImagePlaneWidget::onShowExtrasChanged()
  {
    UpdateFrameBoundaries();
    QVTKWidget::update();
  }

  void Image3DImagePlaneWidget::SetUSVisView(s32 axis1, s32 axis2)
  {
    vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    Matrix44d tPose = Matrix44d::FromCvMat(m_rp.gps.pose);
    Matrix33d pose = tPose.GetOrientation();

    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_rp.gps.pos);
    Vec2d mpp(m_rp.mpp,m_rp.mpp);
    Vec2d origin = m_rp.origin;
    Vec2d mppScale(mpp.x/1000.0, mpp.y/1000.0);

    Vec3d x_axis = rpImageCoordToWorldCoord3(Vec2d(1.0,0.0), posePos, m_cal, origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0.0,0.0), posePos, m_cal, origin, mppScale);  
    Vec3d y_axis = rpImageCoordToWorldCoord3(Vec2d(0.0,1.0), posePos, m_cal, origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0.0,0.0), posePos, m_cal, origin, mppScale);
    x_axis = x_axis.normalized();
    y_axis = y_axis.normalized();
    Vec3d z_axis = x_axis.cross(y_axis);

    Matrix33d orientation = Matrix33d::FromCols(x_axis, y_axis, z_axis);

    Vec3d up = orientation.Col(axis1)*-1.0;
    Vec3d focal = orientation.Col(axis2)*-1.0;
    camera->SetPosition(0,0,0);
    camera->SetFocalPoint(focal.x, focal.y, focal.z);
    camera->SetViewUp(up.x, up.y, up.z);

    f64 *bounds = m_imageActor->GetBounds();
    m_renderer->SetActiveCamera(camera);
    m_renderer->ResetCamera(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], 
      bounds[5]);
    m_renderer->GetActiveCamera()->Zoom(0.8);
    this->update();
  }

  void Image3DImagePlaneWidget::onSetViewXY()
  {
    SetUSVisView(1,2);
  }

  void Image3DImagePlaneWidget::onSetViewXZ()
  {
    SetUSVisView(0,1);
  }

  void Image3DImagePlaneWidget::onSetViewYZ()
  {
    SetUSVisView(1,0);
  }
  ////////////////////////////////////////////////////////
  // End Image3DImagePlaneWidget
  ////////////////////////////////////////////////////////
  
  
  ////////////////////////////////////////////////////////
  // Begin ImageViewer2DTeleoperationWidget
  ////////////////////////////////////////////////////////
  ImageViewer2DTeleoperationWidget::ImageViewer2DTeleoperationWidget(QWidget *parent)
    : ImageViewerWidget(parent) 
    , m_initTeleop(false)
    , m_mask(NULL)
  {
    m_textActor1 = vtkSmartPointer<vtkTextActor>::New();
    m_textActor2 = vtkSmartPointer<vtkTextActor>::New();
    m_textActor3 = vtkSmartPointer<vtkTextActor>::New();
    m_textActor4 = vtkSmartPointer<vtkTextActor>::New();
    m_pointPicker = vtkSmartPointer<vtkPointPicker>::New();
    m_maskImporter = vtkSmartPointer<vtkImageImport>::New();
    m_mapTransparency = vtkSmartPointer<vtkImageMapToColors>::New();
    m_maskActor = vtkSmartPointer<vtkImageActor>::New();
    m_maskFlip = vtkSmartPointer<vtkImageFlip>::New();
    m_lookUpTable = vtkSmartPointer<vtkLookupTable>::New();
  }

  ImageViewer2DTeleoperationWidget::~ImageViewer2DTeleoperationWidget()
  {
    if(m_mask != NULL)
      cvReleaseImage(&m_mask);
  }

  void ImageViewer2DTeleoperationWidget::SetImage(const RPData *rp)
  {   
    ImageViewerWidget::SetImage(rp);
    
    if(!m_initTeleop){
      const IplImage *im = rp->b8;
      // Create blank mask image
      m_mask = cvCloneImage(im);
      cvZero(m_mask);

      // Set up mask importer
      m_maskImporter->SetDataOrigin(0,0,0);
      //m_maskImporter->SetDataSpacing(rp->mpp/1000.0,rp->mpp/1000.0,1);
      m_maskImporter->SetDataSpacing(1,1,1);
      m_maskImporter->SetWholeExtent(0, m_mask->width-1, 0, m_mask->height-1, 0, 0);
      m_maskImporter->SetDataExtentToWholeExtent();
      m_maskImporter->SetDataScalarTypeToUnsignedChar();
      m_maskImporter->SetNumberOfScalarComponents(1);
      m_maskImporter->SetImportVoidPointer(m_mask->imageData);
      m_maskImporter->Update();
      m_maskImporter->Modified();

      // Set up lookup table for transparency
      m_lookUpTable->SetNumberOfTableValues(2);
      m_lookUpTable->SetRange(0.0,1.0);
      m_lookUpTable->SetTableValue( 0.0, 0.0, 0.0, 0.0, 0.0 ); //label 0 is transparent
      m_lookUpTable->SetTableValue( 1.0, 0.0, 0.0, 1.0, 1.0 ); //label 1 is opaque and blue
      m_lookUpTable->Build();

      // Map the image values to transparencies
      m_mapTransparency = vtkSmartPointer<vtkImageMapToColors>::New();
      m_mapTransparency->SetLookupTable(m_lookUpTable);
      m_mapTransparency->PassAlphaToOutputOn();
      m_mapTransparency->SetInputData((vtkDataObject *)m_maskImporter->GetOutput());

      // Flip the overlay image
      m_maskFlip->SetFilteredAxes(1);
      m_maskFlip->SetInputConnection(m_mapTransparency->GetOutputPort());
      m_maskFlip->Update();
      m_maskActor->GetMapper()->SetInputConnection(m_mapTransparency->GetOutputPort());
      m_renderer->AddActor2D(m_maskActor);
      //m_maskActor->RotateZ(180);
      //m_maskActor->RotateY(180);
      //m_maskActor->GetMapper()->SetInputConnection(m_mapTransparency->GetOutputPort());
      //m_renderer->AddActor2D(m_maskActor);

      // Add text overlay for selected target coordinates
      m_textActor1->GetTextProperty()->SetFontSize ( 24 );
      m_textActor1->SetPosition( 10, 10 );
      m_renderer->AddActor2D ( m_textActor1 );
      m_textActor1->SetInput ( "Initialized" );
      m_textActor1->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Add text overlay for user instructions
      m_textActor2->GetTextProperty()->SetFontSize ( 24 );
      //int* size = m_renderer->GetSize();
      m_textActor2->SetPosition( 10, 400 );
      m_renderer->AddActor2D ( m_textActor2 );
      m_textActor2->SetInput ( "" );
      m_textActor2->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Add text overlay for segmentation results
      m_textActor3->GetTextProperty()->SetFontSize ( 24 );
      m_textActor3->SetPosition( 10, 200 );
      m_renderer->AddActor2D ( m_textActor3 );
      m_textActor3->SetInput ( "" );
      m_textActor3->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Add text overlay for estimate variance
      m_textActor3->GetTextProperty()->SetFontSize ( 24 );
      m_textActor3->SetPosition( 200, 200 );
      m_renderer->AddActor2D ( m_textActor3 );
      m_textActor3->SetInput ( "" );
      m_textActor3->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Set up interaction 
      vtkSmartPointer<MouseInteractorStylePP> style = 
        vtkSmartPointer<MouseInteractorStylePP>::New();
      style->SetImageViewerWidget(this);
      style->SetTeleoperation2DWidget(m_teleoperationWidget);
      style->SetRenderWindowInteractor(m_interactor);
      m_interactor->SetInteractorStyle(style);

      // Set up point picker
      m_pointPicker->SetTolerance(0.0);
      m_pointPicker->PickFromListOn();
      m_pointPicker->AddPickList(m_imageActor);
      m_interactor->SetPicker(m_pointPicker);

    }
    m_initTeleop = true;

    // Update the VTK rendering
    m_maskImporter->Update();
    m_maskImporter->Modified();
    m_flip->Modified();
    m_flip->Update();
    m_renderer->Render();
    this->GetRenderWindow()->Render();
    this->repaint();
  } 

  void ImageViewer2DTeleoperationWidget::SetTeleoperation2DWidget(Teleoperation2DWidget *widget)
  { 
    m_teleoperationWidget = widget;  
  }

  void ImageViewer2DTeleoperationWidget::SetTargetText(Vec2d px, Vec3d wpt)
  {
    // Format the click position and print over image
    char str [100];
    int n = sprintf(str, "i = {%.1f, %.1f}\nt = {%.2f, %.2f, %.2f}", px.x, px.y, wpt.x, wpt.y, wpt.z);
    m_textActor1->SetInput(str);

    // Update the VTK rendering
    this->repaint();
  }

  void ImageViewer2DTeleoperationWidget::SetInstructionText(char* str)
  {
    m_textActor2->SetInput(str);
    int* size = m_renderer->GetSize();
    m_textActor2->SetPosition( 10, size[1]-40 );
    this->repaint();
  }

  void ImageViewer2DTeleoperationWidget::SetSegmentationText(Matrix33d R, Vec3d p)
  {
    // Format the click position and print over image
    char str [100];
    int n = sprintf(str, "p = {%.2f, %.2f, %.2f}\nR = \n%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f", 
      p.x, p.y, p.z,
      R.m_data[0][0], R.m_data[0][1], R.m_data[0][2], 
      R.m_data[1][0], R.m_data[1][1], R.m_data[1][2], 
      R.m_data[2][0], R.m_data[2][1], R.m_data[2][2]);
    m_textActor3->SetInput(str);

    int* size = m_renderer->GetSize();
    m_textActor3->SetPosition( 10, int(size[1]/2) );

    // Update the VTK rendering
    this->repaint();
  }

  void ImageViewer2DTeleoperationWidget::SetVarianceText(Vec3d var)
  {
    char str [100];
    int n = sprintf(str,"s_x = %.2f\ns_y = %.2f\ns_z = %.2f",var.x,var.y,var.z);
    m_textActor4->SetInput(str);

    int* size = m_renderer->GetSize();
    m_textActor4->SetPosition( 10, size[0]-100 );
    
    f32 danger = var.normalized()/MAX_VAR_NORM;
    m_textActor4->GetTextProperty()->SetColor( 1.0,1.0-danger,1.0-danger );

    this->repaint();
  }

  void ImageViewer2DTeleoperationWidget::SetPoseOverlay(int r, Vec2d t, bool show, Vec2d p, Vec2d pz, Vec2d py)
  {
    // Zero the image and draw a circle at the click point
    cvZero(m_mask);

    if( r > -1 ) // If we have a circle to draw
    {
      cvCircle(m_mask,cvPoint(t.x,m_mask->height-1-t.y),r,cvScalar(1.0),5,CV_AA);
    }

    if( show ) // If we want to show the tip frame       
    {
      cvLine(m_mask,cvPoint(p.x,m_mask->height-1-p.y),cvPoint(pz.x, m_mask->height-1-pz.y),cvScalar(1.0),3,CV_AA);
      cvLine(m_mask,cvPoint(p.x,m_mask->height-1-p.y),cvPoint(py.x, m_mask->height-1-py.y),cvScalar(1.0),3,CV_AA);
    }
      
    if( show || r > -1 )
    {
      // Update the VTK rendering
      m_maskImporter->Update();
      m_maskImporter->Modified();
      this->repaint();
    }

  }

  void ImageViewer2DTeleoperationWidget::getImageDim(int &w, int &h)
  {
    w = m_rp.b8->width;
    h = m_rp.b8->height;
  }
  ////////////////////////////////////////////////////////
  // End ImageViewer2DTeleoperationWidget
  ////////////////////////////////////////////////////////


}
