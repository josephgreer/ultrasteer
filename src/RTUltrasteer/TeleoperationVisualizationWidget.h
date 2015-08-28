#ifndef TELEOPERATIONVISUALIZATIONWIDGET_H
#define TELEOPERATIONVISUALIZATIONWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QElapsedTimer>
#include <QPushButton>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "RPUlterius.h"
#include "ImageViewerWidget.h"
#include <QtDebug>
#include "NeedleSteeringRobot.h"
#include "ControlAlgorithms.h"
#include "vtkVertexGlyphFilter.h"
#include <vtkProperty.h>
#include <QtGui/QFileDialog>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkSTLReader.h>
#include <vtkPoints.h>
#include <vtkPolyLine.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

namespace Nf
{
  class TeleoperationVisualizationWidget : public ResizableQVTKWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    
    QGridLayout *m_layout;

    //setViewXY
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXY;
    void onSetViewXY();
    CLASS_CALLBACK(onSetViewXY, TeleoperationVisualizationWidget);

    //setViewYZ
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewYZ;
    void onSetViewYZ();
    CLASS_CALLBACK(onSetViewYZ, TeleoperationVisualizationWidget);

    //setViewXZ
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXZ;
    void onSetViewXZ();
    CLASS_CALLBACK(onSetViewXZ, TeleoperationVisualizationWidget);
    
    bool m_viewportInit;

    //Pointer to control object
    ControlAlgorithms* m_control;
    
    vtkSmartPointer < vtkSTLReader > m_robotSTLReader;
    vtkSmartPointer < vtkPolyDataMapper > m_robotSTLMapper;
    vtkSmartPointer < vtkActor > m_robotSTLActor;    
    vtkSmartPointer < vtkSTLReader > m_transducerSTLReader;
    vtkSmartPointer < vtkPolyDataMapper > m_transducerSTLMapper;
    vtkSmartPointer < vtkActor > m_transducerSTLActor;
    vtkSmartPointer < vtkSTLReader > m_estimateSTLReader;
    vtkSmartPointer < vtkPolyDataMapper > m_estimateSTLMapper;
    vtkSmartPointer < vtkActor > m_estimateSTLActor;
    vtkSmartPointer < vtkSTLReader > m_measurementSTLReader;
    vtkSmartPointer < vtkPolyDataMapper > m_measurementSTLMapper;
    vtkSmartPointer < vtkActor > m_measurementSTLActor;
    vtkSmartPointer < vtkSphereSource > m_targetSource;
    vtkSmartPointer < vtkPolyDataMapper > m_targetMapper;
    vtkSmartPointer < vtkActor > m_targetActor;
    CubeVisualizer *m_frameBoundaries;
    vtkSmartPointer < vtkAxesActor > m_robotAxes;
    vtkSmartPointer < vtkAxesActor > m_referenceAxes;
    vtkSmartPointer < vtkAxesActor > m_transducerAxes;
    vtkSmartPointer < vtkAxesActor > m_emAxes;
    vtkSmartPointer < vtkAxesActor > m_estimateAxes;
    vtkSmartPointer < vtkAxesActor > m_measurementAxes;    
    vtkSmartPointer < vtkRenderer > m_renderer;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;

  public:
    TeleoperationVisualizationWidget(QWidget *parent, ControlAlgorithms* control);
    virtual ~TeleoperationVisualizationWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    std::vector < QVTKWidget * > GetChildWidgets();
    void onUpdateVisualization();

  private:
    void initViewport();
    void SetVisView(s32 axis1, s32 axis2);
  };

}

 

#endif //TELEOPERATIONVISUALIZATIONWIDGET_H