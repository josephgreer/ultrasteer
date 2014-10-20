#include "FileWidget.h"

namespace Nf
{
  FileWidget::FileWidget(QWidget *parent, Qt::WFlags flags)
    : QFrame(parent, flags)
  {
    ui.setupUi(this);
  }

  FileWidget::~FileWidget()
  {
  }

  const char *FileWidget::GetFilename()
  {
    return ui.fnameEdit->toPlainText().toStdString().c_str();
  }

  void FileWidget::SetFilename(const char *filename)
  {
  }
}

