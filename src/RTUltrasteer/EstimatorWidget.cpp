#include "EstimatorWidget.h"

namespace Nf
{
  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
  {
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  EstimatorStreamingWidget::EstimatorStreamingWidget(QWidget *parent)
    : RPStreamingWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(ES_READY)
    , m_hwWidget(new RobotHardwareWidget(parent))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_bottomRow(new QGridLayout(parent))
  {
    m_bottomRow->addWidget(m_hwWidget.get(), 0, 0);
    m_bottomRow->addWidget(m_saveDataWidget.get(), 0, 1, Qt::Alignment(Qt::AlignTop));
    m_layout->addLayout(m_bottomRow.get(), 1, 0, 1, 2);

    ADD_CHILD_COLLECTION(m_hwWidget.get());

    
    Connect(m_saveDataWidget->ui.allocateDataButton, SIGNAL(clicked()), SLOT(onSaveDataClicked()));
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

    m_saveDataWidget->AllocateData(temp);

    temp.Release();

  }

  void EstimatorStreamingWidget::onFrame()
  {
    if(!m_init->GetValue() || !m_lock.tryLock(30))
      return;
    
    RPData rp = m_data.Clone();
    HandleFrame(rp);
    DataFrame d;
    d.u = m_u;
    d.rp = rp;
    m_saveDataWidget->SaveDataFrame(d);
    rp.Release();
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