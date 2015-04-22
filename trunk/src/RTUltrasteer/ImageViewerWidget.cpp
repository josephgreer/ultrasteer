#include "ImageViewerWidget.h"
#include "MouseInteractorStylePP.h"
#include <vtkCamera.h>
#include <vtkDataObject.h>

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

namespace Nf
{
   ImageViewerWidget::ImageViewerWidget(QWidget *parent)
    : ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , Nf::ParameterCollection("Image Viewer Widget")
    , m_im(NULL)
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

    m_im = cvCloneImage(im);

    // Set up importer
    m_importer->SetDataOrigin(0,0,0);
    m_importer->SetDataSpacing(1,1,1);
    m_importer->SetWholeExtent(0, m_im->width-1, 0, m_im->height-1, 0, 0);
    m_importer->SetDataExtentToWholeExtent();
    m_importer->SetDataScalarTypeToUnsignedChar();
    m_importer->SetNumberOfScalarComponents(m_im->nChannels);
    m_importer->SetImportVoidPointer(m_im->imageData);
    m_importer->Update();
    m_importer->Modified();

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
    m_init = true;
  }

  ImageViewer2DTeleoperationWidget::ImageViewer2DTeleoperationWidget(QWidget *parent)
    : ImageViewerWidget(parent) 
    , m_initTeleop(false)
  {
    m_textActor = vtkSmartPointer<vtkTextActor>::New();
    m_pointPicker = vtkSmartPointer<vtkPointPicker>::New();

    m_x = 0;
    m_y = 0;
  }

  ImageViewer2DTeleoperationWidget::~ImageViewer2DTeleoperationWidget()
  {
  }

  void ImageViewer2DTeleoperationWidget::SetImage(const IplImage *im)
  {   
    ImageViewerWidget::SetImage(im);

    if(!m_initTeleop){
      // Set up point picker
      m_pointPicker->SetTolerance(0.0);

      // Add text overlay
      m_textActor->GetTextProperty()->SetFontSize ( 24 );
      m_textActor->SetPosition2 ( 10, 40 );
      m_renderer->AddActor2D ( m_textActor );
      m_textActor->SetInput ( "Initialized" );
      m_textActor->GetTextProperty()->SetColor ( 1.0,0.0,0.0 );

      // Set up style      
      vtkSmartPointer<MouseInteractorStylePP> style = 
        vtkSmartPointer<MouseInteractorStylePP>::New();
      style->SetImageViewerWidget(this);
      style->SetImageActor(m_imageActor);
      m_interactor->SetInteractorStyle(style);
    }
    m_initTeleop = true;
  } 

  void ImageViewer2DTeleoperationWidget::SetXY(double x, double y)
  {
    // Save the positions
    m_x = x;
    m_y = y;

    // Format the click position and print over image
    char str [50];
    int n = sprintf(str, "x = %.3f; y = %.3f", m_x, m_y);
    m_textActor->SetInput(str);

    m_flip->Modified();
    m_flip->Update();

    this->repaint();
  }

}