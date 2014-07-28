#ifndef RTULTRASTEER_H
#define RTULTRASTEER_H

#include <QtGui/QMainWindow>
#include "ui_rtultrasteer.h"

class RTUltrasteer : public QMainWindow
{
    Q_OBJECT

public:
    RTUltrasteer(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~RTUltrasteer();

private:
    Ui::RTUltrasteerClass ui;
};

#endif // RTULTRASTEER_H
