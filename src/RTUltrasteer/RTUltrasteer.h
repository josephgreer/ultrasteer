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
#include "Resizable.h"
#include "FileWidget.h"
#include "USVisualizerWidget.h"
#include "RPWidget.h"
#include "RobotHardwareWidget.h"
#include "Teleoperation2DWidget.h"
#include "EMCalibrationWidget.h"
#include "PFVisualizerWidget.h"
#include "EstimatorWidget.h"
#include "ControlAlgorithms.h"
#include "FSExperimentWidget.h"
#include "VineWidget.h"

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

    void SetValue(const bool &val)
    {
      ((QTreeWidgetItem *)m_element)->setCheckState(1, val ? Qt::Checked : Qt::Unchecked);
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

    void SetValue(const bool &val)
    {
      return;
    }
  };

  class IntSlotForwarder : public SlotForwarder, public NumberUIElement < s32 >
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

    void SetValue(const s32 &val)
    {
      QSpinBox *sb = (QSpinBox *)m_element;
      bool old = sb->blockSignals(true);
      sb->setValue(val);
      sb->blockSignals(old);
    }

    void SetMin(s32 min)
    {
      QSpinBox *sb = (QSpinBox *)m_element;
      sb->setMinimum(min);
    }

    void SetMax(s32 max)
    {
      QSpinBox *sb = (QSpinBox *)m_element;
      sb->setMaximum(max);
    }

    void SetStep(s32 step)
    {
      QSpinBox *sb = (QSpinBox *)m_element;
      sb->setSingleStep(step);
    }
  };

  class FloatSlotForwarder : public SlotForwarder, public NumberUIElement < f32 >
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

    void SetValue(const f32 &val)
    {
      QDoubleSpinBox *sb = (QDoubleSpinBox *)m_element;
      bool old = sb->blockSignals(true);
      return sb->setValue(val);
      sb->blockSignals(old);
    }

    void SetMin(f32 min)
    {
      QDoubleSpinBox *sb = (QDoubleSpinBox *)m_element;
      sb->setMinimum(min);
    }

    void SetMax(f32 max)
    {
      QDoubleSpinBox *sb = (QDoubleSpinBox *)m_element;
      sb->setMaximum(max);
    }

    void SetStep(f32 step)
    {
      QDoubleSpinBox *sb = (QDoubleSpinBox *)m_element;
      sb->setSingleStep(step);
    }
  };

  class Vec3dSlotForwarder : public SlotForwarder, public NumberUIElement < Vec3d >
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

    void SetValue(const Vec3d &val)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      bool old = x->blockSignals(true);
      x->setValue(val.x);
      x->blockSignals(old);
      old = y->blockSignals(true);
      y->setValue(val.y);
      y->blockSignals(old);
      old = z->blockSignals(true);
      z->setValue(val.z);
      z->blockSignals(old);
    }

    void SetMin(Vec3d min)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      x->setMinimum(min.x);
      y->setMinimum(min.y);
      z->setMinimum(min.z);
    }

    void SetMax(Vec3d max)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      x->setMaximum(max.x);
      y->setMaximum(max.y);
      z->setMaximum(max.z);
    }

    void SetStep(Vec3d step)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      x->setSingleStep(step.x);
      y->setSingleStep(step.y);
      z->setSingleStep(step.z);
    }
  };

  class Vec3fSlotForwarder : public SlotForwarder, public NumberUIElement < Vec3f >
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

    void SetValue(const Vec3f &val)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      bool old = x->blockSignals(true);
      x->setValue(val.x);
      x->blockSignals(old);
      old = y->blockSignals(true);
      y->setValue(val.y);
      y->blockSignals(old);
      old = z->blockSignals(true);
      z->setValue(val.z);
      z->blockSignals(old);
    }

    void SetMin(Vec3f min)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      x->setMinimum(min.x);
      y->setMinimum(min.y);
      z->setMinimum(min.z);
    }

    void SetMax(Vec3f max)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      x->setMaximum(max.x);
      y->setMaximum(max.y);
      z->setMaximum(max.z);
    }

    void SetStep(Vec3f step)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      QDoubleSpinBox *z = (QDoubleSpinBox *)item->obj[2];

      x->setSingleStep(step.x);
      y->setSingleStep(step.y);
      z->setSingleStep(step.z);
    }
  };

  class Vec3iSlotForwarder : public SlotForwarder, public NumberUIElement < Vec3i >
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

    void SetValue(const Vec3i &val)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      QSpinBox *z = (QSpinBox *)item->obj[2];

      bool old = x->blockSignals(true);
      x->setValue(val.x);
      x->blockSignals(old);
      old = y->blockSignals(true);
      y->setValue(val.y);
      y->blockSignals(old);
      old = z->blockSignals(true);
      z->setValue(val.z);
      z->blockSignals(old);
    }

    void SetMin(Vec3i min)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      QSpinBox *z = (QSpinBox *)item->obj[2];

      x->setMinimum(min.x);
      y->setMinimum(min.y);
      z->setMinimum(min.z);
    }

    void SetMax(Vec3i max)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      QSpinBox *z = (QSpinBox *)item->obj[2];

      x->setMaximum(max.x);
      y->setMaximum(max.y);
      z->setMaximum(max.z);
    }

    void SetStep(Vec3i step)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 3);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      QSpinBox *z = (QSpinBox *)item->obj[2];

      x->setSingleStep(step.x);
      y->setSingleStep(step.y);
      z->setSingleStep(step.z);
    }
  };

  class Vec2dSlotForwarder : public SlotForwarder, public NumberUIElement < Vec2d >
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

    void SetValue(const Vec2d &val)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      
      bool old = x->blockSignals(true);
      x->setValue(val.x);
      x->blockSignals(old);
      old = y->blockSignals(true);
      y->setValue(val.y);
      y->blockSignals(old);
    }

    void SetMin(Vec2d min)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];

      x->setMinimum(min.x);
      y->setMinimum(min.y);
    }

    void SetMax(Vec2d max)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];

      x->setMaximum(max.x);
      y->setMaximum(max.y);
    }

    void SetStep(Vec2d step)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];

      x->setSingleStep(step.x);
      y->setSingleStep(step.y);
    }
  };

  class Vec2fSlotForwarder : public SlotForwarder, public NumberUIElement < Vec2f >
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

    void SetValue(const Vec2f &val)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];
      
      bool old = x->blockSignals(true);
      x->setValue(val.x);
      x->blockSignals(old);
      old = y->blockSignals(true);
      y->setValue(val.y);
      y->blockSignals(old);
    }

    void SetMin(Vec2f min)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];

      x->setMinimum(min.x);
      y->setMinimum(min.y);
    }

    void SetMax(Vec2f max)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];

      x->setMaximum(max.x);
      y->setMaximum(max.y);
    }

    void SetStep(Vec2f step)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QDoubleSpinBox *x = (QDoubleSpinBox *)item->obj[0];
      QDoubleSpinBox *y = (QDoubleSpinBox *)item->obj[1];

      x->setSingleStep(step.x);
      y->setSingleStep(step.y);
    }
  };

  class Vec2iSlotForwarder : public SlotForwarder, public NumberUIElement < Vec2i >
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

    void SetValue(const Vec2i &val)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];
      
      bool old = x->blockSignals(true);
      x->setValue(val.x);
      x->blockSignals(old);
      old = y->blockSignals(true);
      y->setValue(val.y);
      y->blockSignals(old);
    }

    void SetMin(Vec2i min)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];

      x->setMinimum(min.x);
      y->setMinimum(min.y);
    }

    void SetMax(Vec2i max)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];

      x->setMaximum(max.x);
      y->setMaximum(max.y);
    }

    void SetStep(Vec2i step)
    {
      SBContainer *item = (SBContainer *)m_element;
      assert(item->nsbs == 2);
      QSpinBox *x = (QSpinBox *)item->obj[0];
      QSpinBox *y = (QSpinBox *)item->obj[1];

      x->setSingleStep(step.x);
      y->setSingleStep(step.y);
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

    void SetValue(const s32 &val)
    {
      QComboBox *combo = (QComboBox *)m_element;
      bool old = combo->blockSignals(true);
      const QMetaObject &mo = QtEnums::staticMetaObject;
      QMetaEnum meta = mo.enumerator(mo.indexOfEnumerator(m_enumName.c_str()));
      for(s32 ky=0; ky<meta.keyCount(); ky++) {
        if(meta.value(ky) == val) {
          combo->setCurrentIndex(ky);
          break;
        }
      }
      combo->blockSignals(old);
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

    void SetValue(const std::string &val)
    {
      FileWidget *fw = (FileWidget *)m_element;
      fw->SetFilename(val.c_str());
    }
  };
}

