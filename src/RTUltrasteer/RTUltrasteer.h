#ifndef RTULTRASTEER_H
#define RTULTRASTEER_H

#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSpinBox>
#include <QMetaEnum>
#include "ui_rtultrasteer.h"
#include "VTKTransferFunctionWidget.h"
#include "UICore.h"
#include "USVisualizerWidget.h"

namespace Nf
{
  class SlotForwarder : public QObject
  {
    Q_OBJECT

  protected:
    void *m_context;
    Function m_fptr;
    QObject *m_element;
    std::vector < QVTKWidget * > m_repaintList;

  public:
    SlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject *parent)
      : QObject(parent)
      , m_fptr(function)
      , m_context(context)
      , m_element(element)
      , m_repaintList(repaintList)
    {
    }

    public slots:

      void treeWidgetForward(QTreeWidgetItem *item, int col);
      void changeForward();
  };

  class BoolSlotForwarder : public SlotForwarder, public UIElement < bool >
  {
    Q_OBJECT

  public:

    BoolSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {}

    bool GetValue()
    {
      return ((QTreeWidgetItem *)m_element)->checkState(1) == Qt::Checked;
    }
  };

  class ActionSlotForwarder : public SlotForwarder, public UIElement < bool >
  {
    Q_OBJECT

  public:

    ActionSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {}

    bool GetValue()
    {
      return true;
    }
  };

  class IntSlotForwarder : public SlotForwarder, public UIElement < s32 >
  {
    Q_OBJECT

  public:

    IntSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    s32 GetValue()
    {
      QSpinBox *sb = (QSpinBox *)m_element;
      return (s32)sb->value();
    }
  };

  class FloatSlotForwarder : public SlotForwarder, public UIElement < f32 >
  {
    Q_OBJECT

  public:

    FloatSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    f32 GetValue()
    {
      QDoubleSpinBox *sb = (QDoubleSpinBox *)m_element;
      return (f32)sb->value();
    }
  };

  class EnumSlotForwarder : public SlotForwarder, public UIElement < s32 >
  {
    Q_OBJECT

  protected:
    std::string m_enumName;

  public:

    EnumSlotForwarder(const char *enumName, Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
      m_enumName = enumName;
    }

    s32 GetValue()
    {
      QComboBox *combo = (QComboBox *)m_element;
      const QMetaObject &mo = QtEnums::staticMetaObject;
      QMetaEnum meta = mo.enumerator(mo.indexOfEnumerator(m_enumName.c_str()));
      return meta.keyToValue(combo->currentText().toStdString().c_str());
    }
  };
}

class RTUltrasteer : public QMainWindow
{
    Q_OBJECT

public:
    RTUltrasteer(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~RTUltrasteer();

private:
    Ui::RTUltrasteerClass ui;

    QDockWidget *m_usDock;
    USVisualizerWidget *m_usVis;

    QDockWidget *m_menu;
    QTreeWidget *m_params;

    QDockWidget *m_tfDock;
    VTKTransferFunctionWidget *m_tfWidget;
    //TransferFunctionWidget *m_tfWidget;

    void CreateUSVisualizer();
    void CreateMenuDock();
    void CreateTFDock();

    void CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection, const std::vector < QVTKWidget * > & repainters);

    std::vector < QTreeWidgetItem * > m_roots;
};

#endif // RTULTRASTEER_H
