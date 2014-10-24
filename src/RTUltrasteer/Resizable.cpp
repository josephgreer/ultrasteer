#include "Resizable.h"

namespace Nf
{
  Resizable::Resizable(QWidget *parent, QSize sz)
    : QVTKWidget(parent)
    , m_resizeTimer(NULL)
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
    m_resizeTimer->setInterval(100);
    connect(m_resizeTimer, SIGNAL(timeout()), this, SLOT(onResize()));
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
    this->updateGeometry();

    delete m_resizeTimer;
    m_resizeTimer = NULL;

    if(m_resizeTimer2) {
      m_resizeTimer2->stop();
    } else {
      m_resizeTimer2 = new QTimer();
    }
    m_resizeTimer2->setSingleShot(true);
    m_resizeTimer2->setInterval(100);
    connect(m_resizeTimer2, SIGNAL(timeout()), this, SLOT(onResize2()));
    m_resizeTimer2->start();
  }

  void Resizable::onResize2()
  {
    m_minSz = QSize(0,0);
    this->updateGeometry();

    delete m_resizeTimer2;
    m_resizeTimer2 = NULL;
  }
}