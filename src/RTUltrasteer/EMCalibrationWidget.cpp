#include "EMCalibrationWidget.h"


namespace Nf
{
  EMCalibrationWidget::EMCalibrationWidget(QWidget *parent) 
    : Nf::ParameterCollection("EM Calibration")
    , ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , m_viewportInit(false)
    , m_robotCalibrationComplete(false)
  {
    // create layout
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    // initialize VTK objects
    m_stylusPoints = vtkSmartPointer<vtkPoints>::New();
    m_polyData = vtkSmartPointer<vtkPolyData>::New();
    m_glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    m_targetPoints = vtkSmartPointer<vtkPoints>::New();
    m_targetPolyData = vtkSmartPointer<vtkPolyData>::New();
    m_targetGlyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_targetMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_targetActor = vtkSmartPointer<vtkActor>::New();
    m_robotAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_EMrobotAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_stylusAxes = vtkSmartPointer<vtkAxesActor>::New();

    // initialize robot calibration matrices
    m_fiducialMeasurements = arma::zeros<arma::mat>(3,4);
    m_fiducialCoordinates = arma::zeros<arma::mat>(3,4);
    arma::vec3 p1, p2, p3, p4;
    p1 << -27.05 << -82.10 << -6.19;
    p2 << -9.52 << -117.72 << -3.02;
    p3 << 0.00 << -119.31 << -11.75;
    p4 << 9.53 << -16.31 << -11.75;
    m_fiducialCoordinates.col(0) = p1;
    m_fiducialCoordinates.col(1) = p2;
    m_fiducialCoordinates.col(2) = p3;
    m_fiducialCoordinates.col(3) = p4;

    // initialize viewport
    initViewport();

    // create stylus calibration object
    m_stylusCalibration = new StylusCalibration;

    // add framework params
    ADD_BOOL_PARAMETER(m_inStylusCalibration, "Calibrate Stylus", CALLBACK_POINTER(onCalibrateStylus, EMCalibrationWidget), this, false);
    ADD_BOOL_PARAMETER(m_inRobotCalibration, "Calibrate Robot", CALLBACK_POINTER(onCalibrateRobot, EMCalibrationWidget), this, false);
    ADD_INT_PARAMETER(m_robotFiducialNumber, "Robot Fiducial", CALLBACK_POINTER(onUpdateFiducial, EMCalibrationWidget), this, 1, 1, 4, 1);
  }

