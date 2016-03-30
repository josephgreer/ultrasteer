#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include "UICore.h"
#include <QVTKWidget.h>
#include "Resizable.h"
#include <vtkChartXY.h>
#include <vtkPlotBar.h>
#include <vtkRenderer.h>
#include <vtkContextView.h>
#include <vtkPlotLine.h>

namespace Nf
{
  struct PFData; 

  class ChartWidget : public ResizableQVTKWidget
  {
  protected:
    vtkSmartPointer < vtkChartXY > m_chart;
    vtkSmartPointer < vtkContextView > m_view;

  public:
    ChartWidget(QWidget *parent, QSize sz);

    virtual void Initialize() = 0;
    virtual void UpdateVisualization(const PFData &p) = 0;
  };

  class BarChartWidget : public ChartWidget
  {
  protected:
    vtkSmartPointer < vtkPlotBar > m_doppler;
    vtkSmartPointer < vtkPlotBar > m_probOverNeedle;
    vtkSmartPointer < vtkPlotBar > m_probOverNeedleDoppler;

  public:
    BarChartWidget(QWidget *parent, QSize sz);
    virtual void UpdateVisualization(const PFData &p);
    virtual void Initialize();
  };

  class LineChartWidget : public ChartWidget
  {
  protected:
    vtkSmartPointer < vtkPlotLine > m_doppler;

  public:
    LineChartWidget(QWidget *parent, QSize sz);
    virtual void UpdateVisualization(const PFData &p);
    virtual void Initialize();
  };

}

#endif // CHARTWIDGET_H
