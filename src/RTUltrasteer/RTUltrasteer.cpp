#include "RTUltrasteer.h"
#include "FileWidget.h"
#include <QComboBox>
#include <stdio.h>
#include <stdlib.h>

namespace Nf
{

  void SlotForwarder::treeWidgetForward(QTreeWidgetItem *item, int col) 
  {
    if((void * &)item == (void * &)m_element) {
      if(m_fptr)
        m_fptr(m_context);
      //repaint
      for(s32 i=0; i<(s32)m_repaintList.size(); i++) {
        m_repaintList[i]->repaint();
      }
    }
  }

  void SlotForwarder::changeForward()
  {
    if(m_fptr)
      m_fptr(m_context);
    //repaint
    for(s32 i=0; i<(s32)m_repaintList.size(); i++) {
      m_repaintList[i]->repaint();
    }
  }
}


using namespace Nf;

RTUltrasteer::RTUltrasteer(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
    , Nf::ParameterCollection("RTUltrasteer")
    , m_menu(NULL)
    , m_params(NULL)
    , m_usDock(NULL)
    , m_usVis(NULL)
    , m_mouse(this)
{
  ui.setupUi(this);

  ADD_BOOL_PARAMETER(m_usDockVisible, "Show Full US Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_rpFileWidgetVisible, "Show Incremental Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_rpStreamingWidgetVisible, "Show Streaming Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_estimatorFileWidgetVisible, "Show ParticleFilter File Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_estimatorStreamingWidgetVisible, "Show ParticleFilter Streaming Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_robotHWWidgetVisible, "Show Robot HW Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_teleoperation2DFileWidgetVisible, "Show 2D Teleoperation File Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_teleoperation2DStreamWidgetVisible, "Show 2D Teleoperation Stream Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_emCalibrationFileWidgetVisible, "Show EM Calibration File Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_emCalibrationStreamWidgetVisible, "Show EM Calibration Stream Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_fsExperimentFileWidgetVisible, "Show FS Experiment File Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_fsExperimentStreamWidgetVisible, "Show FS Experiment Stream Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_fsSimpleStreamWidgetVisible, "Show FS Simple Stream Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, false);
  ADD_BOOL_PARAMETER(m_vineWidgetVisible, "Show Vine Dock", CALLBACK_POINTER(onSetDocksVisible, RTUltrasteer), this, true);

  CreateUSVisualizer();
  CreateMenuDock();
  CreateRPFileDock();
  CreateRPStreamingDock();
  CreateEstimatorFileDock();
  CreateEstimatorStreamingDock();
  CreateRobotHWDock();
  CreateTeleoperation2DFileDock();
  CreateTeleoperation2DStreamDock();
  CreateEMCalibrationFileDock();
  CreateEMCalibrationStreamDock();
  CreateFSExperimentFileDock();
  CreateFSExperimentStreamDock();
  CreateFSSimpleStreamDock();
	CreateVineWidget();

  QTreeWidgetItem * rt = new QTreeWidgetItem(m_params);
  rt->setText(0, this->GetName());
  CreateUIElements(rt, *this, std::vector<QVTKWidget *>());

  QTreeWidgetItem * usVis = new QTreeWidgetItem(m_params);
  usVis->setText(0, m_usVis->GetName());
  CreateUIElements(usVis, *m_usVis, m_usVis->GetRepaintList());

  QTreeWidgetItem * rpF = new QTreeWidgetItem(m_params);
  rpF->setText(0, "RPFileDock");
  CreateUIElements(rpF, *m_rpFileWidget, m_rpFileWidget->GetChildWidgets());

  QTreeWidgetItem * rpS = new QTreeWidgetItem(m_params);
  rpS->setText(0, "RPStreamingDock");
  CreateUIElements(rpS, *m_rpStreamingWidget, m_rpStreamingWidget->GetChildWidgets());

  QTreeWidgetItem * estF = new QTreeWidgetItem(m_params);
  estF->setText(0, "Particle Filter File");
  CreateUIElements(estF, *m_estimatorFileWidget, m_estimatorFileWidget->GetChildWidgets());

  QTreeWidgetItem * estS = new QTreeWidgetItem(m_params);
  estS->setText(0, "Particle Filter Streaming");
  CreateUIElements(estS, *m_estimatorStreamingWidget, m_estimatorStreamingWidget->GetChildWidgets());

  QTreeWidgetItem * rpRHW = new QTreeWidgetItem(m_params);
  rpRHW->setText(0, "RobotHW");
  CreateUIElements(rpRHW, *m_robotHWWidget, m_robotHWWidget->GetChildWidgets());

  QTreeWidgetItem * rpTLFW = new QTreeWidgetItem(m_params);
  rpTLFW->setText(0, "Teleoperation2DFile");
  CreateUIElements(rpTLFW, *m_teleoperation2DFileWidget, m_teleoperation2DFileWidget->GetChildWidgets());

  QTreeWidgetItem * rpTLSW = new QTreeWidgetItem(m_params);
  rpTLSW->setText(0, "Teleoperation2DStream");
  CreateUIElements(rpTLSW, *m_teleoperation2DStreamWidget, m_teleoperation2DStreamWidget->GetChildWidgets());

  QTreeWidgetItem * rpEMCFW = new QTreeWidgetItem(m_params);
  rpEMCFW->setText(0, "EMCalibrationFile");
  CreateUIElements(rpEMCFW, *m_EMCalibrationFileWidget, m_EMCalibrationFileWidget->GetChildWidgets());

  QTreeWidgetItem * rpEMCSW = new QTreeWidgetItem(m_params);
  rpEMCSW->setText(0, "EMCalibrationStream");
  CreateUIElements(rpEMCSW, *m_EMCalibrationStreamWidget, m_EMCalibrationStreamWidget->GetChildWidgets());

  QTreeWidgetItem * rpFSEFW = new QTreeWidgetItem(m_params);
  rpFSEFW->setText(0, "FSExperimentFile");
  CreateUIElements(rpFSEFW, *m_fsExperimentFileWidget, m_fsExperimentFileWidget->GetChildWidgets());

  QTreeWidgetItem * rpFSESW = new QTreeWidgetItem(m_params);
  rpFSESW->setText(0, "FSExperimentStream");
  CreateUIElements(rpFSESW, *m_fsExperimentStreamWidget, m_fsExperimentStreamWidget->GetChildWidgets());

  QTreeWidgetItem * rpFSSSW = new QTreeWidgetItem(m_params);
  rpFSSSW->setText(0, "FSSimpleStream");
  CreateUIElements(rpFSSSW, *m_fsSimpleStreamWidget, m_fsSimpleStreamWidget->GetChildWidgets());

  QTreeWidgetItem * rpVine = new QTreeWidgetItem(m_params);
  rpVine->setText(0, "Vine");
  CreateUIElements(rpVine, *m_vineWidget, m_vineWidget->GetChildWidgets());

  //Add to our map of root dock windows
  m_roots[std::string("USDock")].dock = m_usDock;
  m_roots[std::string("USDock")].param = m_usDockVisible.get();
  m_roots[std::string("USDock")].root = usVis;
  m_roots[std::string("USDock")].resize = (Resizable *)m_usVis;
  m_roots[std::string("USDock")].collection = (ParameterCollection *)m_usVis;
  m_roots[std::string("RPFileDock")].dock = m_rpFileDock;
  m_roots[std::string("RPFileDock")].param = m_rpFileWidgetVisible.get();
  m_roots[std::string("RPFileDock")].root = rpF;
  m_roots[std::string("RPFileDock")].resize = (Resizable *)m_rpFileWidget;
  m_roots[std::string("RPFileDock")].collection = (ParameterCollection *)m_rpFileWidget;
  m_roots[std::string("RPStreamingDock")].dock = m_rpStreamingDock;
  m_roots[std::string("RPStreamingDock")].param = m_rpStreamingWidgetVisible.get();
  m_roots[std::string("RPStreamingDock")].root = rpS;
  m_roots[std::string("RPStreamingDock")].resize = (Resizable *)m_rpStreamingWidget;
  m_roots[std::string("RPStreamingDock")].collection = (ParameterCollection *)m_rpStreamingWidget;
  m_roots[std::string("EstimatorFileDock")].dock = m_estimatorFileDock;
  m_roots[std::string("EstimatorFileDock")].param = m_estimatorFileWidgetVisible.get();
  m_roots[std::string("EstimatorFileDock")].root = estF;
  m_roots[std::string("EstimatorFileDock")].resize = (Resizable *)m_estimatorFileWidget;
  m_roots[std::string("EstimatorFileDock")].collection = (ParameterCollection *)m_estimatorFileWidget;
  m_roots[std::string("EstimatorStreamingDock")].dock = m_estimatorStreamingDock;
  m_roots[std::string("EstimatorStreamingDock")].param = m_estimatorStreamingWidgetVisible.get();
  m_roots[std::string("EstimatorStreamingDock")].root = estS;
  m_roots[std::string("EstimatorStreamingDock")].resize = (Resizable *)m_estimatorStreamingWidget;
  m_roots[std::string("EstimatorStreamingDock")].collection = (ParameterCollection *)m_estimatorStreamingWidget;
  m_roots[std::string("RobotHWDock")].dock = m_robotHWDock;
  m_roots[std::string("RobotHWDock")].param = m_robotHWWidgetVisible.get();
  m_roots[std::string("RobotHWDock")].root = rpRHW;
  m_roots[std::string("RobotHWDock")].resize = (Resizable *)m_robotHWWidget;
  m_roots[std::string("RobotHWDock")].collection = (ParameterCollection *)m_robotHWWidget;
  m_roots[std::string("2DTeleoperationFileDock")].dock = m_teleoperation2DFileDock;
  m_roots[std::string("2DTeleoperationFileDock")].param = m_teleoperation2DFileWidgetVisible.get();
  m_roots[std::string("2DTeleoperationFileDock")].root = rpTLFW;
  m_roots[std::string("2DTeleoperationFileDock")].resize = (Resizable *)m_teleoperation2DFileWidget;
  m_roots[std::string("2DTeleoperationFileDock")].collection = (ParameterCollection *)m_teleoperation2DFileWidget;
  m_roots[std::string("2DTeleoperationStreamingDock")].dock = m_teleoperation2DStreamDock;
  m_roots[std::string("2DTeleoperationStreamingDock")].param = m_teleoperation2DStreamWidgetVisible.get();
  m_roots[std::string("2DTeleoperationStreamingDock")].root = rpTLSW;
  m_roots[std::string("2DTeleoperationStreamingDock")].resize = (Resizable *)m_teleoperation2DStreamWidget;
  m_roots[std::string("2DTeleoperationStreamingDock")].collection = (ParameterCollection *)m_teleoperation2DStreamWidget;
  m_roots[std::string("EMCalibrationFileDock")].dock = m_EMCalibrationFileDock;
  m_roots[std::string("EMCalibrationFileDock")].param = m_emCalibrationFileWidgetVisible.get();
  m_roots[std::string("EMCalibrationFileDock")].root = rpEMCFW;
  m_roots[std::string("EMCalibrationFileDock")].resize = (Resizable *)m_EMCalibrationFileWidget;
  m_roots[std::string("EMCalibrationFileDock")].collection = (ParameterCollection *)m_EMCalibrationFileWidget;
  m_roots[std::string("EMCalibrationStreamingDock")].dock = m_EMCalibrationStreamDock;
  m_roots[std::string("EMCalibrationStreamingDock")].param = m_emCalibrationStreamWidgetVisible.get();
  m_roots[std::string("EMCalibrationStreamingDock")].root = rpEMCSW;
  m_roots[std::string("EMCalibrationStreamingDock")].resize = (Resizable *)m_EMCalibrationStreamWidget;
  m_roots[std::string("EMCalibrationStreamingDock")].collection = (ParameterCollection *)m_EMCalibrationStreamWidget;
  m_roots[std::string("FSExperimentFileDock")].dock = m_FSExperimentFileDock;
  m_roots[std::string("FSExperimentFileDock")].param = m_fsExperimentFileWidgetVisible.get();
  m_roots[std::string("FSExperimentFileDock")].root = rpFSEFW;
  m_roots[std::string("FSExperimentFileDock")].resize = (Resizable *)m_fsExperimentFileWidget;
  m_roots[std::string("FSExperimentFileDock")].collection = (ParameterCollection *)m_fsExperimentFileWidget;
  m_roots[std::string("FSExperimentStreamingDock")].dock = m_FSExperimentStreamDock;
  m_roots[std::string("FSExperimentStreamingDock")].param = m_fsExperimentStreamWidgetVisible.get();
  m_roots[std::string("FSExperimentStreamingDock")].root = rpFSESW;
  m_roots[std::string("FSExperimentStreamingDock")].resize = (Resizable *)m_fsExperimentStreamWidget;
  m_roots[std::string("FSExperimentStreamingDock")].collection = (ParameterCollection *)m_fsExperimentStreamWidget;
  m_roots[std::string("FSSimpleStreamDock")].dock = m_FSSimpleStreamDock;
  m_roots[std::string("FSSimpleStreamDock")].param = m_fsSimpleStreamWidgetVisible.get();
  m_roots[std::string("FSSimpleStreamDock")].root = rpFSSSW;
  m_roots[std::string("FSSimpleStreamDock")].resize = (Resizable *)m_fsSimpleStreamWidget;
  m_roots[std::string("FSSimpleStreamDock")].collection = (ParameterCollection *)m_fsSimpleStreamWidget;
  m_roots[std::string("VineWidget")].dock = m_vineWidgetDock;
  m_roots[std::string("VineWidget")].param = m_vineWidgetVisible.get();
  m_roots[std::string("VineWidget")].root = rpVine;
  m_roots[std::string("VineWidget")].resize = (Resizable *)m_vineWidget;
  m_roots[std::string("VineWidget")].collection = (ParameterCollection *)m_vineWidget;

  QDockWidget *last = NULL;
  for(std::map < std::string, DockWidgetInfo >::iterator i=m_roots.begin(); i!=m_roots.end(); i++) {
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, i->second.dock);
    if(last)
      this->tabifyDockWidget(last, i->second.dock);
    last = i->second.dock;
  }

  connect(ui.actionSave_Parameters, SIGNAL(triggered(bool)), SLOT(onSaveParameters()));
  connect(ui.actionLoad_Parameters, SIGNAL(triggered(bool)), SLOT(onLoadParameters()));

  onSetDocksVisible();
  m_control.setRobot(&m_robot);
}

void RTUltrasteer::onSetDocksVisible()
{
  //First dock widget we've encountered 
  QDockWidget *first = NULL;

  m_params->expandAll();

  for(std::map<std::string, Nf::DockWidgetInfo >::iterator i=m_roots.begin(); i!=m_roots.end(); i++) {
    i->second.dock->setHidden(!i->second.param->GetValue());
    if(!i->second.param->GetValue())
      i->second.root->setExpanded(false);
  }

  Resize();
}

void RTUltrasteer::resizeEvent(QResizeEvent *event)
{
  Resize();
}

void RTUltrasteer::Resize()
{
  QSize totalSize = this->size();
  QSize menuSize = m_params->size();
  s32 w = totalSize.width()-menuSize.width()-10;

  for(std::map<std::string, Nf::DockWidgetInfo >::iterator i=m_roots.begin(); i!=m_roots.end(); i++) {
    QSize sz = i->second.dock->size();
    i->second.resize->UpdateSize(QSize(w, sz.height()-100));
  }
}

#define EL_VALUE(vec, i) ((i) == 0 ? vec.x : ((i) == 1 ? vec.y : vec.z))
#define EL_VALUE2(vec, i) ((i) == 0 ? vec.x : vec.y)

void RTUltrasteer::CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection, const std::vector < QVTKWidget * > & repainters) 
{

  //File Open Parameters
  std::vector < std::tr1::shared_ptr < Nf::FileParameter > > ofiles = collection.GetOpenFileParameters();
  for(s32 i=0; i<(s32)ofiles.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, ofiles[i]->GetName());

    Nf::FileWidget *fw = new Nf::FileWidget(Nf::FileWidget::FileMode::FM_OPEN, 
      ofiles[i]->GetDirectory().c_str(), ofiles[i]->GetFilename().c_str(), ofiles[i]->GetExtensionWildcard().c_str(), m_params);
    std::tr1::shared_ptr < Nf::FileSlotForwarder > uiElem (new Nf::FileSlotForwarder(ofiles[i]->GetCallback(), ofiles[i]->GetContext(), (QObject *)fw, repainters, NULL));
    m_params->setItemWidget(child, 1, fw); 
    ofiles[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< std::string > >(uiElem));
    QObject::connect((QObject *)fw, SIGNAL(FileChanged()), (QObject *)&(*uiElem), SLOT(changeForward()));
  }

  //File Open Parameters
  std::vector < std::tr1::shared_ptr < Nf::FileParameter > > sfiles = collection.GetSaveFileParameters();
  for(s32 i=0; i<(s32)sfiles.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, sfiles[i]->GetName());

    Nf::FileWidget *fw = new Nf::FileWidget(Nf::FileWidget::FileMode::FM_SAVE, sfiles[i]->GetDirectory().c_str(),
      sfiles[i]->GetFilename().c_str(), sfiles[i]->GetExtensionWildcard().c_str(), m_params);
    std::tr1::shared_ptr < Nf::FileSlotForwarder > uiElem (new Nf::FileSlotForwarder(sfiles[i]->GetCallback(), sfiles[i]->GetContext(), (QObject *)fw, repainters, NULL));
    m_params->setItemWidget(child, 1, fw); 
    sfiles[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< std::string > >(uiElem));
    QObject::connect((QObject *)fw, SIGNAL(FileChanged()), (QObject *)&(*uiElem), SLOT(changeForward()));
  }

  //Bool Parameters
  std::vector < std::tr1::shared_ptr < Nf::BoolParameter > > bools = collection.GetBoolParameters();
  for(s32 i=0; i<(s32)bools.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, bools[i]->GetName());

    //Nf::BoolSlotForwarder * uiElem = new Nf::BoolSlotForwarder(NULL, NULL, child, NULL);
    std::tr1::shared_ptr < Nf::BoolSlotForwarder > uiElem (new Nf::BoolSlotForwarder(bools[i]->GetCallback(), bools[i]->GetContext(), (QObject *)child, repainters, NULL));
    child->setCheckState(1, bools[i]->GetValue() ? Qt::Checked : Qt::Unchecked);
    child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
    bools[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< bool > >(uiElem));
    QObject::connect((QObject *)m_params, SIGNAL(itemClicked(QTreeWidgetItem *, int)), (QObject *)&(*uiElem), SLOT(treeWidgetForward(QTreeWidgetItem *, int)));
  }

  //Action Parameters
  std::vector < std::tr1::shared_ptr < Nf::BoolParameter > > actions = collection.GetActionParameters();
  for(s32 i=0; i<(s32)actions.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, actions[i]->GetName());

    QPushButton *execButton = new QPushButton("Exec", m_params);
    execButton->setMaximumWidth(50);
    std::tr1::shared_ptr < Nf::ActionSlotForwarder > uiElem (new Nf::ActionSlotForwarder(actions[i]->GetCallback(), actions[i]->GetContext(), (QObject *)execButton, repainters, NULL));
    m_params->setItemWidget(child, 1, execButton); 
    actions[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< bool > >(uiElem));
    QObject::connect((QObject *)execButton, SIGNAL(pressed()), (QObject *)&(*uiElem), SLOT(changeForward()));
  }

  //Int Parameters
  std::vector < std::tr1::shared_ptr < Nf::IntParameter > > ints = collection.GetIntParameters();
  for(s32 i=0; i<(s32)ints.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, ints[i]->GetName());

    QSpinBox *sb = new QSpinBox(m_params);
    sb->setMinimum(ints[i]->GetMin());
    sb->setMaximum(ints[i]->GetMax());
    sb->setSingleStep(ints[i]->GetStep());
    sb->setMaximumWidth(8*8);
    sb->setValue(ints[i]->GetValue());

    std::tr1::shared_ptr < Nf::IntSlotForwarder > uiElem (new Nf::IntSlotForwarder(ints[i]->GetCallback(), ints[i]->GetContext(), (QObject *)sb, repainters, NULL));
    m_params->setItemWidget(child, 1, sb); 
    ints[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< s32 > >(uiElem));
    QObject::connect((QObject *)sb, SIGNAL(valueChanged(int)), (QObject *)&(*uiElem), SLOT(changeForward()));
  }

  //Float Parameters
  std::vector < std::tr1::shared_ptr < Nf::FloatParameter > > floats = collection.GetFloatParameters();
  for(s32 i=0; i<(s32)floats.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, floats[i]->GetName());

    QDoubleSpinBox *sb = new QDoubleSpinBox(m_params);
    sb->setDecimals(50);
    sb->setMinimum(floats[i]->GetMin());
    sb->setMaximum(floats[i]->GetMax());
    sb->setSingleStep(floats[i]->GetStep());
    sb->setMaximumWidth(8*8);
    sb->setValue(floats[i]->GetValue());

    std::tr1::shared_ptr < Nf::FloatSlotForwarder > uiElem (new Nf::FloatSlotForwarder(floats[i]->GetCallback(), floats[i]->GetContext(), (QObject *)sb, repainters, NULL));
    m_params->setItemWidget(child, 1, sb); 
    floats[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< f32 > >(uiElem));
    QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
  }

  //Vec3d Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec3dParameter > > vec3ds = collection.GetVec3dParameters();
  for(s32 i=0; i<(s32)vec3ds.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, vec3ds[i]->GetName());


    Nf::SBContainer *sbs = new Nf::SBContainer();
    sbs->nsbs = 3;
    std::tr1::shared_ptr < Nf::Vec3dSlotForwarder > uiElem (new Nf::Vec3dSlotForwarder(vec3ds[i]->GetCallback(), vec3ds[i]->GetContext(), (QObject *)sbs, repainters, NULL));

    const char labels[3][3] = {"x", "y", "z"};
    for(s32 jj=0; jj<3; jj++) {
      QTreeWidgetItem *grandChild = new QTreeWidgetItem();
      child->addChild(grandChild);
      grandChild->setText(0, labels[jj]);

      QDoubleSpinBox *sb = new QDoubleSpinBox(m_params);
      sb->setDecimals(50);
      sb->setMinimum(EL_VALUE2(vec3ds[i]->GetMin(),jj));
      sb->setMaximum(EL_VALUE2(vec3ds[i]->GetMax(),jj));
      sb->setSingleStep(EL_VALUE2(vec3ds[i]->GetStep(),jj));
      sb->setMaximumWidth(8*8);
      sb->setValue(EL_VALUE2(vec3ds[i]->GetValue(),jj));
      sbs->obj[jj] = (QObject *)sb;

      m_params->setItemWidget(grandChild, 1, sb); 

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
    }
    vec3ds[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< Nf::Vec3d > >(uiElem));
  }

  //Vec3f Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec3fParameter > > vec3fs = collection.GetVec3fParameters();
  for(s32 i=0; i<(s32)vec3fs.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, vec3fs[i]->GetName());


    Nf::SBContainer *sbs = new Nf::SBContainer();
    sbs->nsbs = 3;
    std::tr1::shared_ptr < Nf::Vec3fSlotForwarder > uiElem (new Nf::Vec3fSlotForwarder(vec3fs[i]->GetCallback(), vec3fs[i]->GetContext(), (QObject *)sbs, repainters, NULL));

    const char labels[3][3] = {"x", "y", "z"};
    for(s32 jj=0; jj<3; jj++) {
      QTreeWidgetItem *grandChild = new QTreeWidgetItem();
      child->addChild(grandChild);
      grandChild->setText(0, labels[jj]);

      QDoubleSpinBox *sb = new QDoubleSpinBox(m_params);
      sb->setDecimals(50);
      sb->setMinimum(EL_VALUE2(vec3fs[i]->GetMin(),jj));
      sb->setMaximum(EL_VALUE2(vec3fs[i]->GetMax(),jj));
      sb->setSingleStep(EL_VALUE2(vec3fs[i]->GetStep(),jj));
      sb->setMaximumWidth(8*8);
      sb->setValue(EL_VALUE2(vec3fs[i]->GetValue(),jj));
      sbs->obj[jj] = (QObject *)sb;

      m_params->setItemWidget(grandChild, 1, sb); 

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
    }
    vec3fs[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< Nf::Vec3f > >(uiElem));
  }

  //Vec3i Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec3iParameter > > vec3is = collection.GetVec3iParameters();
  for(s32 i=0; i<(s32)vec3is.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, vec3is[i]->GetName());


    Nf::SBContainer *sbs = new Nf::SBContainer();
    sbs->nsbs = 3;
    std::tr1::shared_ptr < Nf::Vec3iSlotForwarder > uiElem (new Nf::Vec3iSlotForwarder(vec3is[i]->GetCallback(), vec3is[i]->GetContext(), (QObject *)sbs, repainters, NULL));

    const char labels[3][3] = {"x", "y", "z"};
    for(s32 jj=0; jj<3; jj++) {
      QTreeWidgetItem *grandChild = new QTreeWidgetItem();
      child->addChild(grandChild);
      grandChild->setText(0, labels[jj]);

      QSpinBox *sb = new QSpinBox(m_params);
      sb->setMinimum(EL_VALUE2(vec3is[i]->GetMin(),jj));
      sb->setMaximum(EL_VALUE2(vec3is[i]->GetMax(),jj));
      sb->setSingleStep(EL_VALUE2(vec3is[i]->GetStep(),jj));
      sb->setMaximumWidth(8*8);
      sb->setValue(EL_VALUE2(vec3is[i]->GetValue(),jj));
      sbs->obj[jj] = (QObject *)sb;

      m_params->setItemWidget(grandChild, 1, sb); 

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(int)), (QObject *)&(*uiElem), SLOT(changeForward()));
    }
    vec3is[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< Nf::Vec3i > >(uiElem));
  }

  //Vec2d Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec2dParameter > > vec2ds = collection.GetVec2dParameters();
  for(s32 i=0; i<(s32)vec2ds.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, vec2ds[i]->GetName());


    Nf::SBContainer *sbs = new Nf::SBContainer();
    sbs->nsbs = 2;
    std::tr1::shared_ptr < Nf::Vec2dSlotForwarder > uiElem (new Nf::Vec2dSlotForwarder(vec2ds[i]->GetCallback(), vec2ds[i]->GetContext(), (QObject *)sbs, repainters, NULL));

    const char labels[3][3] = {"x", "y", "z"};
    for(s32 jj=0; jj<2; jj++) {
      QTreeWidgetItem *grandChild = new QTreeWidgetItem();
      child->addChild(grandChild);
      grandChild->setText(0, labels[jj]);

      QDoubleSpinBox *sb = new QDoubleSpinBox(m_params);
      sb->setDecimals(50);
      sb->setMinimum(EL_VALUE2(vec2ds[i]->GetMin(),jj));
      sb->setMaximum(EL_VALUE2(vec2ds[i]->GetMax(),jj));
      sb->setSingleStep(EL_VALUE2(vec2ds[i]->GetStep(),jj));
      sb->setMaximumWidth(8*8);
      sb->setValue(EL_VALUE2(vec2ds[i]->GetValue(),jj));
      sbs->obj[jj] = (QObject *)sb;

      m_params->setItemWidget(grandChild, 1, sb); 

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
    }
    vec2ds[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< Nf::Vec2d > >(uiElem));
  }

  //Vec2f Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec2fParameter > > vec2fs = collection.GetVec2fParameters();
  for(s32 i=0; i<(s32)vec2fs.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, vec2fs[i]->GetName());


    Nf::SBContainer *sbs = new Nf::SBContainer();
    sbs->nsbs = 2;
    std::tr1::shared_ptr < Nf::Vec2fSlotForwarder > uiElem (new Nf::Vec2fSlotForwarder(vec2fs[i]->GetCallback(), vec2fs[i]->GetContext(), (QObject *)sbs, repainters, NULL));

    const char labels[3][3] = {"x", "y", "z"};
    for(s32 jj=0; jj<2; jj++) {
      QTreeWidgetItem *grandChild = new QTreeWidgetItem();
      child->addChild(grandChild);
      grandChild->setText(0, labels[jj]);

      QDoubleSpinBox *sb = new QDoubleSpinBox(m_params);
      sb->setDecimals(50);
      sb->setMinimum(EL_VALUE2(vec2fs[i]->GetMin(),jj));
      sb->setMaximum(EL_VALUE2(vec2fs[i]->GetMax(),jj));
      sb->setSingleStep(EL_VALUE2(vec2fs[i]->GetStep(),jj));
      sb->setMaximumWidth(8*8);
      sb->setValue(EL_VALUE2(vec2fs[i]->GetValue(),jj));
      sbs->obj[jj] = (QObject *)sb;

      m_params->setItemWidget(grandChild, 1, sb); 

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
    }
    vec2fs[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< Nf::Vec2f > >(uiElem));
  }

  //Vec2i Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec2iParameter > > vec2is = collection.GetVec2iParameters();
  for(s32 i=0; i<(s32)vec2is.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, vec2is[i]->GetName());


    Nf::SBContainer *sbs = new Nf::SBContainer();
    sbs->nsbs = 2;
    std::tr1::shared_ptr < Nf::Vec2iSlotForwarder > uiElem (new Nf::Vec2iSlotForwarder(vec2is[i]->GetCallback(), vec2is[i]->GetContext(), (QObject *)sbs, repainters, NULL));

    const char labels[3][3] = {"x", "y", "z"};
    for(s32 jj=0; jj<2; jj++) {
      QTreeWidgetItem *grandChild = new QTreeWidgetItem();
      child->addChild(grandChild);
      grandChild->setText(0, labels[jj]);

      QSpinBox *sb = new QSpinBox(m_params);
      sb->setMinimum(EL_VALUE2(vec2is[i]->GetMin(),jj));
      sb->setMaximum(EL_VALUE2(vec2is[i]->GetMax(),jj));
      sb->setSingleStep(EL_VALUE2(vec2is[i]->GetStep(),jj));
      sb->setMaximumWidth(8*8);
      sb->setValue(EL_VALUE2(vec2is[i]->GetValue(),jj));
      sbs->obj[jj] = (QObject *)sb;

      m_params->setItemWidget(grandChild, 1, sb); 

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(int)), (QObject *)&(*uiElem), SLOT(changeForward()));
    }
    vec2is[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< Nf::Vec2i > >(uiElem));
  }

  //Enum Parameters
  std::vector < std::tr1::shared_ptr < Nf::EnumParameter > > enums = collection.GetEnumParameters();
  for(s32 i=0; i<(s32)enums.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, enums[i]->GetName());

    QComboBox *combo = new QComboBox(m_params);
    const QMetaObject &mo = Nf::QtEnums::staticMetaObject;
    QMetaEnum meta = mo.enumerator(mo.indexOfEnumerator(enums[i]->GetEnumName()));
    s32 maxLen = 0;
    s32 curKy = -1;
    for(s32 ky=0; ky<meta.keyCount(); ky++) {
      combo->insertItem(ky, meta.key(ky));
      s32 len = strlen(meta.key(ky));
      maxLen = len > maxLen ? len : maxLen;
      if(meta.value(ky) == enums[i]->GetValue())
        curKy = ky;
    }
    combo->setMaximumWidth(maxLen*8);     //multiply strlen by 8 to get right width of the combo box
    combo->setCurrentIndex(curKy);

    std::tr1::shared_ptr < Nf::EnumSlotForwarder > uiElem (new Nf::EnumSlotForwarder(enums[i]->GetEnumName(),
      enums[i]->GetCallback(), enums[i]->GetContext(), (QObject *)combo, repainters, NULL));
    m_params->setItemWidget(child, 1, combo); 
    enums[i]->SetUIElement(std::tr1::shared_ptr< Nf::UIElement < s32 > >(uiElem));
    QObject::connect((QObject *)combo, SIGNAL(currentIndexChanged(int)), (QObject *)&(*uiElem), SLOT(changeForward()), Qt::QueuedConnection);
  }

  //Child parameters
  std::vector < Nf::ParameterCollection * > colls = collection.GetChildCollections();
  for(s32 i=0; i<(s32)colls.size(); i++) {
    QTreeWidgetItem * root = new QTreeWidgetItem(parent);
    root->setText(0, colls[i]->GetName());
    CreateUIElements(root, *colls[i], repainters);
  }
}

