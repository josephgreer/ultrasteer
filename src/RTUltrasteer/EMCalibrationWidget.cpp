#include "EMCalibrationWidget.h"
#include <vtkCamera.h>


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
    m_textActor = vtkSmartPointer<vtkTextActor>::New();
    
    // initialize display text
    m_textActor->GetTextProperty()->SetFontSize ( 24 );
    m_textActor->SetPosition( 10, 10 );
    m_renderer->AddActor2D ( m_textActor );
    m_textActor->SetInput ( " " );
    m_textActor->GetTextProperty()->SetColor( 1.0,1.0,1.0 );

    // initialize robot calibration matrices
    m_fiducialMeasurementsInFref = arma::zeros<arma::mat>(3,4);
    m_fiducialsInFrobot = arma::zeros<arma::mat>(3,4);
    arma::vec3 p1, p2, p3, p4;
    p1 << 13.335 << -15.24 << -0.635;
    p2 << 24.51119 << -82.10035 << -0.635;
    p3 << -24.51081 << -82.10035 << -0.635;
    p4 << 9.52519 << -116.13635 << -5.34249;
    m_fiducialsInFrobot.col(0) = p1;
    m_fiducialsInFrobot.col(1) = p2;
    m_fiducialsInFrobot.col(2) = p3;
    m_fiducialsInFrobot.col(3) = p4;

    // initialize viewport
    initViewport();

    // create stylus calibration object
    m_stylusCalibration = new StylusCalibration;

    // add framework params
    ADD_BOOL_PARAMETER(m_inStylusCalibration, "Calibrate Stylus", CALLBACK_POINTER(onCalibrateStylus, EMCalibrationWidget), this, false);
    ADD_BOOL_PARAMETER(m_inRobotCalibration, "Calibrate Robot", CALLBACK_POINTER(onCalibrateRobot, EMCalibrationWidget), this, false);
    ADD_BOOL_PARAMETER(m_inStylusPointSelection, "Record Stylus Point", CALLBACK_POINTER(onStylusPointSelection, EMCalibrationWidget), this, false);
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

      // Pass the identity matrix to the robot coordinate frame 
      Matrix44d I;
      I = Matrix44d::I();
      m_robotAxes->PokeMatrix(I.GetVTKMatrix());

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
      f32 e = m_stylusCalibration->solveCalibration();
      char str[100];
      int n = sprintf(str,"Pivot FRE = %.3f mm\n",e);
      m_textActor->SetInput ( str );
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
        arma::mat var = cov(m_currentFiducialMeasurements.t());
        char str[100];
        int n = sprintf(str,"var = {%.2f, %.2f, %.2f} mm\n",var.at(0,0), var.at(1,1), var.at(2,2));
        m_textActor->SetInput ( str );

        int fidNum = m_robotFiducialNumber->GetValue()-1;
        char strbuff[100];
        sprintf(strbuff, "fiducialMeasurements %d.m",fidNum);
        m_currentFiducialMeasurements.save(strbuff, arma::arma_ascii);
        m_fiducialMeasurementsInFref.col(fidNum) = mu;
        RenderTargetPoints(true,mu);
        solveRobotRegistration();
      }
    }
  }

  void EMCalibrationWidget::onStylusPointSelection(void)
  {
    if( m_inStylusPointSelection->GetValue() ){ // if starting to collect data
        m_currentFiducialMeasurements.reset();
      }else{ // If finished collecting current
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString("D:/ultrasteer/currentPoint"), "*.m");
        if(fileName.length() <= 0) {
          return;
        }
        std::string fname = fileName.toStdString();
        m_currentFiducialMeasurements.save(fname, arma::arma_ascii);
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
    if( all(vectorise(m_fiducialMeasurementsInFref)) )
    {
      // save the raw datasets for validation
      std::string fname1("fiducialsInFref.m");
      std::string fname2("fiducialsInFrobot.m");
      m_fiducialMeasurementsInFref.save(fname1, arma_ascii);
      m_fiducialsInFrobot.save(fname2, arma_ascii);

      // copy the datasets 
      mat A = m_fiducialMeasurementsInFref;
      mat B = m_fiducialsInFrobot;

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

      // Find the average FRE
      double fre_avg = 0.0;
      for(int i = 0; i < A.n_cols; i++){
        fre_avg += norm( B.col(i)-(R*A.col(i)+t) , 2 );
      }
      fre_avg = fre_avg/double(A.n_cols);
      char str[100];
      int n = sprintf(str,"Average FRE = %.3f mm\n",fre_avg);
      m_textActor->SetInput ( str );

      // Combine into a 4x4 transform
      m_Tref2robot = mat(4,4,fill::eye);
      m_Tref2robot.submat(0,0,2,2) = R;
      m_Tref2robot.submat(0,3,2,3) = t;

      // Save rotation and translation
      QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString("F:/ultrasteer/EMCal"), "*.m");
      if(fileName.length() <= 0) {
        return;
      }
      std::string fname = fileName.toStdString();
      m_Tref2robot.save(fname, arma_ascii);

      // Visualize the fiducial targets and registered measurements
      mat fiducialMeasurementsRegistered(3,m_fiducialsInFrobot.n_cols,fill::zeros);
      for(int i = 0; i < m_fiducialsInFrobot.n_cols; i++)
        fiducialMeasurementsRegistered.col(i) = R*m_fiducialMeasurementsInFref.col(i) + t;

      // Render the transformed points
      RenderTargetPoints(true, m_fiducialsInFrobot);
      RenderMeasuredPoints(true, fiducialMeasurementsRegistered);

      // Render the sensor and robot frames
      m_robotCalibrationComplete = true;
      m_robotAxes->VisibilityOn();
      m_EMrobotAxes->VisibilityOn();
      m_stylusAxes->VisibilityOn();

      // Align camera view with robot frame
      vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
      Vec3d up(-1,0,0);
      Vec3d focal(0,0,1);
      camera->SetPosition(0,0,0);
      camera->SetFocalPoint(focal.x, focal.y, focal.z);
      camera->SetViewUp(up.x, up.y, up.z);
      m_renderer->SetActiveCamera(camera);

      // Set the view bounds
      f64 *bounds = m_targetActor->GetBounds();
      m_renderer->ResetCamera(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], 
        bounds[5]);
      m_renderer->GetActiveCamera()->Zoom(1.3);
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

    if( m_robotCalibrationComplete ) // if the robot is calibrated
    {
      // get the em reference sensor frame relative to the robot frame
      Matrix44d T_emref2robot = Matrix44d::FromArmaMatrix4x4(m_Tref2robot);
      m_EMrobotAxes->PokeMatrix(T_emref2robot.GetVTKMatrix());

      // get the stylus tip frame relative to the robot frame
      mat44 T_stylus2robot;
      rp.GetGPS1RelativeT(T_stylus2robot);    
      mat44 T_stylusTip2stylusRef = eye<mat>(4,4);
      T_stylusTip2stylusRef.submat(0,3,2,3) = m_stylusCalibration->getCalibrationVector();
      Matrix44d Tstylustip2robot = Matrix44d::FromArmaMatrix4x4(m_Tref2robot*T_stylus2robot*T_stylusTip2stylusRef);
      m_stylusAxes->PokeMatrix(Tstylustip2robot.GetVTKMatrix());
      
      // update the display
      QVTKWidget::update();
    }

    if( m_inStylusPointSelection->GetValue() && m_stylusCalibration->isComplete() ) // if we are recording the tip of the calibrated stylus
    {
      // calculate the relative position of the stylus tip at the current time
      mat p_tip = R_i*m_stylusCalibration->getCalibrationVector() + p_i;

      // add the current frame to a matrix of measurements
      m_currentFiducialMeasurements.insert_cols(m_currentFiducialMeasurements.n_cols,p_tip);
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
    ADD_FLOAT_PARAMETER(m_mpp, "MPP", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, 233, 20, 150, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onInitializeToggle, EMCalibrationStreamingWidget), this, Vec2d(330, -153), Vec2d(0,0), Vec2d(10000, 10000), Vec2d(1,1));

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
      u32 mask = RPF_GPS|RPF_BPOST8|RPF_GPS2;
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