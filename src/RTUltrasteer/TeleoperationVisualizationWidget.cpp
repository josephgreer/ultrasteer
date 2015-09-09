#include "TeleoperationVisualizationWidget.h"

namespace Nf
{
  TeleoperationVisualizationWidget::TeleoperationVisualizationWidget(QWidget *parent, ControlAlgorithms* control) 
    : Nf::ParameterCollection("Teleoperation Visualization Widget")
    , ResizableQVTKWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , m_viewportInit(false)
    , m_frameBoundaries((CubeVisualizer *)NULL)
  {
    // set the control pointer
    m_control = control;

    // create layout
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    // initialize VTK objects
    m_robotSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    m_robotSTLMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_robotSTLActor = vtkSmartPointer<vtkActor>::New();

    m_transducerSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    m_transducerSTLMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_transducerSTLActor = vtkSmartPointer<vtkActor>::New();

    m_estimateSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    m_estimateSTLMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_estimateSTLActor = vtkSmartPointer<vtkActor>::New();

    m_measurementSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    m_measurementSTLMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_measurementSTLActor = vtkSmartPointer<vtkActor>::New();

    m_targetSource = vtkSmartPointer<vtkSphereSource>::New();
    m_targetMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_targetActor = vtkSmartPointer<vtkActor>::New();
    m_robotAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_referenceAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_transducerAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_emAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_estimateAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_measurementAxes = vtkSmartPointer<vtkAxesActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    // initialize viewport
    initViewport();

    // add framework params
    ADD_ACTION_PARAMETER(m_setViewXY, "Set View XY", CALLBACK_POINTER(onSetViewXY, TeleoperationVisualizationWidget), this, true);
    ADD_ACTION_PARAMETER(m_setViewYZ, "Set View YZ", CALLBACK_POINTER(onSetViewYZ, TeleoperationVisualizationWidget), this, true);
    ADD_ACTION_PARAMETER(m_setViewXZ, "Set View XZ", CALLBACK_POINTER(onSetViewXZ, TeleoperationVisualizationWidget), this, true);
  }

  TeleoperationVisualizationWidget::~TeleoperationVisualizationWidget()
  {
  }