void RTUltrasteer::onSaveParameters()
{
  QString filename = QFileDialog::getSaveFileName(this, "Save Parameters", BASE_PATH_CAT(""));

  if(filename.length() == 0)
    return;

  FILE *f = fopen(filename.toStdString().c_str(), "w");

  //Find the dock currently visible
  for(std::map<std::string, Nf::DockWidgetInfo >::iterator i=m_roots.begin(); i!=m_roots.end(); i++) {
    if(i->second.param->GetValue()) {
      try {
        SaveSettings(i->second.collection, f, "");
      } catch(std::runtime_error &e) {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
      }
      break;
    }
  }

  if(f != NULL)
    fclose(f);
}

void RTUltrasteer::onLoadParameters()
{
  QString filename = QFileDialog::getOpenFileName(this, "Load Parameters", BASE_PATH_CAT(""));
  if(filename.length() == 0)
    return;

  FILE *f = fopen(filename.toStdString().c_str(), "r");

  //Find the dock currently visible
  for(std::map<std::string, Nf::DockWidgetInfo >::iterator i=m_roots.begin(); i!=m_roots.end(); i++) {
    if(i->second.param->GetValue()) {
      try {
        LoadSettings(i->second.collection, f);
      } catch(std::runtime_error &e) {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
      }
      break;
    }
  }

  fclose(f);
}


