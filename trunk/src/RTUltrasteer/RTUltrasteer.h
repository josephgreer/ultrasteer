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
  class BoolSlotForwarder : public QObject, public UIElement < bool >
  {
    Q_OBJECT

  private:

    QTreeWidgetItem *m_element;
    void *m_context;
    Function m_fptr;

  public:

    BoolSlotForwarder(Function function, void *context, QTreeWidgetItem *element, QObject* parent = 0) 
      : QObject(parent)
      , m_fptr(function)
      , m_context(context)
      , m_element(element)
    {}

    bool GetValue()
    {
      return m_element->checkState(1) == Qt::Checked;
    }

    public slots:

      void forward(QTreeWidgetItem *item, int col);
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

    void CreateUIElements(QTreeWidgetItem *parent, Nf::ParameterCollection &collection);

    std::vector < QTreeWidgetItem * > m_roots;
};

#endif // RTULTRASTEER_H
