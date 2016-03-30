#include "ChartWidget.h"
#include "EstimatorWidget.h"
#include "RTUtil.h"
#include <vtkContextScene.h>
#include <vtkAxis.h>
#include <vtkBrush.h>
#include <vtkPen.h>
#include <vtkTable.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkStringArray.h>

namespace Nf
{  
  ////////////////////////////////////////////////////////
  // Begin ChartWidget
  ////////////////////////////////////////////////////////
  ChartWidget::ChartWidget(QWidget *parent, QSize sz)
    : ResizableQVTKWidget(parent, sz)
  {
    m_view = vtkSmartPointer < vtkContextView >::New();
    m_view->SetRenderWindow(this->GetRenderWindow());
    m_view->GetRenderer()->SetBackground(0,0,0);
    
    m_chart = vtkSmartPointer < vtkChartXY >::New();

    //m_chart->GetBackgroundBrush()->SetColorF(0.2, 0.2, 0.2, 1);

    m_view->GetScene()->AddItem(m_chart);
  }
  ////////////////////////////////////////////////////////
  // END ChartWidget
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  // Begin BarChartWidget
  ////////////////////////////////////////////////////////
  BarChartWidget::BarChartWidget(QWidget *parent, QSize sz)
    : ChartWidget(parent, sz)
  {
    m_doppler = vtkSmartPointer < vtkPlotBar >::New();
    m_probOverNeedle = vtkSmartPointer < vtkPlotBar >::New();
    m_probOverNeedleDoppler = vtkSmartPointer < vtkPlotBar >::New();
  }

  void BarChartWidget::Initialize()
  {
    vtkSmartPointer < vtkPlotBar > barPlots[3] = {m_doppler, m_probOverNeedle, m_probOverNeedleDoppler};
    const char *names[6] = {"dopplerX", "dopplerY", "probOverNeedleX", "probOverNeedleY", "probOverNeedleDopplerX", "probOverNeedleDopplerY"};
    s32 numPlots = sizeof(barPlots)/sizeof(barPlots[0]);

    vtkSmartPointer < vtkTable > table = vtkSmartPointer < vtkTable >::New();
    vtkSmartPointer < vtkIntArray > intArray;
    vtkSmartPointer < vtkFloatArray > floatArray;
    for(s32 i=0; i<numPlots; i++) {
      intArray = vtkSmartPointer < vtkIntArray >::New();
      floatArray = vtkSmartPointer < vtkFloatArray >::New();
      intArray->SetName(names[2*i+0]);
      floatArray->SetName(names[2*i+1]);
      table->AddColumn(intArray);
      table->AddColumn(floatArray);
    }

    table->SetNumberOfRows(1);
    for(s32 i=0; i<numPlots; i++) {
      table->SetValue(0, 2*i+0, i+1);
      table->SetValue(0, 2*i+1, 2*i+1);
    }

    f64 colors[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };

    for(s32 i=0; i<numPlots; i++) {
      m_chart->AddPlot(barPlots[i]);
      barPlots[i]->SetInputData(table,2*i+0,2*i+1);
      barPlots[i]->SetColor(colors[i][0], colors[i][1], colors[i][2]);
    }

    m_chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("");
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetTicksVisible(0);
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetGridVisible(0);
    m_chart->GetAxis(vtkAxis::BOTTOM)->GetPen()->SetColor(200, 200, 200);
    m_chart->GetAxis(vtkAxis::BOTTOM)->GetPen()->SetWidth(2);

    m_chart->GetAxis(vtkAxis::LEFT)->SetTicksVisible(0);
    m_chart->GetAxis(vtkAxis::LEFT)->SetTitle("");
    m_chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
    m_chart->GetAxis(vtkAxis::LEFT)->SetGridVisible(0);
    m_chart->GetAxis(vtkAxis::LEFT)->GetPen()->SetColor(200, 200, 200);
    m_chart->GetAxis(vtkAxis::LEFT)->GetPen()->SetWidth(2);

    m_chart->GetAxis(vtkAxis::BOTTOM)->SetRange(0.8, 3.2);
    m_chart->GetAxis(vtkAxis::LEFT)->SetRange(0,1);

    m_chart->SetBarWidthFraction(1000.0);
  }