  EMCalibrationWidget::~EMCalibrationWidget()
  {
    delete m_stylusCalibration;
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

  void EMCalibrationWidget::initViewport(void)
  {
    if( !m_viewportInit ){ // if not already initialized
      // EM measurement points
      m_polyData->SetPoints(m_stylusPoints);
      m_glyphFilter->SetInputData(m_polyData);
      m_glyphFilter->Update();
      m_mapper->SetInputConnection(m_glyphFilter->GetOutputPort());
      m_actor->SetMapper(m_mapper);
      m_actor->GetProperty()->SetColor(1,0,0);
      m_actor->GetProperty()->SetPointSize(10);
      
      // target points
      m_targetPolyData->SetPoints(m_targetPoints);
      m_targetGlyphFilter->SetInputData(m_targetPolyData);
      m_targetGlyphFilter->Update();
      m_targetMapper->SetInputConnection(m_targetGlyphFilter->GetOutputPort());
      m_targetActor->SetMapper(m_targetMapper);
      m_targetActor->GetProperty()->SetColor(0,1,0);
      m_targetActor->GetProperty()->SetPointSize(10);

      // coordinate frame actors      
      m_robotAxes->SetTotalLength(10.0,10.0,10.0);
      m_EMrobotAxes->SetTotalLength(5.0,5.0,5.0);
      m_stylusAxes->SetTotalLength(7.0,7.0,7.0);
      m_robotAxes->VisibilityOff();
      m_EMrobotAxes->VisibilityOff();
      m_stylusAxes->VisibilityOff();

      // interactor and renderer
      vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
      m_interactor->SetInteractorStyle(style);
      this->GetRenderWindow()->AddRenderer(m_renderer);
      m_renderer->AddActor(m_actor);
      m_renderer->AddActor(m_targetActor);
      m_renderer->AddActor(m_robotAxes);
      m_renderer->AddActor(m_EMrobotAxes);
      m_renderer->AddActor(m_stylusAxes);
      m_renderer->SetBackground(.3, .3, .6); // Background color blue

      // render and start interaction
      m_interactor->SetRenderWindow(this->GetRenderWindow());
      m_interactor->Initialize();
      f64 *bounds = m_actor->GetBounds();
      m_renderer->ResetCamera(bounds);
      vtkCamera *cam = m_renderer->GetActiveCamera();
      m_renderer->SetActiveCamera(cam);
      m_viewportInit = true;
    }
  }


  void EMCalibrationWidget::onCalibrateStylus(void)
  {
    if( m_inStylusCalibration->GetValue() ){ // If starting a new stylus calibration
      RenderTargetPoints(false);
      RenderMeasuredPoints(false);
      m_stylusCalibration->clearMeasurements();
    }else{ // If finished collecting data, solve the stylus calibration
      m_stylusCalibration->solveCalibration();
      RenderTargetPoints(true,m_stylusCalibration->getCenter());
    }
  }

  void EMCalibrationWidget::onCalibrateRobot(void)
  {
    if( m_stylusCalibration->isComplete() ){ // If starting a new robot calibration
      if( m_inRobotCalibration->GetValue() ){
        RenderTargetPoints(false);
        RenderMeasuredPoints(false);
        m_currentFiducialMeasurements.reset();
        m_robotCalibrationComplete = false;
      }else{ // If finished collecting data, solve the robot calibration
        arma::mat mu = mean(m_currentFiducialMeasurements,1);
        m_fiducialMeasurements.col(m_robotFiducialNumber->GetValue()-1) = mu;
        RenderTargetPoints(true,mu);
        solveRobotRegistration();
      }
    }
  }

  void EMCalibrationWidget::onUpdateFiducial(void)
  {
    // do nothing; empty for framework
  }

  void EMCalibrationWidget::solveRobotRegistration(void)
  {
    using namespace arma;

    // check that we have recorded a point for all four robot fiducials
    if( all(vectorise(m_fiducialMeasurements)) )
    {
      // copy the datasets 
      mat A = m_fiducialMeasurements;
      mat B = m_fiducialCoordinates;

      // find the centroids of both datasets
      mat c_a = mean(A,1);
      mat c_b = mean(B,1);

      // find the covariance
      mat H(3,3,fill::zeros);
      for(int i = 0; i < A.n_cols; i++)
        H = H + (A.col(i)-c_a)*(B.col(i)-c_b).t();
      
      // find optimal rotation with SVD
      mat U;
      mat V;
      vec s;
      svd(U,s,V,H);
      mat R = V*U.t();

      // Find optimal translation
      vec t = -R*c_a + c_b;

      // Combine into a 4x4 transform
      m_T_em2robot = mat(4,4,fill::eye);
      m_T_em2robot.submat(0,0,2,2) = R;
      m_T_em2robot.submat(0,3,2,3) = t;

      // Save rotation and translation
      QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString("F:/ultrasteer/EMCal"), "*.m");
      if(fileName.length() <= 0) {
        return;
      }
      std::string fname = fileName.toStdString();
      m_T_em2robot.save(fname);

      // Visualize the fiducial targets and registered measurements
      mat transformedPoints(3,m_fiducialCoordinates.n_cols,fill::zeros);
      for(int i = 0; i < m_fiducialCoordinates.n_cols; i++)
        transformedPoints.col(i) = R*m_fiducialMeasurements.col(i) + t;

      // Render the transformed points
      RenderTargetPoints(true, m_fiducialCoordinates);
      RenderMeasuredPoints(true, transformedPoints);

      // Render the sensor and robot frames
      m_robotCalibrationComplete = true;
      m_robotAxes->VisibilityOn();
      m_EMrobotAxes->VisibilityOn();
      m_stylusAxes->VisibilityOn();
    }
  }

