#pragma once

#include "PropelloDisplay.h"

class PropelloBDisplay : public PropelloDisplay
{
public:
    PropelloBDisplay(QWidget* parent = 0);
    virtual ~PropelloBDisplay();

    bool loadColorMap(const QString file);
    virtual bool setImgData();
};
