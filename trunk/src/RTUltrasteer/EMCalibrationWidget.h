#ifndef EMCALIBRATIONWIDGET_H
#define EMCALIBRATIONWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QElapsedTimer>
#include <QPushButton>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "RPFileReader.h"
#include "RPProcess.h"
#include "ImageViewerWidget.h"
#include <QtDebug>
#include "NeedleSteeringRobot.h"
#include "ControlAlgorithms.h"


namespace Nf
{
  class EMCalibrationWidget : public ResizableQVTKWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    RPData m_data;
    QGridLayout *m_layout;
    vtkSmartPointer < vtkImageImport > m_importer;
    vtkSmartPointer < vtkImageMapper > m_mapper;
    vtkSmartPointer < vtkImageActor > m_imageActor;
    vtkSmartPointer < vtkRenderer > m_renderer;
    vtkSmartPointer < vtkImageFlip > m_flip;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;

  public:
    EMCalibrationWidget(QWidget *parent);
    virtual ~EMCalibrationWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    std::vector < QVTKWidget * > GetChildWidgets();

  //public slots:
  //  void onStartManualNeedleScan();
  };

}

 

#endif //EMCALIBRATIONWIDGET_H