#ifndef RESIZABLE_H
#define RESIZABLE_H

#include <QTimer>
#include <QMetaEnum>
#include <QSize>
#include "UICore.h"
#include <QVTKWidget.h>

namespace Nf
{
  class Resizable
  {
  public:
    virtual void UpdateSize(QSize sz) = 0;
  };

  class ResizableQVTKWidget : public QVTKWidget, public Resizable
  {
    Q_OBJECT 

  protected: 
    QSize m_sz;
    QSize m_minSz;
    QTimer *m_resizeTimer;
    QTimer *m_resizeTimer2;

  public:
    ResizableQVTKWidget(QWidget *parent, QSize sz);

    virtual void UpdateSize(QSize sz);
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

  public slots:
    void onResize();
    void onResize2();
  };
}

#endif // RESIZABLE_H
