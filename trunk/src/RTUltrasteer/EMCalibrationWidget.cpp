#include "EMCalibrationWidget.h"


namespace Nf
{
  EMCalibrationWidget::EMCalibrationWidget(QWidget *parent) 
    : Nf::ParameterCollection("EM Calibration")
    , ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , m_stylusInit(false)
    , m_inRobotCalibration(false)
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    m_stylusPoints = vtkSmartPointer<vtkPoints>::New();
    m_polyData = vtkSmartPointer<vtkPolyData>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    m_glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();

    ADD_BOOL_PARAMETER(m_inStylusCalibration, "Calibrate Stylus", CALLBACK_POINTER(onCalibrateStylus, EMCalibrationWidget), this, false);
    
  }

  EMCalibrationWidget::~EMCalibrationWidget()
  {
  }

  void EMCalibrationWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void EMCalibrationWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQVTKWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > EMCalibrationWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }

  void EMCalibrationWidget::onCalibrateStylus(void)
  {
    if( !m_stylusInit ){
      m_polyData->SetPoints(m_stylusPoints);
      m_glyphFilter->SetInputData(m_polyData);
      m_glyphFilter->Update();

      m_mapper->SetInputConnection(m_glyphFilter->GetOutputPort());
      m_actor->SetMapper(m_mapper);
      m_actor->GetProperty()->SetColor(1,0,0);
      m_actor->GetProperty()->SetPointSize(5);
            
      vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
      m_interactor->SetInteractorStyle(style);

      this->GetRenderWindow()->AddRenderer(m_renderer);

      m_renderer->AddActor(m_actor);
      m_renderer->SetBackground(.3, .3, .6); // Background color blue
       
      // Render and start interaction
      m_interactor->SetRenderWindow(this->GetRenderWindow());
      m_interactor->Initialize();
      f64 *bounds = m_actor->GetBounds();
      m_renderer->ResetCamera(bounds);
      vtkCamera *cam = m_renderer->GetActiveCamera();
      m_renderer->SetActiveCamera(cam);
      m_stylusInit = true;
    }
  }

  void EMCalibrationWidget::resetView(void)
  {
    f64 *bounds = m_actor->GetBounds();
    m_renderer->ResetCamera(bounds);
    vtkCamera *cam = m_renderer->GetActiveCamera();
    m_renderer->SetActiveCamera(cam);
    m_stylusInit = true;
  }
  
  void EMCalibrationWidget::addFrame(void)
  {
    if( m_inStylusCalibration->GetValue() )
    {
      // add point to visualization
      Matrix44d pose = Matrix44d::FromCvMat(m_data.gps.pose);
      
      arma::mat T = pose.ToArmaMatrix4x4();

      Vec3d t = pose.GetPosition();
      Matrix33d R = pose.GetOrientation();
      
      m_stylusPoints->InsertNextPoint(t.x,t.y,t.z);
      m_stylusPoints->Modified();
      m_glyphFilter->Update();
      resetView();
    }

    int N = m_stylusPoints->GetNumberOfPoints();
    double x[3];
    for( int i = 0; i<N; i++ ){
      m_stylusPoints->GetPoint(i,x);
      NTrace("pt[%d] = {%.2f, %.2f, %.2f}\n",i,x[0],x[1],x[2]);
    }

    QVTKWidget::update();
  }

  EMCalibrationFileWidget::EMCalibrationFileWidget(QWidget *parent)
    : EMCalibrationWidget(parent) 
    , m_rpReaders(NULL)
  {
    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, EMCalibrationFileWidget), this, "F:/NeedleScan/6_12_15/Scan1/scan.b8", "Any File (*.*)");
    ADD_INT_PARAMETER(m_frame, "Frame Index", CALLBACK_POINTER(onUpdateFrame, EMCalibrationFileWidget), this, 1, 1, 100, 1);
    onUpdateFile();
  }

  void EMCalibrationFileWidget::onUpdateFile()
  {
    std::string fname = m_rpFile->GetValue();
    fname = fname.substr(0,fname.find_last_of("."));

    if(m_rpReaders) {
      delete m_rpReaders;
      m_rpReaders = NULL;
    }

    m_rpReaders = new RPFileReaderCollection();

    char temp[200];
    sprintf(temp,"%s.b8",fname.c_str());
    RPFileReader *tempReader = new RPFileReader(temp);
    s32 nframes = tempReader->GetHeader().frames;
    m_rpReaders->AddReader(RPF_BPOST8, (RPReader*)tempReader);
    sprintf(temp,"%s.b32",fname.c_str());
    tempReader = new RPFileReader(temp);
    m_rpReaders->AddReader(RPF_BPOST32, (RPReader *)tempReader);
    sprintf(temp,"%s.gps1",fname.c_str());
    m_rpReaders->AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(temp));  

    if(nframes <= 0) {
      delete m_rpReaders;
      m_rpReaders = NULL;
      return;
    }

    m_frame->SetMax(nframes);

    onUpdateFrame();
  }

  void EMCalibrationFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
    {
      m_data.Release();
    }

    m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    addFrame();
  }

  EMCalibrationFileWidget::~EMCalibrationFileWidget()
  {

  }

  EMCalibrationStreamingWidget::EMCalibrationStreamingWidget(QWidget *parent)
    : EMCalibrationWidget(parent) 
  {
    ADD_STRING_PARAMETER(m_rpIp, "Ulterius IP", NULL, this, "192.168.1.64");
    ADD_BOOL_PARAMETER(m_init, "Initialize", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, false);
    ADD_BOOL_PARAMETER(m_addFrames, "Add Frames", CALLBACK_POINTER(onAddFramesToggle, EMCalibrationStreamingWidget), this, false);
    ADD_INT_PARAMETER(m_framerate, "Ulterius Framerate", CALLBACK_POINTER(onFramerateChanged, EMCalibrationStreamingWidget), this, 11, 1, 30, 1);
    ADD_FLOAT_PARAMETER(m_mpp, "MPP", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, 60, 20, 150, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, Vec2d(330, 42), Vec2d(0,0), Vec2d(10000, 10000), Vec2d(1,1));

    onInitializeToggle();
    onAddFramesToggle();

    m_tick = std::tr1::shared_ptr<QTimer>((QTimer *)NULL);
    m_rpReaders = std::tr1::shared_ptr<RPUlteriusProcessManager>((RPUlteriusProcessManager *)NULL);
  }

  void EMCalibrationStreamingWidget::onInitializeToggle()
  {
    if(m_init->GetValue()) {
      //TODO: TAKE INTO ACCOUNT SOS
      Vec2d mpp(m_mpp->GetValue(), m_mpp->GetValue());
      m_rpReaders = std::tr1::shared_ptr < RPUlteriusProcessManager >(new RPUlteriusProcessManager(m_rpIp->GetValue().c_str(), mpp, m_origin->GetValue(), m_framerate->GetValue()));
      Sleep(30);  //Wait for old processes to die
      m_rpReaders->EnableType(RPF_BPOST8, 1);
      m_rpReaders->EnableType(RPF_GPS,1);
      Sleep(300);

      m_data = m_rpReaders->GetNextRPData();

      if(!m_tick) {
        m_tick = std::tr1::shared_ptr<QTimer>(new QTimer());
        connect(m_tick.get(), SIGNAL(timeout()), this, SLOT(onTick()));
        m_tick->setInterval(90);
        m_tick->start();
      }
    }
  }

  void EMCalibrationStreamingWidget::onAddFramesToggle()
  {
    //Don't do anything if we're not initialized
    if(!m_init->GetValue())
      return;

    if(m_addFrames->GetValue()) {
      m_data = m_rpReaders->GetNextRPData();
    }
  }

  void EMCalibrationStreamingWidget::onTick()
  {
    if(!m_init->GetValue())
      return;

    if(m_data.gps.valid)
      m_data.Release();
    m_data = m_rpReaders->GetNextRPData();
    if(m_data.b8 == NULL || !m_data.gps.valid)
      return;

    if(m_addFrames->GetValue()) {
      addFrame();
    }
  }

  void EMCalibrationStreamingWidget::onFramerateChanged()
  {
    if(!m_init->GetValue())
      return;

    m_rpReaders->SetFrameRate(m_framerate->GetValue());
  }

  EMCalibrationStreamingWidget::~EMCalibrationStreamingWidget()
  {
  }


}