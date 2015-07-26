#ifndef SAVEDATAWIDGET_H
#define SAVEDATAWIDGET_H

#include <QFrame>
#include <QtGui/QFileDialog>
#include "ui_SaveDataWidget.h"
#include "UICore.h"
#include "Resizable.h"

#include "ParticleFilter.h"
#include "RPFileReader.h"

namespace Nf 
{
  struct DataFrame
  {
    RPData rp;
    NSCommand u;
  };

  class SaveDataWidget : public ResizableQFrame
  {
    Q_OBJECT

  public:
    
    SaveDataWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~SaveDataWidget();

    void SetEnabled(bool enable);
    void SetProgressBarValue(s32 value);
    void SetProgressBarRange(s32 range);

    void AllocateData(RPData &data);
    void BeginRecording();
    void SaveDataFrame(DataFrame &data);
    void DoneRecording();

  public:
    Ui::SaveDataWidget ui;
    std::vector < DataFrame > m_dataToSave;
    std::vector < DataFrame > m_unusedData;

  public slots:
    void onStartRecordingClicked();
    void onSaveDataClicked();

  protected:
    bool m_isRecording;
    u8 *m_memB8;
    u8 *m_memColor;
  };
};

#endif // SAVEDATAWIDGET_H
