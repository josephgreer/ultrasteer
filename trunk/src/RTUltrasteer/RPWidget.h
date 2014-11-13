#ifndef RPWIDGET_H
#define RPWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "USVisualizerWidget.h"
#include "RPFileReader.h"

namespace Nf
{
  class RPWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < ImageViewerWidget> m_imageViewer;
    std::tr1::shared_ptr < USVisualizer > m_usVis;
    RPFileReaderCollection *m_rpReaders;
    RPData m_data;
    QGridLayout *m_layout;

  public:
    RPWidget(QWidget *parent);
    ~RPWidget();

    //Filename
    std::tr1::shared_ptr < Nf::FileParameter > m_rpFile;
    void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, RPWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    void onUpdateFrame();
    CLASS_CALLBACK(onUpdateFrame, RPWidget);

    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();
  };
}

#endif // RPWIDGET_H
