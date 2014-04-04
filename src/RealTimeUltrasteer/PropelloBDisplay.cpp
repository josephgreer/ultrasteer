#include "stdafx.h"
#include "PropelloBDisplay.h"

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
        ret = portaGetColorImage(m_index, buffer);
		ret = portaGetColorImageOnly (m_index, colorbuffer);
    }
    else
    {
        ret = portaGetBwImage(m_index, buffer, true);
    }

    if (ret)
    {
        scene()->invalidate();
    }

    return true;
}
