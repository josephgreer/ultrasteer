#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <QFrame>
#include <QtGui/QFileDialog>
#include "ui_FileWidget.h"
#include "UICore.h"

namespace Nf 
{
  class FileWidget : public QFrame
  {
    Q_OBJECT

  public:
    FileWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~FileWidget();

    const char *GetFilename();
    void SetFilename(const char *filename);

  private:
    Ui::FileWidgetClass ui;
  };
};

#endif // FILEWIDGET_H
