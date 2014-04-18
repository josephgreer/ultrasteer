#include "stdafx.h"
#include "PropelloBDisplay.h"
#include "SegmentCore.h"

using namespace Nf;

PropelloBDisplay::PropelloBDisplay(QWidget* parent) : PropelloDisplay(parent)
{
}

PropelloBDisplay::~PropelloBDisplay()
{
}

bool PropelloBDisplay::loadColorMap(const QString file)
{
    QImage img;
    if (!img.load(file))
    {
        return false;
    }

    return portaImportColorMap(0, (unsigned int*)img.constBits());
}

bool PropelloBDisplay::setImgData()
{
  bool ret = false;

  if (!m_colorimage || !m_image || m_index < 0)
  {
    return false;
  }

  unsigned char* buffer = m_image->bits();
  unsigned char* colorbuffer = m_colorimage->bits();
  int mode = portaGetCurrentMode();

  if (mode == ColourMode || mode == DualColourMode || mode == TriplexMode)
  {
    ret = portaGetBwImage(m_index, buffer, false);
    ret = portaGetColorImage(m_index, colorbuffer);
	tick(portaGetDisplayFrameCount(m_index));
  }
  else
  {
    ret = portaGetBwImage(m_index, buffer, false);
  }

  if (ret)
  {
    scene()->invalidate();
  }

  return true;
}
