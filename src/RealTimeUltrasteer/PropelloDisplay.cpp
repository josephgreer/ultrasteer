#include "stdafx.h"
#include "PropelloDisplay.h"
#include "SegmentCore.h"

int newFrameInterrupt(void* param, int /*id*/, int /*header*/)
{
	PropelloDisplay* pd = (PropelloDisplay*)param;
	return pd->setImgData(true) ? 1 : 0;
}

PropelloDisplay::PropelloDisplay(QWidget* parent) 
	: QGraphicsView(parent)
	, m_mutex(NULL)
	, m_disImage(NULL)
{
	QGraphicsScene* sc = new QGraphicsScene(this);
	setScene(sc);

	m_image = 0;
	m_colorimage = 0;
	m_index = -1;
}

PropelloDisplay::~PropelloDisplay()
{
}


QImage *PropelloDisplay::getBmode()
{
	return m_image;
}

QImage *PropelloDisplay::getDoppler()
{
	return m_colorimage;
}


void PropelloDisplay::setMutex(QMutex *mutex)
{
	this->m_mutex = mutex;
}


void PropelloDisplay::SetDisplayImage(QImage *disImage)
{
	if(m_disImage)
		delete m_disImage;
	m_disImage = disImage;
}

bool PropelloDisplay::init(int index)
{
	int w, h;
	m_index = index;

	portaSetDisplayCallback(m_index, newFrameInterrupt, (void*)this);

	getOptDims(w, h);
	portaSetDisplayDimensions(m_index, w, h);
	setupBuffer(w, h);
	setSceneRect(0, 0, w, h);

	return true;
}

void PropelloDisplay::resizeEvent(QResizeEvent* event)
{
	if (portaIsConnected())
	{
		//TODO:  Handle this case by reinitializing the needle segmenter with
		//new image dimensions.  For now, just fail.
		Nf::NTrace("Error, cannot resize\n");
		ASSERT(0);
		int w, h;
		getOptDims(w, h);
		portaSetDisplayDimensions(m_index, w, h);
		setupBuffer(w, h);
		setSceneRect(0, 0, w, h);
	}

	QGraphicsView::resizeEvent(event);
}

void PropelloDisplay::drawForeground(QPainter* painter, const QRectF& r)
{
	QGraphicsView::drawForeground(painter, r);

	if (m_disImage)
	{
		//imagingMode mode = portaGetCurrentMode();
		if(m_mutex->tryLock()) {
			painter->drawImage(0, 0, *m_disImage, 0, 0);
			m_mutex->unlock();
		}
	} 
}

void PropelloDisplay::setupBuffer(int w, int h)
{
	if (m_image)
	{
		delete m_image;
	}
	if (m_colorimage)
	{
		delete m_colorimage;
	}

  m_image = new QImage(w, h, QImage::Format_Indexed8);
  //set up grayscale indexing to identity map
  for(s32 i=0; i<256; i++)
    m_image->setColor(i, qRgb(i,i,i));
  m_colorimage = new QImage(w, h, QImage::Format_RGB32);
}

void PropelloDisplay::getOptDims(int& w, int& h)
{
	w = width();
	while (w % 4)
	{
		w--;
	}

	h = height();
	while (h % 4)
	{
		h--;
	}
}


