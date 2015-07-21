#include "EstimatorWidget.h"

namespace Nf
{
#define MAX_TIMESTEPS 800
  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
  {
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  EstimatorStreamingWidget::EstimatorStreamingWidget(QWidget *parent)
    : RPStreamingWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_hwWidget(new RobotHardwareWidget(parent))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_bottomRow(new QGridLayout(parent))
  {
    m_bottomRow->addWidget(m_hwWidget.get(), 0, 0);
    m_bottomRow->addWidget(m_saveDataWidget.get(), 0, 1, Qt::Alignment(Qt::AlignTop));
    m_layout->addLayout(m_bottomRow.get(), 1, 0, 1, 2);

    m_saveDataWidget->SetEnabled(false);

    ADD_CHILD_COLLECTION(m_hwWidget.get());

    m_saveDataWidget->SetProgressBarRange(MAX_TIMESTEPS);
    m_saveDataWidget->SetProgressBarValue(0);

    
    Connect(m_saveDataWidget->ui.startRecordingButton, SIGNAL(clicked()), SLOT(onSaveDataClicked()));
  }

  void EstimatorStreamingWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_imageViewer->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_usVis->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_planeVis->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_hwWidget->UpdateSize(QSize(3*sz.width()/4-10, (sz.height())/4));
    m_saveDataWidget->UpdateSize(QSize(sz.width()/4-10, (sz.height())/4));
  }

  void EstimatorStreamingWidget::onInitializeToggle()
  {
    RPStreamingWidget::onInitializeToggle();
    m_saveDataWidget->SetEnabled(m_init->GetValue());
  }

  void EstimatorStreamingWidget::onSaveDataClicked()
  {
    //TODO CHANGE THSI
    if(!m_lock.tryLock(1000))
      return;
    RPData temp = m_data.Clone();
    m_lock.unlock();

    //s32 colorFrameSz = temp.color->widthStep*temp.color->height;
    s32 colorFrameSz = temp.b8->widthStep*temp.b8->height*4;
    s32 b8FrameSz = temp.b8->widthStep*temp.b8->height;
    u8 *memColor = (u8 *)malloc(colorFrameSz*MAX_TIMESTEPS);
    u8 *memB8 = (u8 *)malloc(b8FrameSz*MAX_TIMESTEPS);

    u8 *currColor = memColor;
    u8 * currB8 = memB8;

    DataFrame d;
    RPData rp;
    for(s32 i=0; i<MAX_TIMESTEPS; i++) {
      rp.b8 = cvCreateImageHeader(cvSize(temp.b8->width, temp.b8->height), IPL_DEPTH_8U, 1);
      rp.color = cvCreateImageHeader(cvSize(temp.b8->width, temp.b8->height), IPL_DEPTH_8U, 4);
      cvSetImageData(rp.b8, currB8, temp.b8->widthStep);
      cvSetImageData(rp.color, currColor, temp.b8->widthStep*4);
      d.rp = rp;
      d.timestamp = 0;
      m_unusedData.push_back(d);

      currColor += colorFrameSz;
      currB8 += b8FrameSz;
    }

    temp.Release();

  }

  void EstimatorStreamingWidget::SetRobot(NeedleSteeringRobot *robot)
  {
    m_robot = robot;
    m_hwWidget->setRobot(robot);
  }

  EstimatorStreamingWidget::~EstimatorStreamingWidget()
  {
  }
}