  void TeleoperationVisualizationWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void TeleoperationVisualizationWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQVTKWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > TeleoperationVisualizationWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }

  void TeleoperationVisualizationWidget::initViewport(void)
  {
    if( !m_viewportInit ){ // if not already initialized
      // robot model
      m_robotSTLReader->SetFileName(STL_PATH_CAT("NeedleSteeringRobot.STL"));
      m_robotSTLReader->Update();
      m_robotSTLMapper->SetInputConnection(m_robotSTLReader->GetOutputPort());
      m_robotSTLActor->SetMapper(m_robotSTLMapper);
      m_robotSTLActor->GetProperty()->SetColor(0.8, 0.8, 0.8);

      // transducer model
      m_transducerSTLMapper->SetInputConnection(m_transducerSTLReader->GetOutputPort());
      m_transducerSTLActor->SetMapper(m_transducerSTLMapper);
      m_transducerSTLActor->GetProperty()->SetColor(0.8, 0.8, 0.8);
      m_transducerSTLActor->VisibilityOff();

      // estimate tip model
      m_estimateSTLReader->SetFileName(STL_PATH_CAT("8mmBentTip.STL"));
      m_estimateSTLReader->Update();
      m_estimateSTLMapper->SetInputConnection(m_estimateSTLReader->GetOutputPort());
      m_estimateSTLActor->SetMapper(m_estimateSTLMapper);
      m_estimateSTLActor->VisibilityOff();

      // measurement tip model
      m_measurementSTLReader->SetFileName(STL_PATH_CAT("4mmBentTip.STL"));
      m_measurementSTLReader->Update();
      m_measurementSTLMapper->SetInputConnection(m_measurementSTLReader->GetOutputPort());
      m_measurementSTLActor->SetMapper(m_measurementSTLMapper);
      m_measurementSTLActor->VisibilityOff();

      // target point
      m_targetSource->SetRadius(5.0);
      m_targetSource->SetCenter(0.0,0.0,0.0);
      m_targetActor->SetVisibility(false);
      m_targetMapper->SetInputConnection(m_targetSource->GetOutputPort());
      m_targetActor->SetMapper(m_targetMapper);
      m_targetActor->GetProperty()->SetColor(0.0, 0.0, 1.0);

      // coordinate frame actors           
      m_robotAxes->SetTotalLength(20.0,20.0,20.0);
      m_referenceAxes->SetTotalLength(10.0,10.0,10.0);
      m_transducerAxes->SetTotalLength(10.0,10.0,10.0);
      m_emAxes->SetTotalLength(10.0,10.0,10.0);
      m_estimateAxes->SetTotalLength(8.0,8.0,8.0);
      m_measurementAxes->SetTotalLength(8.0,8.0,8.0);
      m_robotAxes->VisibilityOff();
      m_referenceAxes->VisibilityOff();
      m_transducerAxes->VisibilityOff();
      m_emAxes->VisibilityOff();
      m_estimateAxes->VisibilityOff();
      m_measurementAxes->VisibilityOff();

      // Pass the identity matrix to the robot coordinate frame 
      Matrix44d I;
      I = Matrix44d::I();
      m_robotAxes->PokeMatrix(I.GetVTKMatrix());
      m_robotAxes->VisibilityOn();

      // interactor and renderer
      vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
      m_interactor->SetInteractorStyle(style);
      this->GetRenderWindow()->AddRenderer(m_renderer);

      m_renderer->AddActor(m_targetActor);
      m_renderer->AddActor(m_robotAxes);
      m_renderer->AddActor(m_referenceAxes);
      m_renderer->AddActor(m_transducerAxes);
      m_renderer->AddActor(m_emAxes);
      m_renderer->AddActor(m_estimateAxes);
      m_renderer->AddActor(m_measurementAxes);
      m_renderer->AddActor(m_robotSTLActor);
      m_renderer->AddActor(m_transducerSTLActor);
      m_renderer->AddActor(m_measurementSTLActor);
      m_renderer->AddActor(m_estimateSTLActor);
      m_renderer->SetBackground(.3, .3, .3); // Background color blue

      // render and start interaction
      m_interactor->SetRenderWindow(this->GetRenderWindow());
      m_interactor->Initialize();
      f64 *bounds = m_robotAxes->GetBounds();
      m_renderer->ResetCamera(bounds);
      vtkCamera *cam = m_renderer->GetActiveCamera();
      m_renderer->SetActiveCamera(cam);

      m_viewportInit = true;
    }
  }

  void TeleoperationVisualizationWidget::onUpdateVisualization(void)
  {
    // define matrices and vectors for target and axes
    Vec3d t;
    Matrix44d x, z, Tref2robot, Ttrans2robot, Tem2robot;
    Cubed framePoints;
    s32 transducerType;

    // get values and update display
    m_control->getVisualizerValues(t, x, z, Tref2robot, Ttrans2robot, transducerType, framePoints, Tem2robot);

    if( !t.isZero() ){
      m_targetSource->SetCenter(t.x,t.y,t.z);
      m_targetSource->Modified();
      m_targetActor->VisibilityOn();
    }else{
      m_targetActor->VisibilityOff();
    }

    if( !x.isZero() ){
      m_estimateAxes->PokeMatrix(x.GetVTKMatrix());
      m_estimateSTLActor->PokeMatrix(x.GetVTKMatrix());
      m_estimateAxes->VisibilityOn();
      m_estimateSTLActor->VisibilityOn();
    }else{
      m_estimateAxes->VisibilityOff();
      m_estimateSTLActor->VisibilityOff();
    }

    if( !z.isZero() ){
      m_measurementAxes->PokeMatrix(z.GetVTKMatrix());
      m_measurementSTLActor->PokeMatrix(z.GetVTKMatrix());
      m_measurementAxes->VisibilityOn();
      m_measurementSTLActor->VisibilityOn();
    }else{
      m_measurementAxes->VisibilityOff();
      m_measurementSTLActor->VisibilityOff();
    }

    if( !Tref2robot.isZero() ){
      m_referenceAxes->PokeMatrix(Tref2robot.GetVTKMatrix());
      m_referenceAxes->VisibilityOn();
    }else{
      m_referenceAxes->VisibilityOff();
    }

    if( !Ttrans2robot.isZero() ){
      m_transducerAxes->PokeMatrix(Ttrans2robot.GetVTKMatrix());
      m_transducerSTLActor->PokeMatrix(Ttrans2robot.GetVTKMatrix());
      m_transducerAxes->VisibilityOn();
      m_transducerSTLActor->VisibilityOn();
      
      if( m_frameBoundaries ){
        m_renderer->RemoveActor(m_frameBoundaries->GetActor());
        delete m_frameBoundaries;
      }
      u8 cubeColor[3] = {200, 200, 200};
      m_frameBoundaries = new CubeVisualizer(framePoints, cubeColor);
      m_renderer->AddActor(m_frameBoundaries->GetActor());

      switch ( transducerType ){
      case QtEnums::Transducer::LINEAR :
        m_transducerSTLReader->SetFileName(STL_PATH_CAT("L14-5GPS.STL"));
        break;
      case QtEnums::Transducer::CONVEX :
        m_transducerSTLReader->SetFileName(STL_PATH_CAT("C-52GPS.STL"));
        break;
      }
      m_transducerSTLReader->Update();
    }else{
      m_transducerAxes->VisibilityOff();
      m_transducerSTLActor->VisibilityOff();
      if( m_frameBoundaries )
        m_renderer->RemoveActor(m_frameBoundaries->GetActor());
    }

    if( !Tem2robot.isZero() ){
      m_emAxes->PokeMatrix(Tem2robot.GetVTKMatrix());
      m_emAxes->VisibilityOn();
    }else{
      m_emAxes->VisibilityOff();
    }
    
    m_renderer->ResetCameraClippingRange();
    QVTKWidget::update();
  }

  void TeleoperationVisualizationWidget::SetVisView(s32 axis1, s32 axis2)
  {
    Matrix33d I = Matrix33d::I();
    vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    Vec3d up = I.Col(axis1);
    Vec3d focal = I.Col(axis2)*-1.0;
    camera->SetPosition(0,0,0);
    camera->SetFocalPoint(focal.x, focal.y, focal.z);
    camera->SetViewUp(up.x, up.y, up.z);
    m_renderer->SetActiveCamera(camera);
    
    if(m_targetActor->GetVisibility()) {
      f64 *bounds = m_targetActor->GetBounds();
      m_renderer->ResetCamera(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], 
        bounds[5]);
      m_renderer->GetActiveCamera()->Zoom(0.4);
    }

    f64 bounds[6];
    m_renderer->ComputeVisiblePropBounds(bounds);
    m_renderer->ResetCamera(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], 
      bounds[5]);
    m_renderer->GetActiveCamera()->Zoom(0.75);
    QVTKWidget::update();
  }

  void TeleoperationVisualizationWidget::onSetViewXY()
  {
    SetVisView(0,2);
  }

  void TeleoperationVisualizationWidget::onSetViewXZ()
  {
    SetVisView(0,1);
  }

  void TeleoperationVisualizationWidget::onSetViewYZ()
  {
    SetVisView(2,0);
  }
}