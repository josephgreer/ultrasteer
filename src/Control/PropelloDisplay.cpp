#include "stdafx.h"
#include "PropelloDisplay.h"

int newFrameInterrupt(void* param, int /*id*/, int /*header*/)
{
	PropelloDisplay* pd = (PropelloDisplay*)param;
	return pd->setImgData() ? 1 : 0;
}

PropelloDisplay::PropelloDisplay(QWidget* parent) : QGraphicsView(parent)
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

	if (m_image)
	{
		painter->drawImage(0, 0, *m_image, 0, 0);
	}
}

void PropelloDisplay::setupBuffer(int w, int h)
{
	int sz;

	sz = w * h * 4;

	if (m_image)
	{
		delete m_image;
	}
	if (m_colorimage)
	{
		delete m_colorimage;
	}

	m_image = new QImage(w, h, QImage::Format_RGB32);
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


