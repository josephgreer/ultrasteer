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
  class SaveDataWidget : public ResizableQFrame
  {
    Q_OBJECT

  public:
    
    SaveDataWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~SaveDataWidget();

    void SetEnabled(bool enable);
    void SetProgressBarValue(s32 value);
    void SetProgressBarRange(s32 range);

    void BeginRecording();
    void SaveDataFrame(const RPData &data);
    void DoneRecording();
    void StartRecording();
    void StopRecording();
    void ResumeRecording();
    void SaveData(RPFileHeader header);
    void SaveData(RPFileHeader header, const char *filename);
    void SaveDataWithSnap(RPFileHeader header, const char *filename, RPData &snap);
    void FreeData();
    bool HasData() const;

  public:
    Ui::SaveDataWidget ui;
    std::vector < RPData > m_dataToSave;

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
