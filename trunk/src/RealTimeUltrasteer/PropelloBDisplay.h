#pragma once

#include "PropelloDisplay.h"

class PropelloBDisplay : public PropelloDisplay
{
	Q_OBJECT

public:
    PropelloBDisplay(QWidget* parent = 0);
    virtual ~PropelloBDisplay();

    bool loadColorMap(const QString file);
    virtual bool setImgData();

signals:
	void tick(int framenum);
};
