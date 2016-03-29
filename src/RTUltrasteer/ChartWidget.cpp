#include "ChartWidget.h"
#include <vtkContextScene.h>
#include <vtkAxis.h>

namespace Nf
{
  ////////////////////////////////////////////////////////
  // Begin PlotData
  ////////////////////////////////////////////////////////
  void PlotData::Initialize()
  {
    doppler = vtkSmartPointer < vtkPlotBar >::New();
    //doppler->GetYAxis()->SetMinimumLimit(0);
    //doppler->GetYAxis()->SetMinimumLimit(1);

#if 0
    //barChart
    barChart = vtkSmartPointer < vtkBarChartActor >::New();
    barChart->SetPosition(0.1,0.5);
    barChart->SetWidth(0.5);
    barChart->SetHeight(1.0);
    barChart->TitleVisibilityOff();

    vtkSmartPointer < vtkFloatArray  > bitter = vtkFloatArray::New();
    vtkSmartPointer < vtkDataObject > dobj = vtkDataObject::New();
    dobj->GetFieldData()->AddArray(bitter);

    bitter->SetNumberOfTuples(5);
    bitter->SetTuple1(0, 0);
    bitter->SetTuple1(1, 0);
    bitter->SetTuple1(2, 0);
    bitter->SetTuple1(3, 1);
    bitter->SetTuple1(4, 0);
    barChart->SetBarColor(0, 1, 0.65, 0);
    barChart->SetBarColor(1, 1, 0.65, 0);
    barChart->SetBarColor(2, 0, 1, 0);
    barChart->SetBarColor(3, 0, 0, 0);
    barChart->SetBarColor(4, 0, 0, 0);

    barChart->SetBarLabel(0,"");
    barChart->SetYTitle("");
    barChart->SetInput(dobj);
    barChart->GetLabelTextProperty()->SetOpacity(0);

    bitter = vtkFloatArray::New();
    dobj = vtkDataObject::New();
    dobj->GetFieldData()->AddArray(bitter);
#endif
  }

  void PlotData::AddActors(vtkSmartPointer < vtkRenderer > renderer)
  {
#if 0
    renderer->AddActor(barChart);
#endif
  }

  void PlotData::UpdateVisualization(const PFData &p)
  {
#if 0
    //raw Doppler
    NTrace("Doppler = %f, overNeedle = %f\n", p.m.doppler(0,0)/500.0, p.overNeedle.x);
    barChart->GetInput()->GetFieldData()->GetArray(0)->SetTuple1(0, MIN(p.m.doppler(0,0)/500.0,1));
    barChart->GetInput()->GetFieldData()->GetArray(0)->SetTuple1(1, p.overNeedle.x);
    barChart->GetInput()->GetFieldData()->GetArray(0)->SetTuple1(2, p.overNeedle.y);
    barChart->Modified();
#endif
  }
  ////////////////////////////////////////////////////////
  // END PlotData
  ////////////////////////////////////////////////////////

  ChartWidget::ChartWidget(QWidget *parent, QSize sz)
    : ResizableQVTKWidget(parent, sz)
  {
    m_view = vtkSmartPointer < vtkContextView >::New();
    m_view->SetRenderWindow(this->GetRenderWindow());

    m_plot.Initialize();
    
    m_chart = vtkSmartPointer < vtkChartXY >::New();
    m_chart->AddPlot(m_plot.doppler);

    m_view->GetScene()->AddItem(m_chart);
  }
}
