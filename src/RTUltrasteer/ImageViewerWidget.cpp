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
  ImageViewerWidget::ImageViewerWidget(QWidget *parent, const char *name)
    : ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , Nf::ParameterCollection(name)
    , m_rp()
    , m_init(false)
    , m_useTrackball(false)
    , m_temp(NULL)
    , m_cal(TRANSDUCER_CALIBRATION_COEFFICIENTS)
  {
    m_mapper = vtkSmartPointer<vtkImageMapper>::New();
    m_importer = vtkSmartPointer<vtkImageImport>::New();
    m_imageActor = vtkSmartPointer<vtkImageActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
		m_flip = vtkSmartPointer<vtkImageFlip>::New();
    m_flipCast = vtkSmartPointer<vtkImageCast>::New();

		ADD_BOOL_PARAMETER(m_flipImage, "Flip Image", NULL, this, false);

    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();    
  }

  ImageViewerWidget::~ImageViewerWidget()
  {
    m_rp.Release();
    if(m_temp != NULL) {
      cvReleaseImage(&m_temp);
      m_temp = NULL;
    }
  }

  static s32 correctNumChannels(const IplImage *a)
  {
    if(a->nChannels == 4)
      return 3;
    return a->nChannels;
  }

  static bool correctSizeImage(const IplImage *a, const IplImage *b)
  {
    if(a->width != b->width)
      return false;
    if(a->height != b->height)
      return false;
    if(a->depth != b->depth)
      return false;
    if(correctNumChannels(a) != b->nChannels)
      return false;

    return true;
  }

  void ImageViewerWidget::SetImage(const RPData *rp, RP_TYPE type)
  {
    if(rp->GetType(type) != NULL) {
      m_rp.Release();
      m_rp = rp->Clone();
    }

    IplImage *modality = m_rp.GetType(type);

    if(modality != NULL) {

      if(m_temp == NULL || !correctSizeImage(modality, m_temp)) {
        if(m_temp != NULL) 
          cvReleaseImage(&m_temp);
        m_temp = cvCreateImage(cvSize(modality->width, modality->height), modality->depth, correctNumChannels(modality));
      }
      IplImage *im;
      if(modality->nChannels == 4) {
        cvCvtColor(modality, m_temp, CV_BGRA2RGB);
        im = m_temp;
      } else {
        im = modality;
      }

      // Set up importer
      m_importer->SetDataOrigin(0,0,0);
      SetDataSpacing(rp);
      m_importer->SetWholeExtent(0, im->width-1, 0, im->height-1, 0, 0);
      m_importer->SetDataExtentToWholeExtent();
      m_importer->SetDataScalarTypeToUnsignedChar();
      m_importer->SetNumberOfScalarComponents(im->nChannels);
      m_importer->SetImportVoidPointer(im->imageData);
      m_importer->Modified();
      m_importer->Update();

			if(m_flipImage->GetValue()) {
				m_flip->Modified();
				m_flip->Update();
			}
    }

    // Initialize
    if(!m_init) {
			if(m_flipImage->GetValue()) {
				m_flip->SetInputData(m_importer->GetOutput());
				m_flip->Update();
				m_flip->SetFilteredAxis(0);

        m_flipCast->SetOutputScalarTypeToUnsignedChar();
        m_flipCast->SetInputConnection(m_flip->GetOutputPort());
        m_flipCast->Update();
			}
      vtkMatrix4x4 *mat = m_imageActor->GetMatrix();
      if(mat == NULL || arma::norm(arma::eye(4,4)-Matrix44d::FromVtkMatrix4x4(mat).ToArmaMatrix4x4()) < 1e-3) {
        m_imageActor->RotateZ(180);
      }
			if(m_flipImage->GetValue()) {
				m_imageActor->SetInputData(m_flipCast->GetOutput());
			} else {
				m_imageActor->SetInputData(m_importer->GetOutput());
			}
      m_renderer->AddActor(m_imageActor);

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
    if(m_rp.GetType(type) != NULL)
      m_init = true;
    QVTKWidget::update();
  }

  void ImageViewerWidget::SetDataSpacing(const RPData *rp)
  {

    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp->gps.pose).GetOrientation(), rp->gps.pos);
    Vec2d mpp = rp->mpp;
    Vec2d origin = rp->origin;
    Vec2d mppScale(mpp.x/1000.0, mpp.y/1000.0);

    Vec3d first = rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, m_cal, origin, mppScale);
    Vec3d second = rpImageCoordToWorldCoord3(Vec2d(0,1), posePos, m_cal, origin, mppScale);
    f64 spacingy = (second-first).magnitude();

    m_importer->SetDataSpacing(rp->mpp.x/1000.0,spacingy,1);
  }

  
 
  void ImageViewerWidget::ResetView()
  {
    f64 *bounds = m_imageActor->GetBounds();
    m_renderer->ResetCamera(bounds);
  }
  ////////////////////////////////////////////////////////
  // End ImageViwerWidget
  ////////////////////////////////////////////////////////

 
  ////////////////////////////////////////////////////////
  // Begin Image3DImagePlaneWidget
  ////////////////////////////////////////////////////////
  Image3DImagePlaneWidget::Image3DImagePlaneWidget(QWidget *parent)
    : ImageViewerWidget(parent, "Image Plane Viewer")
    , SphereContainer()
    , m_frameBoundaries((CubeVisualizer *)NULL)
  {
    m_useTrackball = true;

    ADD_ACTION_PARAMETER(m_setViewXY, "Set View XY", CALLBACK_POINTER(onSetViewXY, Image3DImagePlaneWidget), this, true); 
    ADD_ACTION_PARAMETER(m_setViewXZ, "Set View XZ", CALLBACK_POINTER(onSetViewXZ, Image3DImagePlaneWidget), this, true); 
    ADD_ACTION_PARAMETER(m_setViewYZ, "Set View YZ", CALLBACK_POINTER(onSetViewYZ, Image3DImagePlaneWidget), this, true); 

    ADD_ACTION_PARAMETER(m_setViewXYGPS2, "Set View XY GPS2", CALLBACK_POINTER(onSetViewXYGPS2, Image3DImagePlaneWidget), this, true); 
    ADD_ACTION_PARAMETER(m_setViewXZGPS2, "Set View XZ GPS2", CALLBACK_POINTER(onSetViewXZGPS2, Image3DImagePlaneWidget), this, true); 
    ADD_ACTION_PARAMETER(m_setViewYZGPS2, "Set View YZ GPS2", CALLBACK_POINTER(onSetViewYZGPS2, Image3DImagePlaneWidget), this, true); 

    ADD_BOOL_PARAMETER(m_showFrameBoundaries, "Show Frame Boundary", CALLBACK_POINTER(onShowExtrasChanged, Image3DImagePlaneWidget), this, false);
    ADD_BOOL_PARAMETER(m_showImage, "Show Image", CALLBACK_POINTER(onShowImageChanged, Image3DImagePlaneWidget), this, true);
  }

  Image3DImagePlaneWidget::~Image3DImagePlaneWidget()
  {
  }

  void Image3DImagePlaneWidget::SetImage(const RPData *rp, RP_TYPE type)
  {
    ImageViewerWidget::SetImage(rp, type);
    if(m_init) {
      Matrix44d tPose = Matrix44d::FromCvMat(m_rp.gps.pose);
      Matrix33d pose = tPose.GetOrientation();

      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_rp.gps.pos);
      Vec2d mpp = m_rp.mpp;
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
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(m_rp.gps2.pose).GetOrientation(), m_rp.gps2.pos);
      if(m_sphereVis == NULL) {
        CreateSphere(m_rp.gps2.pos, 1.0);
        m_renderer->AddActor(m_sphereVis->GetActor());
      } else {
        m_sphereVis->SetCenter(m_rp.gps2.pos);
        m_sphereVis->SetRadius(1.0);
      }
      if(m_axis == NULL) {
        CreateAxis(posePos, 5);
        m_renderer->AddActor(m_axis);
      } else {
        UpdateAxis(posePos);
      }
    }
  }

  void Image3DImagePlaneWidget::onShowExtrasChanged()
  {
    UpdateFrameBoundaries();
    QVTKWidget::update();
    m_renderer->ResetCameraClippingRange();
  }

  void Image3DImagePlaneWidget::onShowImageChanged()
  {
    m_imageActor->SetVisibility(m_showImage->GetValue());
  }


  void Image3DImagePlaneWidget::SetUSVisView(s32 axis1, s32 axis2)
  {
    vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    Matrix44d tPose = Matrix44d::FromCvMat(m_rp.gps.pose);
    Matrix33d pose = tPose.GetOrientation();

    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_rp.gps.pos);
    Vec2d mpp = m_rp.mpp;
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
    m_renderer->GetActiveCamera()->Zoom(1.5);
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

  void Image3DImagePlaneWidget::SetGPS2VisView(s32 axis1, s32 axis2)
  {
    vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    Matrix33d pose = Matrix44d::FromCvMat(m_rp.gps2.pose).GetOrientation();

    Vec3d up = pose.Col(axis1)*-1.0;
    Vec3d focal = pose.Col(axis2)*-1.0;
    camera->SetPosition(0,0,0);
    camera->SetFocalPoint(focal.x, focal.y, focal.z);
    camera->SetViewUp(up.x, up.y, up.z);
    
    m_renderer->SetActiveCamera(camera);
    f64 bounds[] = {m_rp.gps2.pos.x-50, m_rp.gps2.pos.x+50, m_rp.gps2.pos.y-50, m_rp.gps2.pos.y+50, m_rp.gps2.pos.z-50, m_rp.gps2.pos.z+50};
    m_renderer->ResetCamera(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], 
      bounds[5]);
    m_renderer->GetActiveCamera()->Zoom(0.8);
    this->update();
  }

  void Image3DImagePlaneWidget::onSetViewXYGPS2()
  {
    SetGPS2VisView(1,2);
  }

  void Image3DImagePlaneWidget::onSetViewXZGPS2()
  {
    SetGPS2VisView(0,1);
  }

  void Image3DImagePlaneWidget::onSetViewYZGPS2()
  {
    SetGPS2VisView(1,0);
  }
  ////////////////////////////////////////////////////////
  // End Image3DImagePlaneWidget
  ////////////////////////////////////////////////////////
  
  
  ////////////////////////////////////////////////////////
  // Begin ImageViewer2DTeleoperationWidget
  ////////////////////////////////////////////////////////
  ImageViewer2DTeleoperationWidget::ImageViewer2DTeleoperationWidget(QWidget *parent, ControlAlgorithms* control, Teleoperation2DWidget* teleop)
    : ImageViewerWidget(parent) 
    , m_initTeleop(false)
    , m_mask(NULL)
    , m_inCountdownToManualScan(false)
  {
    m_control = control;

    m_teleoperationWidget = teleop;

    m_targetTextActor = vtkSmartPointer<vtkTextActor>::New();
    m_instructionTextActor = vtkSmartPointer<vtkTextActor>::New();
    m_measurementTextActor = vtkSmartPointer<vtkTextActor>::New();
    m_estimateTextActor = vtkSmartPointer<vtkTextActor>::New();
    m_pointPicker = vtkSmartPointer<vtkPointPicker>::New();
    m_maskImporter = vtkSmartPointer<vtkImageImport>::New();
    m_mapTransparency = vtkSmartPointer<vtkImageMapToColors>::New();
    m_maskActor = vtkSmartPointer<vtkImageActor>::New();
    m_lookUpTable = vtkSmartPointer<vtkLookupTable>::New();
 
    ADD_BOOL_PARAMETER(m_showTextOverlay, "Show Text Overlay", CALLBACK_POINTER(onShowTextOverlay, ImageViewer2DTeleoperationWidget), this, true);
  }

  ImageViewer2DTeleoperationWidget::~ImageViewer2DTeleoperationWidget()
  {
    if(m_mask != NULL)
      cvReleaseImage(&m_mask);
  }

  void ImageViewer2DTeleoperationWidget::SetImage(const RPData *rp, RP_TYPE type)
  { 
    ImageViewerWidget::SetImage(rp, type);
    
    if(!m_initTeleop){
      const IplImage *im = rp->b8;
      // Create blank mask image
      m_mask = cvCloneImage(im);
      cvZero(m_mask);

      m_imageActor->RotateY(180);
      f64 *bounds = m_imageActor->GetBounds();
      m_renderer->ResetCamera(bounds);
      
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
      m_maskActor->RotateZ(180);
      m_maskActor->RotateY(180);
      m_maskActor->GetMapper()->SetInputConnection(m_mapTransparency->GetOutputPort());
      m_renderer->AddActor2D(m_maskActor);

      // Add text overlay for selected target coordinates
      m_targetTextActor->GetTextProperty()->SetFontSize ( 24 );
      m_targetTextActor->SetPosition( 10, 10 );
      m_renderer->AddActor2D ( m_targetTextActor );
      m_targetTextActor->SetInput ( "" );
      m_targetTextActor->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Add text overlay for user instructions
      m_instructionTextActor->GetTextProperty()->SetFontSize ( 24 );
      //int* size = m_renderer->GetSize();
      m_instructionTextActor->SetPosition( 10, 400 );
      m_renderer->AddActor2D ( m_instructionTextActor );
      m_instructionTextActor->SetInput ( "" );
      m_instructionTextActor->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Add text overlay for measurement results
      m_measurementTextActor->GetTextProperty()->SetFontSize ( 24 );
      m_measurementTextActor->SetPosition( 10, 200 );
      m_renderer->AddActor2D ( m_measurementTextActor );
      m_measurementTextActor->SetInput ( "" );
      m_measurementTextActor->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Add text overlay for estimate 
      m_estimateTextActor->GetTextProperty()->SetFontSize ( 24 );
      m_estimateTextActor->SetPosition( 200, 200 );
      m_renderer->AddActor2D ( m_estimateTextActor );
      m_estimateTextActor->SetInput ( "" );
      m_estimateTextActor->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

      // Set up interaction 
      vtkSmartPointer<MouseInteractorStylePP> style = 
        vtkSmartPointer<MouseInteractorStylePP>::New();
      style->SetImageViewerWidget(this);
      style->SetTeleoperation2DWidget(m_teleoperationWidget);
      style->SetControl(m_control);
      style->SetRenderWindowInteractor(m_interactor);
      m_interactor->SetInteractorStyle(style);

      // Set up point picker
      m_pointPicker->SetTolerance(0.0);
      m_pointPicker->PickFromListOn();
      m_pointPicker->AddPickList(m_imageActor);
      m_interactor->SetPicker(m_pointPicker);

    }
    m_initTeleop = true;
    
    m_maskImporter->Modified();
    QVTKWidget::update();
  } 

  void ImageViewer2DTeleoperationWidget::onShowTextOverlay(void)
  {
    m_targetTextActor->SetVisibility(m_showTextOverlay->GetValue());    
    m_estimateTextActor->SetVisibility(m_showTextOverlay->GetValue());
    m_measurementTextActor->SetVisibility(m_showTextOverlay->GetValue());
  }

  void ImageViewer2DTeleoperationWidget::SetTeleoperation2DWidget(Teleoperation2DWidget *widget)
  { 
    m_teleoperationWidget = widget;
  }

  void ImageViewer2DTeleoperationWidget::setControl(ControlAlgorithms* control)
  {
    m_control = control;
  }

  void ImageViewer2DTeleoperationWidget::onUpdateOverlay()
  {   
    // zero the mask image
    cvZero(m_mask);

    // get current values for overlaying
    Vec3d p_img, pz_img, py_img;
    Matrix44d x, z;
    Vec3d t_img, t;
    Vec3d Sxyz;
    double mmToNextScan;
    double alpha;
    bool insertionDepthReached;
    m_control->getOverlayValues(x, p_img, pz_img, py_img,
      z, Sxyz, t_img, t, mmToNextScan, insertionDepthReached,alpha);

    // update target
    if( !t.isZero() ){
      SetTargetText(Vec2d(t_img.x,t_img.y), t);      
      if( fabs(t_img.z) < 10.0 ) {
        DrawTargetIcon(t_img);
      }
    }

      // update estimate
      if( !x.isZero() ){
        SetEstimateText(x,Sxyz);
        DrawTipIcon(p_img, pz_img, py_img);
      }

      // update measurement
      if( !z.isZero()  ){
        SetMeasurementText(z);
      }

      // update instructions if scan needed
    if( (!m_inCountdownToManualScan) && (!m_control->inManualScanning()) && m_control->inTaskSpaceControl() ){
      char str [100];
      if( mmToNextScan == 0.0 ){
        int n = sprintf(str, "Needle scan needed.");
      }else{
        int n = sprintf(str, "%.2f mm to next needle scan and alpha = %.2f",mmToNextScan,alpha);
      }   
      SetInstructionText(str);
      f32 danger = 1.0 - mmToNextScan/MAX_OPEN_LOOP_INSERTION;
      m_instructionTextActor->GetTextProperty()->SetColor( 1.0,1.0-danger,1.0-danger );

      if( insertionDepthReached ){
        int n = sprintf(str, "Target depth reached.");
        SetInstructionText(str);
      }
    }
  }

  void ImageViewer2DTeleoperationWidget::SetInCountdownToManualScan(bool input)
  {
    m_inCountdownToManualScan = input;
  }

  void ImageViewer2DTeleoperationWidget::SetInstructionText(char* str)
  {
    m_instructionTextActor->SetInput(str);
    int* size = m_renderer->GetSize();
    m_instructionTextActor->SetPosition( 10, size[1]-40 );
    m_instructionTextActor->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

  }

  void ImageViewer2DTeleoperationWidget::SetTargetText(Vec2d t_img, Vec3d t_wld)
  {
    // Format the click position and print over image
    char str [1000];
    int n = sprintf(str, "t_img = {%.1f, %.1f}\nt_wld = {%.2f, %.2f, %.2f}", t_img.x, t_img.y, t_wld.x, t_wld.y, t_wld.z);
    m_targetTextActor->SetInput(str);    
  }

  void ImageViewer2DTeleoperationWidget::SetEstimateText(Matrix44d x, Vec3d Sxyz)
  {
    // Format the click position and print over image
    Vec3d p = x.GetPosition();
    Matrix33d R = x.GetOrientation();
    char str [1000];
   
#ifdef GPS3_SAVING
    int n = sprintf(str, "x = \n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n\n p_gps = %.2f, %.2f, %.2f", 
      R.m_data[0][0], R.m_data[0][1], R.m_data[0][2], p.x, 
      R.m_data[1][0], R.m_data[1][1], R.m_data[1][2], p.y, 
      R.m_data[2][0], R.m_data[2][1], R.m_data[2][2], p.z,
      Sxyz.x, Sxyz.y, Sxyz.z);
#else
    int n = sprintf(str, "x = \n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n\n Sxyz = %.2f, %.2f, %.2f", 
      R.m_data[0][0], R.m_data[0][1], R.m_data[0][2], p.x, 
      R.m_data[1][0], R.m_data[1][1], R.m_data[1][2], p.y, 
      R.m_data[2][0], R.m_data[2][1], R.m_data[2][2], p.z,
      Sxyz.x, Sxyz.y, Sxyz.z);
#endif

    m_estimateTextActor->SetInput(str);
    int* size = m_renderer->GetSize();
    m_estimateTextActor->SetPosition( 10, int(size[1]/2) );
  }

  void ImageViewer2DTeleoperationWidget::SetMeasurementText(Matrix44d z)
  {
    // Format the click position and print over image
    Vec3d p = z.GetPosition();
    Matrix33d R = z.GetOrientation();
    char str [1000];
    int n = sprintf(str, "z = \n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n", 
      R.m_data[0][0], R.m_data[0][1], R.m_data[0][2], p.x, 
      R.m_data[1][0], R.m_data[1][1], R.m_data[1][2], p.y, 
      R.m_data[2][0], R.m_data[2][1], R.m_data[2][2], p.z);
    m_measurementTextActor->SetInput(str);

    int* size = m_renderer->GetSize();
    m_measurementTextActor->SetPosition( 10, int(size[1]/4) );
    
  }

  void ImageViewer2DTeleoperationWidget::DrawTargetIcon(Vec3d t)
  {
    int r = 40-t.z;

    if( r > -1 ) // If we have a circle to draw
    {
      cvCircle(m_mask,cvPoint(t.x,t.y),r,cvScalar(1.0),5,CV_AA);

      // Update the VTK rendering
      m_maskImporter->Update();
      m_maskImporter->Modified();
      //this->repaint();
    }
  }

  void ImageViewer2DTeleoperationWidget::DrawTipIcon(Vec3d p, Vec3d pz, Vec3d py)
  {

    if( fabs(p.z) < 10.0 ){ // if the tip estimate is within 10 mm of the image plane
      cvLine(m_mask,cvPoint(p.x,p.y),cvPoint(pz.x, pz.y),cvScalar(1.0),3,CV_AA);
      cvLine(m_mask,cvPoint(p.x,p.y),cvPoint(py.x, py.y),cvScalar(1.0),3,CV_AA);
    }
    // Update the VTK rendering
    m_maskImporter->Update();
    m_maskImporter->Modified();
    //this->repaint();
  }

  void ImageViewer2DTeleoperationWidget::getImageDim(int &w, int &h)
  {
    w = m_rp.b8->width;
    h = m_rp.b8->height;
  }

  void ImageViewer2DTeleoperationWidget::SetDataSpacing(const RPData *rp)
  {
    m_importer->SetDataSpacing(1,1,1);
  }


  ////////////////////////////////////////////////////////
  // End ImageViewer2DTeleoperationWidget
  ////////////////////////////////////////////////////////
}
