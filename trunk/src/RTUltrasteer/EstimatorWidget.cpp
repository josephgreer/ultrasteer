#include "EstimatorWidget.h"

namespace Nf
{
  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
  {
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  EstimatorStreamingWidget::EstimatorStreamingWidget(QWidget *parent)
    : RPStreamingWidget(parent, (USVisualizer *)new PFVisualizer(parent))
  {
  }

  EstimatorStreamingWidget::~EstimatorStreamingWidget()
  {
  }
}