#include "RTUltrasteer.h"
#include "FileWidget.h"
#include <QComboBox>

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


RTUltrasteer::RTUltrasteer(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
    , m_menu(NULL)
    , m_params(NULL)
    , m_usDock(NULL)
    , m_usVis(NULL)
    , m_tfDock(NULL)
{
  ui.setupUi(this);

  CreateTFDock();
  CreateUSVisualizer();
  CreateMenuDock();

  QTreeWidgetItem * usVis = new QTreeWidgetItem(m_params);
  usVis->setText(0, m_usVis->GetName());
  std::vector < QVTKWidget * > repainters;
  repainters.push_back(m_usVis);
  CreateUIElements(usVis, *m_usVis, repainters);

  m_roots.push_back(usVis);
  m_params->expandAll();
}

void RTUltrasteer::CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection, const std::vector < QVTKWidget * > & repainters) 
{
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
    sb->setMinimum(floats[i]->GetMin());
    sb->setMaximum(floats[i]->GetMax());
    sb->setSingleStep(floats[i]->GetStep());

    std::tr1::shared_ptr < Nf::FloatSlotForwarder > uiElem (new Nf::FloatSlotForwarder(floats[i]->GetCallback(), floats[i]->GetContext(), (QObject *)sb, repainters, NULL));
    m_params->setItemWidget(child, 1, sb); 
    floats[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< f32 > >(uiElem));
    QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
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
    for(s32 ky=0; ky<meta.keyCount(); ky++) {
      combo->insertItem(ky, meta.key(ky));
    }
    combo->setCurrentIndex(enums[i]->GetValue());

    std::tr1::shared_ptr < Nf::EnumSlotForwarder > uiElem (new Nf::EnumSlotForwarder(enums[i]->GetEnumName(),
      enums[i]->GetCallback(), enums[i]->GetContext(), (QObject *)combo, repainters, NULL));
    m_params->setItemWidget(child, 1, combo); 
    enums[i]->SetUIElement(std::tr1::shared_ptr< Nf::UIElement < s32 > >(uiElem));
    QObject::connect((QObject *)combo, SIGNAL(currentIndexChanged(int)), (QObject *)&(*uiElem), SLOT(changeForward()));
  }

  //Child parameters
  std::vector < Nf::ParameterCollection * > colls = collection.GetChildCollections();
  for(s32 i=0; i<(s32)colls.size(); i++) {
    QTreeWidgetItem * root = new QTreeWidgetItem(parent);
    root->setText(0, colls[i]->GetName());
    CreateUIElements(root, *colls[i], repainters);
  }
}


void RTUltrasteer::CreateUSVisualizer()
{
  assert(m_tfWidget != NULL);
  m_usDock = new QDockWidget(tr("Ultrasound Visualization"), this);
  m_usDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_usVis = new USVisualizerWidget(m_tfWidget->GetColorTransferFunction(), m_tfWidget->GetOpacityTransferFunction());
  m_usVis->Initialize();
  m_usDock->setWidget(m_usVis);
  
  addDockWidget(Qt::RightDockWidgetArea, m_usDock);
  m_usDock->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

  m_tfWidget->SetInteractionObserver(m_usVis);
}

void RTUltrasteer::CreateTFDock()
{
  m_tfDock = new QDockWidget(tr("Transfer Function Editor"), this);
  m_tfDock->setAllowedAreas(Qt::AllDockWidgetAreas);

  m_tfWidget = new VTKTransferFunctionWidget();
  this->m_tfWidget->Initialize();
  m_tfDock->setWidget(m_tfWidget);
  
  addDockWidget(Qt::RightDockWidgetArea, m_tfDock);
  m_tfDock->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
}


void RTUltrasteer::CreateMenuDock()
{
  m_params = new QTreeWidget(m_menu);
  m_params->setColumnCount(2);
  m_params->setHeaderLabels(QStringList() << "Parameter" << "Value");

  m_params->setColumnWidth(0, 130);
  m_params->setColumnWidth(1, 220);
  
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
  if(m_tfWidget)
    delete m_tfWidget;
  if(m_tfDock)
    delete m_tfDock;
}
