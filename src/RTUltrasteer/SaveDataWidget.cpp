#include "SaveDataWidget.h"

namespace Nf
{
  SaveDataWidget::SaveDataWidget(QWidget *parent, Qt::WFlags flags)
    : ResizableQFrame(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
  {
    ui.setupUi(this);
  }

  SaveDataWidget::~SaveDataWidget()
  {
  }

  void SaveDataWidget::SetEnabled(bool enabled)
  {
    ui.progressBar->setEnabled(enabled);
    ui.startRecordingButton->setEnabled(enabled);
    ui.saveDataButton->setEnabled(enabled);
  }
}

