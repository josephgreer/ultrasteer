#include "FileWidget.h"

namespace Nf
{
  FileWidget::FileWidget(FileMode fm, const char *dir, const char *filename, const char *extWC, QWidget *parent, Qt::WFlags flags)
    : QFrame(parent, flags)
    , m_fm(fm)
    , m_dir(dir)
    , m_extWC(extWC)
    , m_fname(filename)
  {
    ui.setupUi(this);
    connect(ui.chooseFile, SIGNAL(clicked(bool)), this, SLOT(FileDlgPushed()));
    ui.fnameEdit->setPlainText(m_fname.c_str());
  }

  FileWidget::~FileWidget()
  {
  }

  std::string FileWidget::GetFilename()
  {
    return m_dir+std::string("/")+m_fname;
  }

  void FileWidget::SetFilename(const char *filename)
  {
    std::string fname(filename);
    m_dir = fname.substr(0,fname.find_last_of("/"));
    m_fname = fname.substr(fname.find_last_of("/")+1,std::string::npos);
    ui.fnameEdit->setPlainText(m_fname.c_str());
  }

  void FileWidget::FileDlgPushed()
  {
    if(m_fm == FM_OPEN) {
      QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), m_dir.c_str(), m_extWC.c_str());
      if(fileName.length() > 0) {
        SetFilename(fileName.toStdString().c_str());
        emit FileChanged();
      }
    } else {
    }
  }
}

