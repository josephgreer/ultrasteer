#ifndef SAVEDATAWIDGET_H
#define SAVEDATAWIDGET_H

#include <QFrame>
#include <QtGui/QFileDialog>
#include "ui_SaveDataWidget.h"
#include "UICore.h"
#include "Resizable.h"

namespace Nf 
{
  class SaveDataWidget : public ResizableQFrame
  {
    Q_OBJECT

  public:
    
    SaveDataWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~SaveDataWidget();

    void SetEnabled(bool enable);

  private:
    Ui::SaveDataWidget ui;
  };
};

#endif // SAVEDATAWIDGET_H