void RTUltrasteer::SaveSettings(Nf::ParameterCollection *collection, FILE *f, const char *nameAppend)
{
  if(f == NULL)
    throw std::runtime_error("Invalid settings file\n");
  std::string appended;
  if(strlen(nameAppend) > 0)
    appended = std::string(nameAppend) + "++" + collection->GetName();
  else
    appended = collection->GetName();

  nameAppend = appended.c_str();

  std::vector < std::tr1::shared_ptr < Nf::FileParameter > > ofiles = collection->GetOpenFileParameters();
  for(s32 i=0; i<(s32)ofiles.size(); i++) {
    fprintf(f, "%s++%s->OpenFile== %s\n", nameAppend, ofiles[i]->GetName(), ofiles[i]->GetValue().c_str());
  }

  //File Open Parameters
  std::vector < std::tr1::shared_ptr < Nf::FileParameter > > sfiles = collection->GetSaveFileParameters();
  for(s32 i=0; i<(s32)sfiles.size(); i++) {
    fprintf(f, "%s++%s->SaveFile== %s\n", nameAppend, sfiles[i]->GetName(), sfiles[i]->GetValue().c_str());
  }

  //Bool Parameters
  std::vector < std::tr1::shared_ptr < Nf::BoolParameter > > bools = collection->GetBoolParameters();
  for(s32 i=0; i<(s32)bools.size(); i++) {
    fprintf(f, "%s++%s->Bool== %d\n", nameAppend, bools[i]->GetName(), bools[i]->GetValue());
  }

  //Int Parameters
  std::vector < std::tr1::shared_ptr < Nf::IntParameter > > ints = collection->GetIntParameters();
  for(s32 i=0; i<(s32)ints.size(); i++) {
    fprintf(f, "%s++%s->Int== %d\n", nameAppend, ints[i]->GetName(), ints[i]->GetValue());
  }

  //Float Parameters
  std::vector < std::tr1::shared_ptr < Nf::FloatParameter > > floats = collection->GetFloatParameters();
  for(s32 i=0; i<(s32)floats.size(); i++) {
    fprintf(f, "%s++%s->Float== %f\n", nameAppend, floats[i]->GetName(), floats[i]->GetValue());
  }

  //Vec3d Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec3dParameter > > vec3ds = collection->GetVec3dParameters();
  for(s32 i=0; i<(s32)vec3ds.size(); i++) {
    fprintf(f, "%s++%s->Vec3d== %f %f %f\n", nameAppend, vec3ds[i]->GetName(), vec3ds[i]->GetValue().x, vec3ds[i]->GetValue().y, vec3ds[i]->GetValue().z);
  }

  //Vec3f Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec3fParameter > > vec3fs = collection->GetVec3fParameters();
  for(s32 i=0; i<(s32)vec3fs.size(); i++) {
    fprintf(f, "%s++%s->Vec3f== %f %f %f\n", nameAppend, vec3fs[i]->GetName(), vec3fs[i]->GetValue().x, vec3fs[i]->GetValue().y, vec3fs[i]->GetValue().z);
  }

  //Vec3i Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec3iParameter > > vec3is = collection->GetVec3iParameters();
  for(s32 i=0; i<(s32)vec3is.size(); i++) {
    fprintf(f, "%s++%s->Vec3i== %d %d %d\n", nameAppend, vec3is[i]->GetName(), vec3is[i]->GetValue().x, vec3is[i]->GetValue().y, vec3is[i]->GetValue().z);
  }

  //Vec2d Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec2dParameter > > vec2ds = collection->GetVec2dParameters();
  for(s32 i=0; i<(s32)vec2ds.size(); i++) {
    fprintf(f, "%s++%s->Vec2d== %f %f\n", nameAppend, vec2ds[i]->GetName(), vec2ds[i]->GetValue().x, vec2ds[i]->GetValue().y);
  }

  //Vec2f Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec2fParameter > > vec2fs = collection->GetVec2fParameters();
  for(s32 i=0; i<(s32)vec2fs.size(); i++) {
    fprintf(f, "%s++%s->Vec2f== %f %f\n", nameAppend, vec2fs[i]->GetName(), vec2fs[i]->GetValue().x, vec2fs[i]->GetValue().y);
  }

  //Vec2i Parameters
  std::vector < std::tr1::shared_ptr < Nf::Vec2iParameter > > vec2is = collection->GetVec2iParameters();
  for(s32 i=0; i<(s32)vec2is.size(); i++) {
    fprintf(f, "%s++%s->Vec2i== %d %d\n", nameAppend, vec2is[i]->GetName(), vec2is[i]->GetValue().x, vec2is[i]->GetValue().y);
  }

  //Enum Parameters
  std::vector < std::tr1::shared_ptr < Nf::EnumParameter > > enums = collection->GetEnumParameters();
  for(s32 i=0; i<(s32)enums.size(); i++) {
    fprintf(f, "%s++%s->Enum== %d\n", nameAppend, enums[i]->GetName(), enums[i]->GetValue());
  }

  //Child parameters
  std::vector < Nf::ParameterCollection * > colls = collection->GetChildCollections();
  for(s32 i=0; i<(s32)colls.size(); i++) {
    SaveSettings(colls[i], f, nameAppend);
  }
}

