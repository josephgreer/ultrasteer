#ifndef RTULTRASTEER_H
#define RTULTRASTEER_H

#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidget>
#include "ui_rtultrasteer.h"
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
    QTreeWidgetItem *m_element;
    std::vector < QVTKWidget * > m_repaintList;

  public:
    SlotForwarder(Function function, void *context, QTreeWidgetItem *element, std::vector < QVTKWidget * > repaintList, QObject *parent)
      : QObject(parent)
      , m_fptr(function)
      , m_context(context)
      , m_element(element)
      , m_repaintList(repaintList)
    {
    }

    public slots:

      void forward(QTreeWidgetItem *item, int col);
  };

  class BoolSlotForwarder : public SlotForwarder, public UIElement < bool >
  {
    Q_OBJECT

  public:

    BoolSlotForwarder(Function function, void *context, QTreeWidgetItem *element, std::vector < QVTKWidget * > repaintList, QObject* parent = 0) 
      : SlotForwarder(function, context, element, repaintList, parent)
    {}

    bool GetValue()
    {
      return m_element->checkState(1) == Qt::Checked;
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

    void CreateUSVisualizer();
    void CreateMenuDock();

    void CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection, const std::vector < QVTKWidget * > & repainters);

    std::vector < QTreeWidgetItem * > m_roots;
};

#endif // RTULTRASTEER_H