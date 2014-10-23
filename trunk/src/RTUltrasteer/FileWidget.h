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

    enum FileMode
    {
      FM_OPEN = 0,
      FM_SAVE,
    };
    
    FileWidget(FileMode fm, const char *dir, const char *filename, const char *extWC = "Anything (*.*)", QWidget *parent = 0, Qt::WFlags flags = 0);
    ~FileWidget();

    std::string GetFilename();
    void SetFilename(const char *filename);

  public slots:
    void FileDlgPushed();

  signals:
    void FileChanged();

  private:
    Ui::FileWidgetClass ui;
    FileMode m_fm;
    std::string m_extWC;
    std::string m_dir;
    std::string m_fname;
  };
};

#endif // FILEWIDGET_H
