#include "ImageViewerWidget.h"
#include <vtkCamera.h>

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
    m_imageActor = vtkSmartPointer<vtkActor2D>::New();
    m_imAct = vtkSmartPointer<vtkImageActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

#if 0
    m_importer->SetDataOrigin(0,0,0);
    m_importer->SetWholeExtent(0, 0, 0, 0, 0, 0);
    m_importer->SetDataExtentToWholeExtent();
    m_importer->SetDataScalarTypeToUnsignedChar();
    m_importer->SetNumberOfScalarComponents(0);
    m_importer->SetImportVoidPointer(NULL);
    m_importer->Update();
    m_importer->Modified();
#endif
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

    m_importer->SetDataOrigin(0,0,0);
    m_importer->SetDataSpacing(1,1,1);
    m_importer->SetWholeExtent(0, m_im->width-1, 0, m_im->height-1, 0, 0);
    m_importer->SetDataExtentToWholeExtent();
    m_importer->SetDataScalarTypeToUnsignedChar();
    m_importer->SetNumberOfScalarComponents(m_im->nChannels);
    m_importer->SetImportVoidPointer(m_im->imageData);
    m_importer->Update();
    m_importer->Modified();

    if(!m_init) {
#if 0
      m_mapper->SetInputData(m_importer->GetOutput(0));
      m_mapper->SetColorWindow(255);
      m_mapper->SetColorLevel(127.5);

      m_imageActor->SetMapper(m_mapper);
      m_imageActor->SetPosition(0,0);
      m_renderer->AddActor2D(m_imageActor);
#else
      m_imAct->SetInputData(m_importer->GetOutput());
      m_renderer->AddActor2D(m_imAct);
#endif

      this->GetRenderWindow()->AddRenderer(m_renderer);

      vtkSmartPointer<vtkInteractorStyleImage> style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();

      m_interactor->SetInteractorStyle(style);

      // Render and start interaction
      m_interactor->SetRenderWindow(this->GetRenderWindow());
      m_interactor->Initialize();
      m_renderer->ResetCamera();
      vtkCamera *cam = m_renderer->GetActiveCamera();
      cam->Zoom(2.0);
      m_renderer->SetActiveCamera(cam);
    } 

    m_init = true;
    this->repaint();
  }
}