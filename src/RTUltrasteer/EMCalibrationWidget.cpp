#include "EMCalibrationWidget.h"


namespace Nf
{
  EMCalibrationWidget::EMCalibrationWidget(QWidget *parent) 
    : Nf::ParameterCollection("EM Calibration")
    , ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
  {
    m_mapper = vtkSmartPointer<vtkImageMapper>::New();
    m_importer = vtkSmartPointer<vtkImageImport>::New();
    m_imageActor = vtkSmartPointer<vtkImageActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();

    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();        
  }

  EMCalibrationWidget::~EMCalibrationWidget()
  {
  }

  void EMCalibrationWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void EMCalibrationWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQVTKWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > EMCalibrationWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }

}