#include "SegmentationAlgorithms.h"

namespace Nf {

  InPlaneSegmentation::InPlaneSegmentation()
  {
  }

  InPlaneSegmentation::~InPlaneSegmentation()
  {
  }

  void InPlaneSegmentation::addManualScanFrame(RPData data)
  {
    m_scanFrames.push_back(data.Clone());
  }

  void InPlaneSegmentation::resetManualScan()
  {
    RPData frame;

    for( int i = m_scanFrames.size(); i>0 ; i-- )
    {
        frame = m_scanFrames[i-1];
        frame.Release();
        m_scanFrames.pop_back();
    }
  }

  Matrix44d InPlaneSegmentation::processManualScan()
  {
    if( m_scanFrames.size() < 1 )
      return Matrix44d::I();

    for( int i = 0; i < m_scanFrames.size(); i++ )
    {
      RPData frame = m_scanFrames[i];
      processFrame(frame);
    }
  }

  void InPlaneSegmentation::processFrame(RPData)
  {
    int i = 1;
  }
  
};