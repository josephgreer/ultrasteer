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

namespace Nf
{
  ImageViewerWidget::ImageViewerWidget(QWidget *parent)
    : ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , Nf::ParameterCollection("Image Viewer Widget")
    , m_im(NULL)
    , m_mask(NULL)
    , m_init(false)
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
    if(m_im != NULL)
      cvReleaseImage(&m_im);
  }

  void ImageViewerWidget::SetImage(const IplImage *im)
  {
    if(m_im != NULL)
      cvReleaseImage(&m_im);

    if(im) {
      m_im = cvCloneImage(im);

      // Set up importer
      m_importer->SetDataOrigin(0,0,0);
      m_importer->SetDataSpacing(1,1,1);
      m_importer->SetWholeExtent(0, m_im->width-1, 0, m_im->height-1, 0, 0);
      m_importer->SetDataExtentToWholeExtent();
      m_importer->SetDataScalarTypeToUnsignedChar();
      m_importer->SetNumberOfScalarComponents(m_im->nChannels);
      m_importer->SetImportVoidPointer(m_im->imageData);
      m_importer->Modified();
      m_importer->Update();

      m_flip->Update();
    }

    // Initialize
    if(!m_init) {
      m_flip->SetFilteredAxes(1);
      m_flip->SetInputData((vtkDataObject *)m_importer->GetOutput());
      m_flip->Update();
      m_imageActor->SetInputData(m_flip->GetOutput());
      m_renderer->AddActor2D(m_imageActor);

      this->GetRenderWindow()->AddRenderer(m_renderer);

      // Set up style      
      vtkSmartPointer<vtkInteractorStyleImage> style = 
        vtkSmartPointer<vtkInteractorStyleImage>::New();
      m_interactor->SetInteractorStyle(style);

      // Render and start interaction
      m_interactor->SetRenderWindow(this->GetRenderWindow());
      m_interactor->Initialize();
      f64 *bounds = m_imageActor->GetBounds();
      m_renderer->ResetCamera(bounds);
      vtkCamera *cam = m_renderer->GetActiveCamera();
      m_renderer->SetActiveCamera(cam);
    } 
    if(im)
      m_init = true;
  }

  ImageViewer2DTeleoperationWidget::ImageViewer2DTeleoperationWidget(QWidget *parent)
    : ImageViewerWidget(parent) 
    , m_initTeleop(false)
  {
    m_textActor = vtkSmartPointer<vtkTextActor>::New();
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

  void ImageViewer2DTeleoperationWidget::SetImage(const IplImage *im)
  {   
    ImageViewerWidget::SetImage(im);
    
    if(!m_initTeleop){
      // Create blank mask image
      m_mask = cvCloneImage(im);
      cvZero(m_mask);

      // Set up mask importer
      m_maskImporter->SetDataOrigin(0,0,0);
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
      m_lookUpTable->SetTableValue( 1.0, 1.0, 0.0, 0.0, 1.0 ); //label 1 is opaque and red
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

      // Add text overlay
      m_textActor->GetTextProperty()->SetFontSize ( 24 );
      m_textActor->SetPosition2 ( 10, 40 );
      m_renderer->AddActor2D ( m_textActor );
      m_textActor->SetInput ( "Initialized" );
      m_textActor->GetTextProperty()->SetColor ( 1.0,0.0,0.0 );

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

  void ImageViewer2DTeleoperationWidget::SetTextOverlay(Vec2d px, Vec3d wpt)
  {

    // Format the click position and print over image
    char str [100];
    int n = sprintf(str, "pix = {%.1f, %.1f}\nworld = {%.2f, %.2f, %.2f}", px.x, px.y, wpt.x, wpt.y, wpt.z);
    m_textActor->SetInput(str);

    // Update the VTK rendering
    this->repaint();

  }

  void ImageViewer2DTeleoperationWidget::SetTargetOverlay(int r, Vec2d px)
  {
    // Zero the image and draw a circle at the click point
    cvZero(m_mask);

    if( r > -1 ) // If we have a circle to draw
    {
      cvCircle(m_mask,cvPoint(px.x,m_mask->height-1-px.y),r,cvScalar(1.0),5,CV_AA);

      // Update the VTK rendering
      m_maskImporter->Update();
      m_maskImporter->Modified();
      this->repaint();
    }

  }

  void ImageViewer2DTeleoperationWidget::getImageDim(int &w, int &h)
  {
    w = m_im->width;
    h = m_im->height;
  }


}
