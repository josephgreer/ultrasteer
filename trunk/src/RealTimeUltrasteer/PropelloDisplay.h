#pragma once

#include "QGraphicsView.h"
#include "RobotControl.h"

class PropelloDisplay : public QGraphicsView
{
public:
    PropelloDisplay(QWidget* parent = 0);
    virtual ~PropelloDisplay();

    bool init(int index);
    virtual bool setImgData() { return true; }

protected:
    virtual void drawForeground(QPainter* painter, const QRectF& r);
    virtual void resizeEvent(QResizeEvent*);

protected:
    void getOptDims(int& w, int& h);
    void setupBuffer(int w, int h);

protected:

    int m_index;
    QImage* m_image;
    QImage* m_colorimage;
};
