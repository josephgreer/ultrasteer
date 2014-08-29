#include "RTUltrasteer.h"

RTUltrasteer::RTUltrasteer(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
    , m_menu(NULL)
    , m_params(NULL)
    , m_usDock(NULL)
    , m_usVis(NULL)
{
  ui.setupUi(this);

  CreateMenuDock();
  CreateUSVisualizer();

  QTreeWidgetItem * usVis = new QTreeWidgetItem(m_params);
  usVis->setText(0, m_usVis->GetName());
  CreateUIElements(usVis, *m_usVis);
  m_roots.push_back(usVis);
}

void RTUltrasteer::CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection) 
{
  std::vector < std::tr1::shared_ptr < Nf::BoolParameter > > bools = collection.GetBoolParameters();
  for(s32 i=0; i<(s32)bools.size(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem();
    parent->addChild(child);
    child->setText(0, bools[i]->GetName());
    child->setText(1, bools[i]->GetValue() ? "true" : "false");

    Nf::BoolSlotForwarder * uiElem = new Nf::BoolSlotForwarder(NULL, NULL, child, NULL);
    //td::tr1::shared_ptr < Nf::BoolSlotForwarder > uiElem (new Nf::BoolSlotForwarder(NULL, NULL, child, NULL));
    //child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
    bools[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< bool > >(uiElem));
    QObject::connect((QObject *)this, SIGNAL(QTreeWidgetItem *, int), (QObject *)uiElem, SLOT(forward()));
  }
}


void RTUltrasteer::CreateUSVisualizer()
{
  m_usDock = new QDockWidget(tr("Ultrasound Visualization"), this);
  m_usDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_usVis = new USVisualizerWidget();
  m_usVis->Initialize();
  m_usDock->setWidget(m_usVis);
  
  addDockWidget(Qt::RightDockWidgetArea, m_usDock);
}

void RTUltrasteer::CreateMenuDock()
{
  m_menu = new QDockWidget(tr("Menu"), this);
  m_menu->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_params = new QTreeWidget(m_menu);
  m_params->setColumnCount(2);
  m_params->setHeaderLabels(QStringList() << "Parameter" << "Value");

  m_menu->setWidget(m_params);
  
  addDockWidget(Qt::LeftDockWidgetArea, m_menu);
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
}
