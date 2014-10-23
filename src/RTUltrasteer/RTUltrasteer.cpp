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
    ofiles[i]->SetUIElement(std::tr1::shared_ptr<Nf::UIElement< std::string > >(uiElem));
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

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
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

      QObject::connect((QObject *)sb, SIGNAL(valueChanged(double)), (QObject *)&(*uiElem), SLOT(changeForward()));
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
    for(s32 ky=0; ky<meta.keyCount(); ky++) {
      combo->insertItem(ky, meta.key(ky));
      s32 len = strlen(meta.key(ky));
      maxLen = len > maxLen ? len : maxLen;
    }
    combo->setMaximumWidth(maxLen*8);     //multiply strlen by 8 to get right width of the combo box
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
  if(m_tfWidget)
    delete m_tfWidget;
  if(m_tfDock)
    delete m_tfDock;
}
