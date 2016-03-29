#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include "UICore.h"
#include <QVTKWidget.h>
#include "Resizable.h"
#include <vtkChartXY.h>
#include <vtkPlotBar.h>
#include <vtkRenderer.h>
#include <vtkContextView.h>

namespace Nf
{
  struct PFData; 

  struct PlotData
  {
    vtkSmartPointer < vtkPlotBar > doppler;
    void Initialize();
    void AddActors(vtkSmartPointer < vtkRenderer > renderer);
    void UpdateVisualization(const PFData &p); 
  };

  class ChartWidget : public ResizableQVTKWidget
  {
  protected:
    vtkSmartPointer < vtkChartXY > m_chart;
    vtkSmartPointer < vtkContextView > m_view;
    PlotData m_plot;

  public:
    ChartWidget(QWidget *parent, QSize sz);
  };

}

#endif // CHARTWIDGET_H