  void EMCalibrationWidget::RenderTargetPoints(bool show, arma::mat P)
  {
    // remove all existing target points
    m_targetPoints->Reset();
    
    if( show ){ // if we have points to show, add them individually
      for(int i = 0; i < P.n_cols; i++)
        m_targetPoints->InsertNextPoint(P(0,i),P(1,i),P(2,i));
    }

    // update and reset the view
    QVTKWidget::update();
    m_targetPoints->Modified();
    m_targetGlyphFilter->Update();
    QVTKWidget::update();
    resetView();
  }

  void EMCalibrationWidget::RenderMeasuredPoints(bool show, arma::mat P)
  {    
    // remove all existing measured points
    m_stylusPoints->Reset();

    if( show ){ // if we have points to show, add them individually
      for(int i = 0; i < P.n_cols; i++)
        m_stylusPoints->InsertNextPoint(P(0,i),P(1,i),P(2,i));
    }  

    // update and reset the view
    QVTKWidget::update();
    m_stylusPoints->Modified();
    m_glyphFilter->Update();
    QVTKWidget::update();
    resetView();
  }

  void EMCalibrationWidget::resetView(void)
  {
    f64 *bounds = m_actor->GetBounds();
    m_renderer->ResetCamera(bounds);
    vtkCamera *cam = m_renderer->GetActiveCamera();
    m_renderer->SetActiveCamera(cam);
  }
  
  void EMCalibrationWidget::addFrame(RPData &rp)
  {
    using namespace arma;

    // get relative position and orientation of the stylus
    mat33 R_i;
    vec3 p_i;
    rp.GetGPS1Relative(R_i,p_i);

    if( m_inStylusCalibration->GetValue() ) // if we are in stylus calibration mode
    {
      // pass relative position and orientation to the stylus calibration object
      m_stylusCalibration->addMeasurement(R_i,p_i);
            
      // add the point position to visualization
      m_stylusPoints->InsertNextPoint(p_i(0), p_i(1), p_i(2));
      m_stylusPoints->Modified();
      m_glyphFilter->Update();
      resetView();
      QVTKWidget::update();
    }

    if( m_inRobotCalibration->GetValue() && m_stylusCalibration->isComplete() ) // if we are in robot calibration mode
    {
    
      // calculate the relative position of the stylus tip at the current time
      mat p_tip = R_i*m_stylusCalibration->getCalibrationVector() + p_i;

      // add the current frame to a matrix of measurements
      m_currentFiducialMeasurements.insert_cols(m_currentFiducialMeasurements.n_cols,p_tip);
      RenderMeasuredPoints(true, m_currentFiducialMeasurements);
    }

    if( m_robotCalibrationComplete )
    {
      Matrix44d Eye;
      Eye.I();
      m_robotAxes->PokeMatrix(Eye.GetVTKMatrix());

      Matrix44d T1 = Matrix44d::FromArmaMatrix4x4(m_T_em2robot);
      m_EMrobotAxes->PokeMatrix(T1.GetVTKMatrix());

      mat33 Rs;
      vec3 ts;
      rp.GetGPS1Relative(Rs,ts);
      mat44 Ts;
      Ts.i();
      Ts.submat(0,0,2,2) = Rs;
      Ts.submat(0,3,2,3) = ts;
      Matrix44d T2 = Matrix44d::FromArmaMatrix4x4(m_T_em2robot*Ts);
      m_stylusAxes->PokeMatrix(T2.GetVTKMatrix());
    }
  }

