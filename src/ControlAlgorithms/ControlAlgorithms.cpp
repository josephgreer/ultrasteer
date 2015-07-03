#include "ControlAlgorithms.h"
#include "math.h"
#include <time.h>

namespace Nf {

  ControlAlgorithms::ControlAlgorithms()
  {
    m_targetDefined = false;
  }

  ControlAlgorithms::~ControlAlgorithms()
  {
  }

  void ControlAlgorithms::SetTarget(Vec3d target)
  {
    m_target = target;
    m_targetDefined = true;
  }

  void ControlAlgorithms::GetTarget(Vec3d &target)
  {
    target = m_target;
  } 

  bool ControlAlgorithms::isTargetDefined()
  {
    return m_targetDefined;
  } 

  void ControlAlgorithms::addManualScanFrame(RPData data)
  {
    m_segmentation.addManualScanFrame(data);
  }

  void ControlAlgorithms::resetManualScan()
  {
    m_segmentation.resetManualScan();
  }

  Matrix44d ControlAlgorithms::processManualScan()
  {
    return m_segmentation.processManualScan();
  }

};