#include "rtultrasteer.h"

RTUltrasteer::RTUltrasteer(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
    , m_menu(NULL)
    , m_params(NULL)
    , m_usDock(NULL)
    , m_usVis(NULL)
{
  ui.setupUi(this);

  CreateUSVisualizer();
  CreateMenuDock();
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

  m_params = new QListWidget(m_menu);
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
