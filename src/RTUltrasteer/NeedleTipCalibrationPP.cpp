#include <vtkCell.h>
#include <vtkGenericCell.h>
#include <vtkCellData.h>
#include "NeedleTipCalibrationPP.h"

namespace Nf
{  
  
    NeedleTipCalibrationPP::NeedleTipCalibrationPP()
      : m_pointData(vtkPointData::New())
      , m_image(NULL)
    {
    }

    void NeedleTipCalibrationPP::OnLeftButtonDown() 
    {
      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0], 
                         this->Interactor->GetEventPosition()[1], 
                         0,  // always zero.
                         this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      f64 pos[3];
      this->Interactor->GetPicker()->GetPickPosition(pos);

      // Find the cell that contains pos
      f64 *spacing = m_image->GetSpacing();
      f64 pt[3]; m_image->GetPoint(((vtkPointPicker *)this->Interactor->GetPicker())->GetPointId(), pt);
      pt[0] /= spacing[0];
      pt[1] /= spacing[1];

      m_estimatorWidget->onPointPushed(Vec2d(pt[0], pt[1]));

      // Forward events
      vtkInteractorStyleImage::OnLeftButtonDown();
    }
}