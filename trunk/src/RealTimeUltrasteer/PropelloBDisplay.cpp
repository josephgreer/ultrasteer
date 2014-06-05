#include "stdafx.h"
#include "PropelloBDisplay.h"
#include "SegmentCore.h"

using namespace Nf;

static void generateRandomFrame(QImage *color, QImage *bmode, s32 frameNum)
{
	s32 colors[4][3] = {{0xFF,0x00,0x00}, {0x00,0xFF,0x00}, {0x00,0x00,0xFF}, {0xFF,0xFF,0x00}};
	s32 idx = (frameNum&0x3);

	u8 grey = (colors[idx][0]+colors[idx][1]+colors[idx][2])/3;

	u8 *pcolor = color->scanLine(0);
	u8 *pgrey = bmode->scanLine(0);

	for(s32 r=0; r<color->height(); r++) {
		u8 *pc = pcolor + r*color->bytesPerLine();
		u8 *pg = pgrey + r*bmode->bytesPerLine();
		for(s32 c=0; c<color->width(); c++) {
			pc[c*4+0] = colors[idx][0];
			pc[c*4+1] = colors[idx][1];
			pc[c*4+2] = colors[idx][2];
			pc[c*4+3] = 0xFF;
		}
		for(s32 c=0; c<bmode->width(); c++) {
			pg[c] = grey;
		}
	}
}

PropelloBDisplay::PropelloBDisplay(QWidget* parent) 
	: PropelloDisplay(parent)
	, m_frameCount(0)
{
#ifdef MEM_TEST
	s32 w = 480;
	s32 h = 460;
	m_image = new QImage(w, h, QImage::Format_Indexed8);
	//set up grayscale indexing to identity map
	for(s32 i=0; i<256; i++)
		m_image->setColor(i, qRgb(i,i,i));
	m_colorimage = new QImage(w, h, QImage::Format_RGB32);

	m_index = 0;

	m_timerStarted = false;
	connect(&m_memTestTimer, SIGNAL(timeout()), this, SLOT(onMemTestTimer()));
	QTimer::singleShot(1000,this,SLOT(onMemTestTimer()));
#endif
}

PropelloBDisplay::~PropelloBDisplay()
{
}

#ifdef MEM_TEST
void PropelloBDisplay::onMemTestTimer()
{
	if(!m_timerStarted) {
		setSceneRect(0, 0, 480, 460);
		m_memTestTimer.setInterval(50);
		m_memTestTimer.start();
		m_timerStarted = true;
	}
	this->setImgData(false);
}
#endif

bool PropelloBDisplay::loadColorMap(const QString file)
{
	QImage img;
	if (!img.load(file))
	{
		return false;
	}

	return portaImportColorMap(0, (unsigned int*)img.constBits());
}

void PropelloBDisplay::resetFrameCount()
{
	m_frameCount = 0;
}

bool PropelloBDisplay::setImgData(bool usePorta)
{
	bool ret = false;

	if (!m_colorimage || !m_image || m_index < 0)
	{
		return false;
	}
	
	if(m_mutex && !m_mutex->tryLock())
		return false;

	//usePorta == true is normal processing pipeline
	//where we get a frame from porta
	//usePorta == false is test processing pipeline
	//where we are calling this function from a timer.
	if(usePorta) {
		unsigned char* buffer = m_image->bits();
		unsigned char* colorbuffer = m_colorimage->bits();
		int mode = portaGetCurrentMode();
		if (mode == ColourMode || mode == DualColourMode || mode == TriplexMode)
		{
			ret = portaGetBwImage(m_index, buffer, false);
			ret = portaGetColorImage(m_index, colorbuffer);
			if(m_mutex) m_mutex->unlock();
			tick(m_frameCount++);
		}
		else
		{
			ret = portaGetBwImage(m_index, buffer, false);
			if(m_mutex) m_mutex->unlock();
		} 
	} else {
		generateRandomFrame(m_colorimage, m_image, m_frameCount);
		if(m_mutex) m_mutex->unlock();
		ret = true;
		tick(m_frameCount++);
	}

	if (ret)
	{
		scene()->invalidate();
	}

	return true;
}
