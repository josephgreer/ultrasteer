#ifndef ESTIMATORWIDGET_H
#define ESTIMATORWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "USVisualizerWidget.h"
#include "RPFileReader.h"
#include "RPProcess.h"
#include "PFVisualizerWidget.h"
#include "RPWidget.h"

namespace Nf
{
  class EstimatorFileWidget : public RPFileWidget
  {
    Q_OBJECT 

  public:
    EstimatorFileWidget(QWidget *parent);
    virtual ~EstimatorFileWidget();
  };

  class EstimatorStreamingWidget : public RPStreamingWidget
  {
    Q_OBJECT 

  public:
    EstimatorStreamingWidget(QWidget *parent);
    virtual ~EstimatorStreamingWidget();

  };
}

#endif // ESTIMATORWIDGET_H