  void BarChartWidget::UpdateVisualization(const PFData &p)
  {
    m_doppler->GetInput()->SetValue(0, 1, MIN(p.m.doppler(0,0)/500.0,1));
    m_doppler->SetWidth(20);
    m_chart->SetBarWidthFraction(100);

    m_probOverNeedleDoppler->GetInput()->SetValue(0, 3, p.overNeedle.x);
    m_probOverNeedle->GetInput()->SetValue(0,5, p.overNeedle.y);

    m_doppler->Update();
    m_doppler->Modified();
    m_probOverNeedle->Update();
    m_probOverNeedle->Modified();
    m_probOverNeedleDoppler->Update();
    m_probOverNeedleDoppler->Modified();
    m_chart->Modified();
    QVTKWidget::update();
    this->repaint();
  }
  ////////////////////////////////////////////////////////
  // END BarChartWidget
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  // Begin LineChartWidget
  ////////////////////////////////////////////////////////
  LineChartWidget::LineChartWidget(QWidget *parent, QSize sz)
    : ChartWidget(parent, sz)
  {
    m_doppler = vtkSmartPointer < vtkPlotLine >::New();
  }

  void LineChartWidget::Initialize()
  {
    vtkSmartPointer < vtkTable > table = vtkSmartPointer < vtkTable >::New();
    vtkSmartPointer < vtkFloatArray > x;
    vtkSmartPointer < vtkFloatArray > y;
    x = vtkSmartPointer < vtkFloatArray >::New();
    y = vtkSmartPointer < vtkFloatArray >::New();
    x->SetName("X");
    y->SetName("Y");

    table->AddColumn(x);
    table->AddColumn(y);
    table->SetNumberOfRows(0);

    m_doppler->SetInputData(table, 0, 1);
    m_doppler->SetColor(1,0,0);
    m_doppler->SetWidth(3);
    
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetRange(0, 600);
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("");
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetTicksVisible(0);
    m_chart->GetAxis(vtkAxis::BOTTOM)->SetGridVisible(0);
    m_chart->GetAxis(vtkAxis::BOTTOM)->GetPen()->SetColor(200, 200, 200);
    m_chart->GetAxis(vtkAxis::BOTTOM)->GetPen()->SetWidth(2);
    
    m_chart->GetAxis(vtkAxis::LEFT)->SetRange(0, 7);
    m_chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
    m_chart->GetAxis(vtkAxis::LEFT)->SetTicksVisible(0);
    m_chart->GetAxis(vtkAxis::LEFT)->SetTitle("");
    m_chart->GetAxis(vtkAxis::LEFT)->SetGridVisible(0);
    m_chart->GetAxis(vtkAxis::LEFT)->GetPen()->SetColor(200, 200, 200);
    m_chart->GetAxis(vtkAxis::LEFT)->GetPen()->SetWidth(2);

    m_chart->AddPlot(m_doppler);
  }

  void LineChartWidget::UpdateVisualization(const PFData &p)
  {
#if 1
    m_doppler->GetInput()->InsertNextBlankRow();
    s32 nRows = m_doppler->GetInput()->GetNumberOfRows();
    m_doppler->GetInput()->SetValue(nRows-1, 0, nRows);
    m_doppler->GetInput()->SetValue(nRows-1, 1, std::log(MAX(p.m.doppler(0,0), 1)));
    m_doppler->Update();
    m_doppler->Modified();
    m_chart->Update();
    m_chart->Modified();
    QVTKWidget::update();
    this->repaint();
#endif
    //m_chart->GetInput()->GetFieldData()->GetArray(0)->SetTuple1(0, MIN(p.m.doppler(0,0)/500.0,1));
  }
  ////////////////////////////////////////////////////////
  // END LineChartWidget
  ////////////////////////////////////////////////////////
}