namespace Nf 
{
  struct DockWidgetInfo
  {
    QDockWidget *dock;
    BoolParameter *param;
    QTreeWidgetItem *root;
    Resizable *resize;
    ParameterCollection *collection;
  };
}

class RTUltrasteer : public QMainWindow, public Nf::ParameterCollection
{
    Q_OBJECT

public:
    RTUltrasteer(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~RTUltrasteer();

    std::tr1::shared_ptr < Nf::BoolParameter > m_usDockVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_rpStreamingWidgetVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_rpFileWidgetVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_estimatorStreamingWidgetVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_estimatorFileWidgetVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_robotHWWidgetVisible;
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_teleoperation2DFileWidgetVisible;
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_teleoperation2DStreamWidgetVisible;
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_emCalibrationFileWidgetVisible;
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_emCalibrationStreamWidgetVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_fsExperimentFileWidgetVisible;
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_fsExperimentStreamWidgetVisible;

    std::tr1::shared_ptr < Nf::BoolParameter > m_fsSimpleStreamWidgetVisible;
		
    std::tr1::shared_ptr < Nf::BoolParameter > m_vineWidgetVisible;

    void onSetDocksVisible();
    CLASS_CALLBACK(onSetDocksVisible, RTUltrasteer);

    public slots:
      void onSaveParameters();
      void onLoadParameters();


private:
    Ui::RTUltrasteerClass ui;

    QDockWidget *m_usDock;
    USVisualizerFullRP *m_usVis;

    QDockWidget *m_menu;
    QTreeWidget *m_params;

    QDockWidget *m_rpFileDock;
    Nf::RPFileWidget *m_rpFileWidget;

    QDockWidget *m_rpStreamingDock;
    Nf::RPStreamingWidget *m_rpStreamingWidget;

    QDockWidget *m_estimatorFileDock;
    Nf::EstimatorFileWidget *m_estimatorFileWidget;

    QDockWidget *m_estimatorStreamingDock;
    Nf::EstimatorStreamingWidget *m_estimatorStreamingWidget;

    QDockWidget *m_robotHWDock;
    Nf::RobotHardwareWidget *m_robotHWWidget;

    QDockWidget *m_teleoperation2DFileDock;
    Nf::Teleoperation2DFileWidget *m_teleoperation2DFileWidget;

    QDockWidget *m_teleoperation2DStreamDock;
    Nf::Teleoperation2DStreamingWidget *m_teleoperation2DStreamWidget;

    QDockWidget *m_EMCalibrationFileDock;
    Nf::EMCalibrationFileWidget *m_EMCalibrationFileWidget;

    QDockWidget *m_EMCalibrationStreamDock;
    Nf::EMCalibrationStreamingWidget *m_EMCalibrationStreamWidget;

    QDockWidget *m_FSExperimentFileDock;
    Nf::FSExperimentFileWidget *m_fsExperimentFileWidget;

    QDockWidget *m_FSExperimentStreamDock;
    Nf::FSExperimentStreamingWidget *m_fsExperimentStreamWidget;

    QDockWidget *m_FSSimpleStreamDock;
    Nf::FSSimpleWidget *m_fsSimpleStreamWidget;

		QDockWidget *m_vineWidgetDock;
		Nf::VineWidget *m_vineWidget;

    NeedleSteeringRobot m_robot;
    Nf::ControlAlgorithms m_control;
    Mouse3DInput m_mouse;

    void CreateUSVisualizer();
    void CreateMenuDock();
    void CreateRPFileDock();
    void CreateRPStreamingDock();
    void CreateEstimatorFileDock();
    void CreateEstimatorStreamingDock();
    void CreateRobotHWDock();
    void CreateTeleoperation2DFileDock();
    void CreateTeleoperation2DStreamDock();
    void CreateEMCalibrationFileDock();
    void CreateEMCalibrationStreamDock();
    void CreateFSExperimentFileDock();
    void CreateFSExperimentStreamDock();
    void CreateFSSimpleStreamDock();
		void CreateVineWidget();
    void resizeEvent(QResizeEvent *event);
    void Resize();

    void CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection, const std::vector < QVTKWidget * > & repainters);

    void SaveSettings(Nf::ParameterCollection *collection, FILE *f, const char *nameAppend);
    void LoadSettings(Nf::ParameterCollection *collection, FILE *f);
    void SetValue(Nf::ParameterCollection *collection, std::string line);

    //Holds all potential main dock windows
    std::map < std::string, Nf::DockWidgetInfo > m_roots;
};

#endif // RTULTRASTEER_H