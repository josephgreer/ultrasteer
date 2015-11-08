#include "FSExperimentWidget.h"
#include <qmessagebox.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>

namespace Nf
{
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN ExperimentCalibrationData CLASS
  ////////////////////////////////////////////////////////////////////////////////
  ExperimentCalibrationData::ExperimentCalibrationData()
    : ParameterCollection("ExperimentCalibrationData")
  {
    ADD_BOOL_PARAMETER(m_addCornerPoints, "Add Corner Points", CALLBACK_POINTER(onAddCornerPoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_clearCornerPoints, "Clear Corner Points", CALLBACK_POINTER(onClearCornerPoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_determinePuncturePlane, "Calibrate Puncture Plane", CALLBACK_POINTER(onDeterminePuncturePlane, ExperimentCalibrationData), this, false);
    ADD_BOOL_PARAMETER(m_addNeedlePoints, "Add Needle Points", CALLBACK_POINTER(onAddNeedlePoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_clearNeedlePoints, "Clear Needle Points", CALLBACK_POINTER(onAddNeedlePoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_determineNeedleOffset, "Calibrate Needle Offset", CALLBACK_POINTER(onDetermineNeedleOffset, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_saveCalibrationData, "Save Calibration Data", CALLBACK_POINTER(SaveCalibrationData, ExperimentCalibrationData), this, false);
    ADD_INT_PARAMETER(m_cornerNumber, "Corner Number", NULL, this, 0, 0, 4, 1);
    
    m_planeCalibration = std::tr1::shared_ptr < PlaneCalibrator >(new PlaneCalibrator());
    m_tipCalibration = std::tr1::shared_ptr < EMNeedleTipCalibrator >(new EMNeedleTipCalibrator());
    m_corners = arma::zeros(3,4);
  }

  ExperimentCalibrationData::~ExperimentCalibrationData()
  {

  }
  
  void ExperimentCalibrationData::onAddCornerPoints()
  {
  }

  void ExperimentCalibrationData::onClearCornerPoints()
  {
    m_planeCalibration->ClearPoints();
  }

  void ExperimentCalibrationData::onDeterminePuncturePlane()
  {
    m_planeCalibration->DoCalibration();
    SaveCalibrationData();
  }

  void ExperimentCalibrationData::onAddNeedlePoints()
  {
  }

  void ExperimentCalibrationData::onClearNeedlePoints()
  {
    m_tipCalibration->ClearPoints();
  }

  void ExperimentCalibrationData::onDetermineNeedleOffset()
  {
    m_tipCalibration->DoCalibration();
    SaveCalibrationData();
  }

  void ExperimentCalibrationData::SaveCalibrationData()
  {
    QString filename = QFileDialog::getSaveFileName(NULL, "Save File", BASE_PATH_CAT(""), "*.m");
    if(filename.length() > 0) {
      QFileInfo fi(filename);
      std::string dir = fi.dir().absolutePath().toStdString()+"/";
      std::string fname = fi.baseName().toStdString();

      if(m_planeCalibration->IsCalibrated()) {
        m_planeCalibration->GetPoints().save(dir+fname+std::string("_plane.m"), arma::raw_ascii);
        m_corners.save(dir+fname+std::string("_corners.m"), arma::raw_ascii);
        m_lastGPS2.save(dir+fname+std::string("_GPS2.m"), arma::raw_ascii);

        //save this for matlab
        m_planeCalibration->GetSolution().GetABCD().save(dir+fname+std::string("_abcd.m"), arma::raw_ascii);
      }

      if(m_tipCalibration->IsCalibrated()) {
        Matrix33d tipFrame; Vec3d tipOffset;
        m_tipCalibration->GetSolution(tipOffset, tipFrame);

        arma::mat tipCalib = join_vert(tipFrame.ToArmaMat(), tipOffset.ToArmaVec().t());
        tipCalib.save(dir+fname+std::string("_tipOffset.m"), arma::raw_ascii);
      }

      if(m_stylusCalibration->IsCalibrated()) {
        Matrix33d styFrame; Vec3d styoffset;
        m_stylusCalibration->GetSolution(styoffset, styFrame);

        arma::mat styCalib = join_vert(styFrame.ToArmaMat(), styoffset.ToArmaVec().t());
        styCalib.save(dir+fname+std::string("_styOffset.m"), arma::raw_ascii);
      }

      if(m_baseFrame.n_rows >= 3 && m_baseFrame.n_rows >= 3 && m_baseOffset.n_elem > 0) {
        arma::mat frameCalib = arma::eye(4,4);
        frameCalib.submat(arma::span(0,2), arma::span(0,2)) = m_baseFrame;
        frameCalib.submat(arma::span(0,2), arma::span(3,3)) = m_baseOffset;
        frameCalib.save(dir+fname+std::string("_robotFrame.m"), arma::raw_ascii);
      }
    }
  }

  void ExperimentCalibrationData::addFrame(RPData rp)
  {
    m_lastGPS2 = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation(), rp.gps2.pos).ToArmaMatrix4x4();

    if(m_addCornerPoints->GetValue()) {
      if(!m_stylusCalibration) {
        QMessageBox mb;
        m_stylusCalibration = std::tr1::shared_ptr < EMNeedleTipCalibrator > (new EMNeedleTipCalibrator());
        mb.setText("Stylus calibration has not been specified");
        mb.exec();
      } else {
        Vec3d tipOffset; Matrix33d tipFrame;
        m_stylusCalibration->GetSolution(tipOffset, tipFrame, rp.gps.pos, Matrix44d::FromCvMat(rp.gps.pose).GetOrientation());
        m_planeCalibration->AddPoint(tipOffset);
        u8 corner = (u8)m_cornerNumber->GetValue();
        if(corner < m_corners.n_cols)
          m_corners.submat(arma::span(0,2), arma::span(corner,corner)) = tipOffset.ToArmaVec();
      }
    }
    if(m_addNeedlePoints->GetValue()) {
      if(m_baseFrame.n_elem == 0 || m_baseOffset.n_elem == 0) {
        QMessageBox mb;

        m_baseFrame = arma::zeros(3,3);
        m_baseOffset = arma::zeros(3,1);
        mb.setText("Robot coordinate frame has not been specified");
        mb.exec();
      } else {
        //Everything is relative to robot coordinate frame and rp.gps has been compensated to that frame already
        Vec3d tipOffset; Matrix33d tipFrame;
        m_stylusCalibration->GetSolution(tipOffset, tipFrame, rp.gps.pos, Matrix44d::FromCvMat(rp.gps.pose).GetOrientation());
        m_tipCalibration->AddPoint(Vec3d(0,0,0), Matrix33d::I(), tipOffset);
      }
    }
  }

  void ExperimentCalibrationData::SetBaseFrameData(arma::mat baseFrame, arma::mat baseOffset)
  {
    m_baseFrame = baseFrame;
    m_baseOffset = (arma::vec)baseOffset;
  }

  void ExperimentCalibrationData::SetStylusCalibration(std::tr1::shared_ptr < EMNeedleTipCalibrator > stylusCalib)
  {
    m_stylusCalibration = stylusCalib;
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END ExperimentCalibrationData CLASS
  ////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentCalibrationFileWidget::FSExperimentCalibrationFileWidget(QWidget *parent) 
    : EMCalibrationFileWidget(parent, "FSExperimentCalibrationFile")
  {
  }

  void FSExperimentCalibrationFileWidget::addFrame(RPData rp)
  {
    EMCalibrationFileWidget::addFrame(rp);
  }

  FSExperimentCalibrationFileWidget::~FSExperimentCalibrationFileWidget()
  {
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentCalibrationStreamingWidget::FSExperimentCalibrationStreamingWidget(QWidget *parent)
    : EMCalibrationStreamingWidget(parent, "FSExperimentCalibrationFile") 
    , m_experimentCalib(new ExperimentCalibrationData())
    , m_forceSensor(std::tr1::shared_ptr < cForceSensor > (new cForceSensor()))
    , m_styCalib(NULL)
    , m_cornerPointVis(std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(5, Vec3d(1,1,1))))
    , m_planeVis(std::tr1::shared_ptr < PlaneVisualizer > (new PlaneVisualizer(Vec3d(0,0,0), Vec3d(1,0,0))))
    , m_needleTipVis(std::tr1::shared_ptr < SphereVisualizer >(new SphereVisualizer(Vec3d(0,0,0), 5)))
    , m_hwWidget(NULL)
  {
    ADD_CHILD_COLLECTION((ParameterCollection *)m_experimentCalib.get());
    
    ADD_ACTION_PARAMETER(m_loadCalib, "Load Calibration Files", CALLBACK_POINTER(onLoadCalib, FSExperimentCalibrationStreamingWidget), this, false);
    ADD_ACTION_PARAMETER(m_alignCamToPlane, "Align Cam To Plane", CALLBACK_POINTER(onAlignCamToPlane, FSExperimentCalibrationStreamingWidget), this, false);

    m_planeAxis = vtkSmartPointer < vtkAxesActor >::New();
    m_planeAxis->SetTotalLength(20,20,20);
    m_planeAxis->SetVisibility(0);

    m_renderer->AddActor(m_planeAxis);
    m_renderer->AddActor(m_needleTipVis->GetActor());
    m_renderer->AddActor(m_cornerPointVis->GetActor());
    m_renderer->AddActor(m_planeVis->GetActor());

    m_needleTipVis->GetActor()->SetVisibility(0);
    m_needleTipVis->SetColor(Vec3d(1,0,0));
    m_cornerPointVis->GetActor()->SetVisibility(0);
    m_planeAxis->SetVisibility(0);
    m_planeVis->GetActor()->SetVisibility(0);

    CreateSphere(Vec3d(0,0,0), 5);
  }

  RPData FSExperimentCalibrationStreamingWidget::ResetDataRelativeToRobotFrame(const RPData &rp)
  {
    RPData res = rp;
    
    arma::mat44 agpsPosePos;
    rp.GetGPS1RelativeT(agpsPosePos);
    Matrix44d gpsPosePos = Matrix44d::FromArmaMatrix4x4(agpsPosePos);

    //Matrix44d gpsPosePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp.gps.pose).GetOrientation(), rp.gps.pos);
    gpsPosePos = Matrix44d::FromArmaMatrix4x4(m_Tref2robot)*gpsPosePos;
    res.gps.pose = Matrix44d::FromOrientationAndTranslation(gpsPosePos.GetOrientation(), Vec3d(0,0,0)).ToCvMat();
    res.gps.pos = gpsPosePos.GetPosition();
    return res;
  }

  void FSExperimentCalibrationStreamingWidget::onLoadCalib()
  {
    QString filename = QFileDialog::getOpenFileName(NULL, "Open File", BASE_PATH_CAT(""), "*.m");
    if(filename.length() > 0) {
      QFileInfo fi(filename);
      std::string dir = fi.dir().absolutePath().toStdString()+"/";
      std::string fname = fi.baseName().toStdString();
      fname = fname.substr(0,fname.find_last_of("_"));
      
      fi = QFileInfo(QString::fromStdString(dir+fname+std::string("_tipOffset.m")));
      if(fi.exists()) {
        arma::mat tip; tip.load(dir+fname+std::string("_tipOffset.m"));
        std::tr1::shared_ptr < EMNeedleTipCalibrator > tipOffset(new EMNeedleTipCalibrator());
        tipOffset->SetSolution(tip);
        m_experimentCalib->SetTipCalibration(tipOffset);
      }

      fi = QFileInfo(QString::fromStdString(dir+fname+std::string("_styOffset.m")));
      if(fi.exists()) {
        arma::mat sty; sty.load(dir+fname+std::string("_styOffset.m"));
        m_styCalib = std::tr1::shared_ptr < EMNeedleTipCalibrator >(new EMNeedleTipCalibrator());
        m_styCalib->SetSolution(sty);
        m_experimentCalib->SetStylusCalibration(m_styCalib);
        m_stylusCalibration->SetOffset(sty.submat(arma::span(3,3), arma::span(0,2)).t());
      }
      
      fi = QFileInfo(QString::fromStdString(dir+fname+std::string("_robotFrame.m")));
      if(fi.exists()) {
        arma::mat robotFrame; robotFrame.load(dir+fname+std::string("_robotFrame.m"));
        m_Tref2robot = robotFrame;
        m_experimentCalib->SetBaseFrameData(m_Tref2robot.submat(arma::span(0,2), arma::span(0,2)), m_Tref2robot.submat(arma::span(0,2), arma::span(3,3)));
        m_robotCalibrationComplete = true;
        m_robotAxes->VisibilityOn();
        m_EMrobotAxes->VisibilityOn();
        m_stylusAxes->VisibilityOn();
        RenderTargetPoints(true, m_fiducialsInFrobot);

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
      

      fi = QFileInfo(QString::fromStdString(dir+fname+std::string("_plane.m")));
      bool planeDataExists = fi.exists(); 
      fi = QFileInfo(QString::fromStdString(dir+fname+std::string("_GPS2.m")));
      bool frameDataExists = fi.exists();
      
      fi = QFileInfo(QString::fromStdString(dir+fname+std::string("_corners.m")));
      bool cornersExist = fi.exists();
      if(planeDataExists && frameDataExists && cornersExist && m_robotCalibrationComplete) {
        //pts are \in [p1^T; ...; p_m^T] \in R^{mx3}
        arma::mat pts; pts.load(dir+fname+std::string("_plane.m"));
        // convert pts to [p1 p2 ... p_m]  \in R^{3xm} for batch transformation
        pts = pts.t();
        //augment with ones for homogenous 
        pts = arma::join_vert(pts, arma::ones(1,pts.n_cols));

        //load frame these points were expressed in
        arma::mat F1; F1.load(dir+fname+std::string("_GPS2.m"));

        //transform pts to current frame
        pts = m_Tref2robot*inv(m_lastGPS2)*F1*inv(m_Tref2robot)*pts;
        
        // now convert back to preferred format
        pts = pts.rows(arma::span(0,2));
        pts = pts.t();

        std::tr1::shared_ptr < PlaneCalibrator > planeCalib(new PlaneCalibrator());

        arma::mat corners; corners.load(dir+fname+std::string("_corners.m"));
        //corners are in format [p1 p2 p3 p4] \in R^{3x4}
        //augment with ones for homogenous
        corners = arma::join_vert(corners, arma::ones(1, corners.n_cols));
        // convert to current coordinate system
        corners = m_Tref2robot*inv(m_lastGPS2)*F1*inv(m_Tref2robot)*corners;

        //now strip ones
        corners = corners.rows(arma::span(0,2));

        m_experimentCalib->SetCorners(corners);

        //Set the points
        planeCalib->SetPoints(pts);
        //recalculate plane
        planeCalib->DoCalibration();
        m_experimentCalib->SetPlaneCalibration(planeCalib);
      }
    }
  }
  
  void FSExperimentCalibrationStreamingWidget::onAlignCamToPlane()
  {
    if(m_experimentCalib->GetPlaneCalibration()->IsCalibrated()) {
      Plane plane = m_experimentCalib->GetPlaneCalibration()->GetSolution();
      arma::mat corners = m_experimentCalib->GetCorners();
      Vec3d corner, axis1, axis2;
      plane.GetCornerAndAxisVectors(corner, axis1, axis2, Vec3d::FromArmaVec((arma::vec3)corners.col(0)), 
        Vec3d::FromArmaVec((arma::vec3)corners.col(1)), Vec3d::FromArmaVec((arma::vec3)corners.col(2)));

      Vec3d xx = axis1.normalized();
      Vec3d yy = axis2.normalized();
      Vec3d zz = axis1.cross(axis2);

      // Align camera view with robot frame
      vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
      Vec3d up(xx.x, xx.y, xx.z);
      Vec3d focal(zz.x, zz.y, zz.z);
      camera->SetPosition(0,0,0);
      camera->SetFocalPoint(focal.x, focal.y, focal.z);
      camera->SetViewUp(up.x, up.y, up.z);
      m_renderer->SetActiveCamera(camera);

      // Set the view bounds
      f64 bounds[6] = {0};
      m_renderer->ComputeVisiblePropBounds(bounds);
      m_renderer->ResetCamera(bounds);
    }
  }


  void FSExperimentCalibrationStreamingWidget::addFrame(RPData rp)
  {
    EMCalibrationStreamingWidget::addFrame(rp);

    if(m_robotCalibrationComplete)
      rp = ResetDataRelativeToRobotFrame(rp);

    // save off lastGPS2 so that we know base frame. 
    // this isn eeded for calibration. For things like a plane that don't move with the coordinate system
    m_lastGPS2 = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation(), rp.gps2.pos).ToArmaMatrix4x4();

    m_experimentCalib->addFrame(rp);

    m_saveDataWidget->SaveDataFrame(rp);

    if(true) {//m_experimentCalib->m_addCornerPoints->GetValue()) {
      arma::mat pts = m_experimentCalib->GetPlaneCalibration()->GetPoints();
      std::vector < Vec3d > points;
      for(s32 i=0; i<pts.n_rows; i++) 
        points.push_back(Vec3d::FromArmaVec((arma::vec3)(pts.row(i).t())));
      m_cornerPointVis->SetPoints(points);
      m_cornerPointVis->GetActor()->SetVisibility(1);
    } else {
      m_cornerPointVis->GetActor()->SetVisibility(0);
    }

    if(m_experimentCalib->GetPlaneCalibration()->IsCalibrated()) {
      Plane plane = m_experimentCalib->GetPlaneCalibration()->GetSolution();
      arma::mat corners = m_experimentCalib->GetCorners();
      Vec3d corner, axis1, axis2;
      plane.GetCornerAndAxisVectors(corner, axis1, axis2, Vec3d::FromArmaVec((arma::vec3)corners.col(0)), 
        Vec3d::FromArmaVec((arma::vec3)corners.col(1)), Vec3d::FromArmaVec((arma::vec3)corners.col(2)));
      m_planeVis->SetPlane(corner,axis1,axis2);
      m_planeVis->GetActor()->SetVisibility(1);

      Vec3d xx = axis1.normalized();
      Vec3d yy = axis2.normalized();
      Vec3d zz = axis1.cross(axis2);
      m_planeAxis->PokeMatrix(Matrix44d::FromOrientationAndTranslation(Matrix33d::FromCols(xx,yy,zz), corner+(axis1+axis2)/2.0).GetVTKMatrix());
      m_planeAxis->SetVisibility(0);
    } else if(m_renderer->HasViewProp(m_planeVis->GetActor())) {
      m_planeVis->GetActor()->SetVisibility(0);
      m_planeAxis->SetVisibility(0);
    } 

    if(m_experimentCalib->GetTipCalibration()->IsCalibrated()) {
      f64 ins = 0;
      if(m_hwWidget->GetRCWidget()->isInsertionInitialized())
        ins = m_hwWidget->GetRCWidget()->GetInsertion();
      std::tr1::shared_ptr < EMNeedleTipCalibrator > tipCalib = m_experimentCalib->GetTipCalibration();

      Vec3d tipPos; Vec3d emPos(0,0,0); Matrix33d tipFrame; Matrix33d emFrame = Matrix33d::I();
      tipCalib->GetSolution(tipPos, tipFrame, emPos, emFrame);
      tipPos=tipPos+Vec3d(0,0,1)*ins;
      m_needleTipVis->SetCenter(tipPos);
      m_needleTipVis->GetActor()->SetVisibility(1);
    } else {
      m_needleTipVis->GetActor()->SetVisibility(0);
    }
    QVTKWidget::update();
  }

  FSExperimentCalibrationStreamingWidget::~FSExperimentCalibrationStreamingWidget()
  {
  }

  void FSExperimentCalibrationStreamingWidget::onCalibrateRobot()
  {
    EMCalibrationStreamingWidget::onCalibrateRobot();

    if(m_robotCalibrationComplete) {
      m_experimentCalib->SetBaseFrameData(m_Tref2robot.submat(arma::span(0,2), arma::span(0,2)), m_Tref2robot.submat(arma::span(0,2), arma::span(3,3)));
    }
  }

  void FSExperimentCalibrationStreamingWidget::onCalibrateStylus(void)
  {
    EMCalibrationStreamingWidget::onCalibrateStylus();

    //Done calibrating stylus
    if(m_stylusCalibration && m_stylusCalibration->isComplete()) {
      arma::mat offset = m_stylusCalibration->getCalibrationVector();
      m_styCalib = std::tr1::shared_ptr < EMNeedleTipCalibrator >(new EMNeedleTipCalibrator());

      arma::mat soln = arma::eye(4,4);
      soln.submat(arma::span(3,3), arma::span(0,2)) = offset.t();
      m_styCalib->SetSolution(soln);

      m_experimentCalib->SetStylusCalibration(m_styCalib);
    }
  }
  

  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentFileWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentFileWidget::FSExperimentFileWidget(QWidget *parent)
    : ParameterCollection("FSExperimentWidget")
    , m_layout(new QGridLayout(parent))
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_calibWidget(new FSExperimentCalibrationFileWidget(parent))
  {
    ADD_CHILD_COLLECTION((ParameterCollection *)m_calibWidget.get());
    m_layout->addWidget(m_calibWidget.get(), 0, 0);

    this->setLayout(m_layout.get());
  }

  FSExperimentFileWidget::~FSExperimentFileWidget()
  {
  }

  void FSExperimentFileWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_calibWidget->UpdateSize(QSize(sz.width()-10, sz.height()));
  }
  
  std::vector < QVTKWidget * > FSExperimentFileWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_calibWidget.get());
    return res;
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentFileWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentStreamingWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentStreamingWidget::FSExperimentStreamingWidget(QWidget *parent)
    : m_hwWidget(new RobotHardwareWidget(parent))
    , ParameterCollection("FSExperimentStreamingWidget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_calibWidget(new FSExperimentCalibrationStreamingWidget(parent))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_layout(new QGridLayout(parent))
    , m_bottomRow(new QGridLayout(parent))
    , m_robot(NULL)
  {
    ADD_CHILD_COLLECTION((ParameterCollection *)m_calibWidget.get());
    ADD_CHILD_COLLECTION((ParameterCollection *)m_hwWidget.get());

    m_calibWidget->SetSaveDataWidget(m_saveDataWidget);

    m_layout->addWidget(m_calibWidget.get(), 0, 0);
    
    m_layout->setContentsMargins(0,0,5,0);
    this->setLayout(m_layout.get());

    m_bottomRow->addWidget(m_hwWidget.get(), 0, 0);
    m_bottomRow->addWidget(m_saveDataWidget.get(), 0, 1);
    m_layout->addLayout(m_bottomRow.get(), 1, 0, 1, 2);
  }

  void FSExperimentStreamingWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_calibWidget->UpdateSize(QSize(sz.width()-10, (3*sz.height())/4));
    m_hwWidget->UpdateSize(QSize(3*sz.width()/4-10, (sz.height())/4));
    m_saveDataWidget->UpdateSize(QSize(sz.width()/4-10, (sz.height())/4));
  }

  FSExperimentStreamingWidget::~FSExperimentStreamingWidget()
  {
    this->setLayout(NULL);
    m_bottomRow = NULL;
    m_layout = NULL;
  }

  void FSExperimentStreamingWidget::SetRobot(NeedleSteeringRobot *robot)
  {
    m_robot = robot;
    m_hwWidget->setRobot(robot);
    m_calibWidget->SetRobotHW(m_hwWidget);
  }
  

  std::vector < QVTKWidget * > FSExperimentStreamingWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_calibWidget.get());
    return res;
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentStreamingWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
}