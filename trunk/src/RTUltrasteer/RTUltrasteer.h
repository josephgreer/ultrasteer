#ifndef RTULTRASTEER_H
#define RTULTRASTEER_H

#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSpinBox>
#include <QTimer>
#include <QMetaEnum>
#include "ui_rtultrasteer.h"
#include "VTKTransferFunctionWidget.h"
#include "UICore.h"
#include "FileWidget.h"
#include "USVisualizerWidget.h"
#include "Resizable.h"

namespace Nf
{
  struct SBContainer{
    QObject *obj[3];
    s32 nsbs;
  }; 

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

  class Vec3dSlotForwarder : public SlotForwarder, public UIElement < Vec3d >
  {
    Q_OBJECT

  public:

    Vec3dSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    Vec3d GetValue()
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];
      return Vec3d((f64)x->value(), (f64)y->value(), (f64)z->value());
    }
  };

  class Vec3fSlotForwarder : public SlotForwarder, public UIElement < Vec3f >
  {
    Q_OBJECT

  public:

    Vec3fSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    Vec3f GetValue()
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];
      return Vec3f((f32)x->value(), (f32)y->value(), (f32)z->value());
    }
  };

  class Vec3iSlotForwarder : public SlotForwarder, public UIElement < Vec3i >
  {
    Q_OBJECT

  public:

    Vec3iSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    Vec3i GetValue()
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      QSpinBox *z = (QSpinBox *)item->obj[2];
      return Vec3i((s32)x->value(), (s32)y->value(), (s32)z->value());
    }
  };

  class Vec2dSlotForwarder : public SlotForwarder, public UIElement < Vec2d >
  {
    Q_OBJECT

  public:

    Vec2dSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    Vec2d GetValue()
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      return Vec2d((f64)x->value(), (f64)y->value());
    }
  };

  class Vec2fSlotForwarder : public SlotForwarder, public UIElement < Vec2f >
  {
    Q_OBJECT

  public:

    Vec2fSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    Vec2f GetValue()
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      return Vec2f((f32)x->value(), (f32)y->value());
    }
  };

  class Vec2iSlotForwarder : public SlotForwarder, public UIElement < Vec2i >
  {
    Q_OBJECT

  public:

    Vec2iSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    Vec2i GetValue()
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      return Vec2i((s32)x->value(), (s32)y->value());
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

  class FileSlotForwarder : public SlotForwarder, public UIElement < std::string >
  {
    Q_OBJECT

  public:

    FileSlotForwarder(Function function, void *context, QObject *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {
    }

    std::string GetValue()
    {
      FileWidget *fw = (FileWidget *)m_element;
      return std::string(fw->GetFilename());
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

    void CreateUSVisualizer();
    void CreateMenuDock();
    void CreateTFDock();
    void resizeEvent(QResizeEvent *event);

    void CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection, const std::vector < QVTKWidget * > & repainters);

    std::vector < QTreeWidgetItem * > m_roots;
};

#endif // RTULTRASTEER_H
