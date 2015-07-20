#include "Resizable.h"

namespace Nf
{
  Resizable::Resizable(QSize sz)
    : m_resizeTimer(NULL)
    , m_resizeTimer2(NULL)
    , m_minSz(sz)
    , m_sz(sz)
  {
  }

  void Resizable::UpdateSize(QSize sz)
  {
    m_sz = sz;
    m_minSz = sz;

    if(m_resizeTimer) {
      m_resizeTimer->stop();
    } else {
      m_resizeTimer = new QTimer();
    }
    m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(30);
    Connect(m_resizeTimer, SIGNAL(timeout()), SLOT(onResize()));
    m_resizeTimer->start();
  }

  QSize Resizable::sizeHint() const
  {
    return m_sz;
  }

  QSize Resizable::minimumSizeHint() const
  {
    return m_minSz;
  }

  void Resizable::onResize()
  {
    this->UpdateGeometry();

    delete m_resizeTimer;
    m_resizeTimer = NULL;

    if(m_resizeTimer2) {
      m_resizeTimer2->stop();
    } else {
      m_resizeTimer2 = new QTimer();
    }
    m_resizeTimer2->setSingleShot(true);
    m_resizeTimer2->setInterval(1000);
    Connect(m_resizeTimer2, SIGNAL(timeout()), SLOT(onResize2()));
    m_resizeTimer2->start();
  }

  void Resizable::onResize2()
  {
    m_minSz = QSize(0,0);
    this->UpdateGeometry();

    delete m_resizeTimer2;
    m_resizeTimer2 = NULL;
  }


  ResizableQVTKWidget::ResizableQVTKWidget(QWidget *parent, QSize sz)
    : Resizable(sz)
    , QVTKWidget(parent)
  {
  }

  void ResizableQVTKWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void ResizableQVTKWidget::Connect(QObject *signaler, const char *signal, const char *slot)
  {
    connect(signaler, signal, this, slot);
  }

  void ResizableQVTKWidget::onResize()
  {
    Resizable::onResize();
  }

  void ResizableQVTKWidget::onResize2()
  {
    Resizable::onResize2();
  }

  QSize ResizableQVTKWidget::sizeHint() const
  {
    return m_sz;
  }

  QSize ResizableQVTKWidget::minimumSizeHint() const
  {
    return m_minSz;
  }

  ResizableQWidget::ResizableQWidget(QWidget *parent, QSize sz)
    : Resizable(sz)
    , QWidget(parent)
  {
  }

  void ResizableQWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void ResizableQWidget::Connect(QObject *signaler, const char *signal, const char *slot)
  {
    connect(signaler, signal, this, slot);
  }

  void ResizableQWidget::onResize()
  {
    Resizable::onResize();
  }

  void ResizableQWidget::onResize2()
  {
    Resizable::onResize2();
  }

  QSize ResizableQWidget::sizeHint() const
  {
    return m_sz;
  }

  QSize ResizableQWidget::minimumSizeHint() const
  {
    return m_minSz;
  }

  ResizableQMainWindow::ResizableQMainWindow(QWidget *parent, QSize sz)
    : Resizable(sz)
    , QMainWindow(parent)
  {
  }

  void ResizableQMainWindow::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void ResizableQMainWindow::Connect(QObject *signaler, const char *signal, const char *slot)
  {
    connect(signaler, signal, this, slot);
  }

  void ResizableQMainWindow::onResize()
  {
    Resizable::onResize();
  }

  void ResizableQMainWindow::onResize2()
  {
    Resizable::onResize2();
  }

  QSize ResizableQMainWindow::sizeHint() const
  {
    return m_sz;
  }

  QSize ResizableQMainWindow::minimumSizeHint() const
  {
    return m_minSz;
  }

  ResizableQFrame::ResizableQFrame(QWidget *parent, QSize sz)
    : Resizable(sz)
    , QFrame(parent)
  {
  }

  void ResizableQFrame::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void ResizableQFrame::Connect(QObject *signaler, const char *signal, const char *slot)
  {
    connect(signaler, signal, this, slot);
  }

  void ResizableQFrame::onResize()
  {
    Resizable::onResize();
  }

  void ResizableQFrame::onResize2()
  {
    Resizable::onResize2();
  }

  QSize ResizableQFrame::sizeHint() const
  {
    return m_sz;
  }

  QSize ResizableQFrame::minimumSizeHint() const
  {
    return m_minSz;
  }
}