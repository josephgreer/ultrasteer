#ifndef RESIZABLE_H
#define RESIZABLE_H

#include <QTimer>
#include <QMetaEnum>
#include <QSize>
#include "UICore.h"
#include <QVTKWidget.h>

namespace Nf
{
#if 0
  class Resizable
  {
  public:
    virtual void UpdateSize(QSize sz) = 0;
  };
#endif

  class ResizableQObject 
  {
  public:
    virtual void UpdateGeometry() = 0;
    virtual void Connect(QObject *signaler, const char *signal, const char *slot) = 0;
  };

  class Resizable : public virtual ResizableQObject
  {
  protected: 
    QSize m_sz;
    QSize m_minSz;
    QTimer *m_resizeTimer;
    QTimer *m_resizeTimer2;

  public:
    Resizable(QSize sz);

    virtual void UpdateSize(QSize sz);
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    virtual void onResize();
    virtual void onResize2();
  };

  class ResizableQVTKWidget : public QVTKWidget, public Resizable
  {
    Q_OBJECT

  protected: 

  public:
    ResizableQVTKWidget(QWidget *parent, QSize sz);
    virtual void UpdateGeometry();
    virtual void Connect(QObject *signaler, const char *signal, const char *slot);
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

  public slots:
    virtual void onResize();
    virtual void onResize2();
  };

  class ResizableQWidget : public QWidget, public Resizable
  {
    Q_OBJECT

  protected: 

  public:
    ResizableQWidget(QWidget *parent, QSize sz);
    virtual void UpdateGeometry();
    virtual void Connect(QObject *signaler, const char *signal, const char *slot);
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

  public slots:
    virtual void onResize();
    virtual void onResize2();
  };


}

#endif // RESIZABLE_H