void RTUltrasteer::SetValue(Nf::ParameterCollection *collection, std::string line)
{
  std::string collName = line.substr(0, line.find_first_of("++"));
  std::string paramName = line.substr(line.find_first_of("++")+2, line.npos);

  if(paramName.find_first_of("++") == paramName.npos) {
    //We're done descending into child nodes 
    char inputStrings[3][200] = {0};

    strcpy(inputStrings[0], paramName.substr(0, paramName.find_first_of("->")).c_str());
    strcpy(inputStrings[1], paramName.substr(paramName.find_first_of("->")+2, paramName.find_first_of("==")-paramName.find_first_of("->")-2).c_str());
    strcpy(inputStrings[2], paramName.substr(paramName.find_first_of("==")+3, paramName.npos).c_str());

    if(strcmpi("OpenFile", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::FileParameter > > ofiles = collection->GetOpenFileParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)ofiles.size(); i++) {
        if(strcmpi(ofiles[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      if(idx < 0)
        throw std::runtime_error("Cannot parses string\n");

      std::string temp = inputStrings[2];
      temp = temp.substr(0,temp.length()-1);
      ofiles[idx]->SetValue(temp);
      //ofiles[idx]->GetCallback()(ofiles[idx]->GetContext());
    } else if(strcmpi("SaveFile", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::FileParameter > > sfiles = collection->GetSaveFileParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)sfiles.size(); i++) {
        if(strcmpi(sfiles[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      if(idx < 0)
        throw std::runtime_error("Cannot parses string\n");
      
      std::string temp = inputStrings[2];
      temp = temp.substr(0,temp.length()-1);
      sfiles[idx]->SetValue(temp);
      //sfiles[idx]->GetCallback()(sfiles[idx]->GetContext());
    } else if(strcmpi("Bool", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::BoolParameter > > bparams = collection->GetBoolParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)bparams.size(); i++) {
        if(strcmpi(bparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      s32 val = 0;
      if(idx < 0 || sscanf(inputStrings[2], "%d", &val) != 1)
        throw std::runtime_error("Cannot parses string\n");

      bparams[idx]->SetValue((bool)val);
      //bparams[idx]->GetCallback()(bparams[idx]->GetContext());
    } else if(strcmpi("Int", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::IntParameter > > iparams = collection->GetIntParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)iparams.size(); i++) {
        if(strcmpi(iparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      s32 val = 0;
      if(idx < 0 || sscanf(inputStrings[2], "%d", &val) != 1)
        throw std::runtime_error("Cannot parses string\n");

      iparams[idx]->SetValue(val);
      //iparams[idx]->GetCallback()(iparams[idx]->GetContext());
    } else if(strcmpi("Float", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::FloatParameter > > fparams = collection->GetFloatParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)fparams.size(); i++) {
        if(strcmpi(fparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      f32 val = 0;
      if(idx < 0 || sscanf(inputStrings[2], "%f", &val) != 1)
        throw std::runtime_error("Cannot parses string\n");

      fparams[idx]->SetValue(val);
      //fparams[idx]->GetCallback()(fparams[idx]->GetContext());
    } else if(strcmpi("Vec3d", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::Vec3dParameter > > vparams = collection->GetVec3dParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)vparams.size(); i++) {
        if(strcmpi(vparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      Vec3d val(0,0,0);
      if(idx < 0 || sscanf(inputStrings[2], "%lf %lf %lf", &val.x, &val.y, &val.z) != 3)
        throw std::runtime_error("Cannot parses string\n");

      vparams[idx]->SetValue(val);
      //vparams[idx]->GetCallback()(vparams[idx]->GetContext());
    } else if(strcmpi("Vec3f", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::Vec3fParameter > > vparams = collection->GetVec3fParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)vparams.size(); i++) {
        if(strcmpi(vparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      Vec3f val(0,0,0);
      if(idx < 0 || sscanf(inputStrings[2], "%f %f %f", &val.x, &val.y, &val.z) != 3)
        throw std::runtime_error("Cannot parses string\n");

      vparams[idx]->SetValue(val);
      //vparams[idx]->GetCallback()(vparams[idx]->GetContext());
    } else if(strcmpi("Vec3i", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::Vec3iParameter > > vparams = collection->GetVec3iParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)vparams.size(); i++) {
        if(strcmpi(vparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      Vec3i val(0,0,0);
      if(idx < 0 || sscanf(inputStrings[2], "%d %d %d", &val.x, &val.y, &val.z) != 3)
        throw std::runtime_error("Cannot parses string\n");

      vparams[idx]->SetValue(val);
      //vparams[idx]->GetCallback()(vparams[idx]->GetContext());
    } else if(strcmpi("Vec2d", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::Vec2dParameter > > vparams = collection->GetVec2dParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)vparams.size(); i++) {
        if(strcmpi(vparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      Vec2d val(0,0);
      if(idx < 0 || sscanf(inputStrings[2], "%lf %lf", &val.x, &val.y) != 2)
        throw std::runtime_error("Cannot parses string\n");

      vparams[idx]->SetValue(val);
      //vparams[idx]->GetCallback()(vparams[idx]->GetContext());
    } else if(strcmpi("Vec2f", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::Vec2fParameter > > vparams = collection->GetVec2fParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)vparams.size(); i++) {
        if(strcmpi(vparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      Vec2f val(0,0);
      if(idx < 0 || sscanf(inputStrings[2], "%f %f", &val.x, &val.y) != 2)
        throw std::runtime_error("Cannot parses string\n");

      vparams[idx]->SetValue(val);
      //vparams[idx]->GetCallback()(vparams[idx]->GetContext());
    } else if(strcmpi("Vec2i", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::Vec2iParameter > > vparams = collection->GetVec2iParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)vparams.size(); i++) {
        if(strcmpi(vparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      Vec2i val(0,0);
      if(idx < 0 || sscanf(inputStrings[2], "%d %d", &val.x, &val.y) != 2)
        throw std::runtime_error("Cannot parses string\n");

      vparams[idx]->SetValue(val);
      //vparams[idx]->GetCallback()(vparams[idx]->GetContext());
    } else if(strcmpi("Enum", inputStrings[1]) == 0) {
      std::vector < std::tr1::shared_ptr < Nf::EnumParameter > > eparams = collection->GetEnumParameters();
      s32 idx = -1;
      for(s32 i=0; i<(s32)eparams.size(); i++) {
        if(strcmpi(eparams[i]->GetName(), inputStrings[0]) == 0) {
          idx = i;
          break;
        }
      }

      s32 val = 0;
      if(idx < 0 || sscanf(inputStrings[2], "%d", &val) != 1)
        throw std::runtime_error("Cannot parses string\n");

      eparams[idx]->SetValue(val);
      //eparams[idx]->GetCallback()(eparams[idx]->GetContext());
    }
  } else {
    collName = paramName.substr(0, paramName.find_first_of("++"));

    std::vector < Nf::ParameterCollection * > colls = collection->GetChildCollections();
    s32 idx = -1;
    for(s32 i=0; i<colls.size(); i++) {
      if(strcmpi(colls[i]->GetName(), collName.c_str()) == 0) {
        idx = i;
        break;
      }
    }
    if(idx == -1)
      throw std::runtime_error("Unable to find child collection\n");

    SetValue(colls[idx], paramName);
  }
}

void RTUltrasteer::LoadSettings(Nf::ParameterCollection *collection, FILE *f)
{
  s32 maxCount = 1000;
  char *line = new char[maxCount];

  //Read line by line
  while(fgets(line, maxCount, f) != NULL) {
    std::string ln = line;
    SetValue(collection, ln);
  }

  delete line;

}

void RTUltrasteer::CreateUSVisualizer()
{
  m_usDock = new QDockWidget(tr("Ultrasound Volume Visualizer"), this);
  m_usDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_usVis = new USVisualizerFullRP(NULL);
  m_usVis->Initialize();
  m_usDock->setWidget(m_usVis);
  
  m_usDock->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  m_usVis->addAction(m_usDock->toggleViewAction());
}

void RTUltrasteer::CreateRPFileDock()
{
  m_rpFileDock = new QDockWidget(tr("RPFileDock"), this);
  m_rpFileDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_rpFileWidget = new Nf::RPFileWidget(m_rpFileDock);
  m_rpFileDock->setWidget(m_rpFileWidget);
  
  m_rpFileDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateRPStreamingDock()
{
  m_rpStreamingDock = new QDockWidget(tr("RPStreamingDock"), this);
  m_rpStreamingDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_rpStreamingWidget = new Nf::RPStreamingWidget(m_rpStreamingDock);
  m_rpStreamingDock->setWidget(m_rpStreamingWidget);
  
  m_rpStreamingDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateEstimatorFileDock()
{
  m_estimatorFileDock = new QDockWidget(tr("Particle Filter"), this);
  m_estimatorFileDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_estimatorFileWidget = new Nf::EstimatorFileWidget(m_estimatorFileDock);
  m_estimatorFileDock->setWidget(m_estimatorFileWidget);
  
  m_estimatorFileDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateEstimatorStreamingDock()
{
  m_estimatorStreamingDock = new QDockWidget(tr("Particle Filter"), this);
  m_estimatorStreamingDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_estimatorStreamingWidget = new Nf::EstimatorStreamingWidget(m_estimatorStreamingDock);
  m_estimatorStreamingDock->setWidget(m_estimatorStreamingWidget);

  m_estimatorStreamingWidget->SetRobot(&m_robot);
  
  m_estimatorStreamingDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateRobotHWDock()
{
  m_robotHWDock = new QDockWidget(tr("RobotHWDock"), this);
  m_robotHWDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_robotHWWidget = new Nf::RobotHardwareWidget(m_robotHWDock);
  m_robotHWWidget->setRobot(&m_robot);
  m_robotHWDock->setWidget(m_robotHWWidget);

  m_robotHWDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateTeleoperation2DFileDock()
{
  m_teleoperation2DFileDock = new QDockWidget(tr("2DTeleoperationFileDock"), this);
  m_teleoperation2DFileDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_teleoperation2DFileWidget = new Nf::Teleoperation2DFileWidget(m_teleoperation2DFileDock, &m_robot, &m_control, &m_mouse);
  m_teleoperation2DFileDock->setWidget(m_teleoperation2DFileWidget);
      
  m_teleoperation2DFileDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateTeleoperation2DStreamDock()
{
  m_teleoperation2DStreamDock = new QDockWidget(tr("2DTeleoperationStreamingDock"), this);
  m_teleoperation2DStreamDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_teleoperation2DStreamWidget = new Nf::Teleoperation2DStreamingWidget(m_teleoperation2DStreamDock, &m_robot, &m_control, &m_mouse);
  m_teleoperation2DStreamDock->setWidget(m_teleoperation2DStreamWidget);
      
  m_teleoperation2DStreamDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}



void RTUltrasteer::CreateEMCalibrationFileDock()
{
  m_EMCalibrationFileDock = new QDockWidget(tr("EMCalibrationFileDock"), this);
  m_EMCalibrationFileDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_EMCalibrationFileWidget = new Nf::EMCalibrationFileWidget(m_EMCalibrationFileDock);
  m_EMCalibrationFileDock->setWidget(m_EMCalibrationFileWidget);

  m_EMCalibrationFileDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateEMCalibrationStreamDock()
{
  m_EMCalibrationStreamDock = new QDockWidget(tr("EMCalibrationStreamingDock"), this);
  m_EMCalibrationStreamDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_EMCalibrationStreamWidget = new Nf::EMCalibrationStreamingWidget(m_EMCalibrationStreamDock);
  m_EMCalibrationStreamDock->setWidget(m_EMCalibrationStreamWidget);
    
  m_EMCalibrationStreamDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}



void RTUltrasteer::CreateFSExperimentFileDock()
{
  m_FSExperimentFileDock = new QDockWidget(tr("FSExperimentFile"), this);
  m_FSExperimentFileDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_fsExperimentFileWidget = new Nf::FSExperimentFileWidget(m_FSExperimentFileDock);
  m_FSExperimentFileDock->setWidget(m_fsExperimentFileWidget);

  m_FSExperimentFileDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateFSExperimentStreamDock()
{
  m_FSExperimentStreamDock = new QDockWidget(tr("FSExperimentStreaming"), this);
  m_FSExperimentStreamDock->setAllowedAreas(Qt::AllDockWidgetAreas);
  
  m_fsExperimentStreamWidget = new Nf::FSExperimentStreamingWidget(m_FSExperimentStreamDock);
  m_FSExperimentStreamDock->setWidget(m_fsExperimentStreamWidget);

  m_fsExperimentStreamWidget->SetRobot(&m_robot);
    
  m_FSExperimentStreamDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateFSSimpleStreamDock()
{
  m_FSSimpleStreamDock = new QDockWidget(tr("FSSimpleStreaming"), this);
  m_FSSimpleStreamDock->setAllowedAreas(Qt::AllDockWidgetAreas);
  
  m_fsSimpleStreamWidget = new Nf::FSSimpleWidget(m_FSExperimentStreamDock);
  m_FSSimpleStreamDock->setWidget(m_fsSimpleStreamWidget);
    
  m_FSExperimentStreamDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateVineWidget()
{
  m_vineWidgetDock = new QDockWidget(tr("VineWidget"), this);
  m_vineWidgetDock->setAllowedAreas(Qt::AllDockWidgetAreas);
  
  m_vineWidget = new Nf::VineWidget(m_vineWidgetDock);
  m_vineWidgetDock->setWidget(m_vineWidget);
    
  m_vineWidgetDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}

void RTUltrasteer::CreateMenuDock()
{
  m_params = new QTreeWidget(m_menu);
  m_params->setColumnCount(2);
  m_params->setHeaderLabels(QStringList() << "Parameter" << "Value");

  m_params->setColumnWidth(0, 170);
  m_params->setColumnWidth(1, 190);
  
  m_menu = new QDockWidget(tr("Menu"), this);
  m_menu->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_menu->setWidget(m_params);
  m_menu->setMinimumWidth(370);
  
  addDockWidget(Qt::LeftDockWidgetArea, m_menu);
  m_menu->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
}
RTUltrasteer::~RTUltrasteer()
{
  if(m_params)
    delete m_params;
  if(m_menu)
    delete m_menu;
  if(m_usVis)
    delete m_usVis;
  if(m_usDock)
    delete m_usDock;
  if(m_rpFileDock)
    delete m_rpFileDock;
  if(m_rpStreamingDock)
    delete m_rpStreamingDock;
  if(m_estimatorFileDock)
    delete m_estimatorFileDock;
  if(m_estimatorStreamingDock)
    delete m_estimatorStreamingDock;
  if(m_teleoperation2DFileDock)
    delete m_teleoperation2DFileDock;
  if(m_teleoperation2DStreamDock)
    delete m_teleoperation2DStreamDock;
  if(m_EMCalibrationFileDock)
    delete m_EMCalibrationFileDock;
  if(m_EMCalibrationStreamDock)
    delete m_EMCalibrationStreamDock;
}
