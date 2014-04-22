#pragma once

#include "PropelloDisplay.h"

//MEM_TEST exercises the segmentation pipeline without
//using porta.  It is used to detect memory leaks within our code.
//Because porta has such a large memory footprint, it is difficult 
//to detect memory leaks while porta is initialized.
//#define MEM_TEST

class PropelloBDisplay : public PropelloDisplay
{
	Q_OBJECT

public:
    PropelloBDisplay(QWidget* parent = 0);
    virtual ~PropelloBDisplay();

    bool loadColorMap(const QString file);
    virtual bool setImgData(bool usePorta);
#ifdef MEM_TEST
	QTimer m_memTestTimer;
	bool m_timerStarted;
#endif

private:
	int m_frameCount;

signals:
	void tick(int framenum);

#ifdef MEM_TEST
public slots:
	void onMemTestTimer();
#endif
};