  EMCalibrationFileWidget::EMCalibrationFileWidget(QWidget *parent)
    : EMCalibrationWidget(parent) 
    , m_rpReaders(NULL)
  {
    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, EMCalibrationFileWidget), this, "F:/NeedleScan/8_18_15/TroyData/scan.gps1", "Any File (*.*)");
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

    s32 nframes;

    //sprintf(temp,"%s.b8",fname.c_str());
    //tempReader = new RPFileReader(temp);
    //m_rpReaders->AddReader(RPF_BPOST8, (RPReader*)tempReader);
    //
    //sprintf(temp,"%s.b32",fname.c_str());
    //tempReader = new RPFileReader(temp);
    //m_rpReaders->AddReader(RPF_BPOST32, (RPReader *)tempReader);
    
    sprintf(temp,"%s.gps1",fname.c_str());
    RPGPSReader *tempReader = new RPGPSReader(temp);
    m_rpReaders->AddGPSReader((RPGPSReaderBasic *)tempReader);  
    sprintf(temp,"%s.gps2",fname.c_str());
    tempReader = new RPGPSReader(temp);
    m_rpReaders->AddGPSReader2((RPGPSReaderBasic *)tempReader);

    nframes = tempReader->GetHeader().frames;
    if(nframes <= 0) {
      delete m_rpReaders;
      m_rpReaders = NULL;
      return;
    }

    m_frame->SetMax(nframes);
    m_frame->SetValue(1);

    onUpdateFrame();
  }

  void EMCalibrationFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
    {
      m_data.Release();
    }

    m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    addFrame(m_data);
  }

  EMCalibrationFileWidget::~EMCalibrationFileWidget()
  {

  }

  EMCalibrationStreamingWidget::EMCalibrationStreamingWidget(QWidget *parent)
    : EMCalibrationWidget(parent) 
  {
    ADD_STRING_PARAMETER(m_rpIp, "Ulterius IP", NULL, this, "192.168.1.129");
    ADD_BOOL_PARAMETER(m_init, "Initialize", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, false);
    ADD_BOOL_PARAMETER(m_addFrames, "Add Frames", CALLBACK_POINTER(onAddFramesToggle, EMCalibrationStreamingWidget), this, false);
    ADD_INT_PARAMETER(m_framerate, "Ulterius Framerate", CALLBACK_POINTER(onFramerateChanged, EMCalibrationStreamingWidget), this, 11, 1, 30, 1);
    ADD_FLOAT_PARAMETER(m_mpp, "MPP", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, 60, 20, 150, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, Vec2d(330, 42), Vec2d(0,0), Vec2d(10000, 10000), Vec2d(1,1));

    onInitializeToggle();
    onAddFramesToggle();

    m_rpReaders = std::tr1::shared_ptr<RPUlteriusReaderCollectionPush>((RPUlteriusReaderCollectionPush *)NULL);

    connect(this, SIGNAL(frameSignal()), SLOT(onFrame()), Qt::QueuedConnection);
  }

  void EMCalibrationStreamingWidget::onInitializeToggle()
  {
    if(m_init->GetValue()) {
      //TODO: TAKE INTO ACCOUNT SOS
      Vec2d mpp(m_mpp->GetValue(), m_mpp->GetValue());
      m_rpReaders = std::tr1::shared_ptr < RPUlteriusReaderCollectionPush >(new RPUlteriusReaderCollectionPush(m_rpIp->GetValue().c_str(), mpp, m_origin->GetValue()));
      m_rpReaders->SetRPCallbackReceiver(this);
      u32 mask = RPF_GPS|RPF_BPOST8;
      m_rpReaders->EnableMask(mask);
      Sleep(30);
    }
  }

  void EMCalibrationStreamingWidget::onAddFramesToggle()
  {
    //Don't do anything if we're not initialized
    if(!m_init->GetValue())
      return;
  }

  void EMCalibrationStreamingWidget::onFrame()
  {
    if(!m_init->GetValue() || !m_lock.tryLock(30))
      return;

    RPData rp = m_data.Clone();
    m_lock.unlock();

    if(m_addFrames->GetValue()) {
      addFrame(rp);
    }

    rp.Release();
  }

  void EMCalibrationStreamingWidget::Callback(const RPData *rp)
  {
    if(rp && m_lock.tryLock(30)) {
      m_data.Release();
      m_data = *rp;
      m_lock.unlock();
      emit frameSignal();
      //QMetaObject::invokeMethod(this, "onFrame", Qt::QueuedConnection);
    }
  }

  void EMCalibrationStreamingWidget::onFramerateChanged()
  {
    if(!m_init->GetValue())
      return;
  }

  EMCalibrationStreamingWidget::~EMCalibrationStreamingWidget()
  {
  }


}