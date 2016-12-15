<<<<<<< HEAD:src/UnitTests/testIO.cpp.cpp
#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "RPFileReader.h"
#include "RPFileWriter.h"
#include "RPUlterius.h"
#include "Calibration.h"
#include <memory>
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

using namespace Nf;

typedef struct {
  char ip[100];
  u32 typeMask;
  u32 nframes;
  Vec2d mpp;
  Vec2d origin;
} BasicsRPUlteriusTest;

TEST(IO, UlteriusCaptureRate)
{
	BasicsRPUlteriusTest tests[] = {
    {"192.168.1.129", RPF_BPOST8|RPF_GPS, 200, Vec2d(87.0,87.0), Vec2d(330.0, 78.0)}
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++){
		RPUlteriusReaderCollection reader(tests[n].ip, tests[n].mpp, tests[n].origin);

    reader.EnableMask(tests[n].typeMask);

    u32 t0 = GetTickCount();
    s32 i=0;
    printf("\n");
    while(i < tests[n].nframes) {
      RPData curr = reader.GetNextRPData();
      if(curr.FullSet(tests[n].typeMask)) {
        curr.Release();
        i++;
      }
      printf("\rFrame %d", i);
    }
    u32 t1 = GetTickCount();

    f32 msPerFrame = (f32)(t1-t0)/(f32)tests[n].nframes;
    f32 sPerFrame = msPerFrame/1000.0f;
    f32 fps = 1/sPerFrame;

    NTrace("Average fps %f over %d frames\n", fps, tests[n].nframes);
  }
}

TEST(Scratch, ReverseIterator)
{
  std::map < int, int > yep;

  for(s32 i=5; i<20; i++) {
    yep[i] = i;
  }

  for(std::map<int,int>::reverse_iterator i = yep.rbegin(); i!=yep.rend(); i++)
    NTrace("curr %d\n", i->first);
 
  s32 del = 8;

  std::map<int,int>::reverse_iterator i = yep.rbegin();
  for(; i!=yep.rend(); i++) {
    if(i->first == del)
      break;
  }

  
  yep.erase(std::next(i).base());

  NTrace("After deletion of %d\n", del);
  for(std::map<int,int>::reverse_iterator i = yep.rbegin(); i!=yep.rend(); i++)
    NTrace("curr %d\n", i->first);

  for(s32 i=5; i<20; i++) {
    if(i == del)
      assert(yep.find(i) == yep.end());
    else
      assert(yep.find(i) != yep.end());
  }
}

TEST(GPS, Smooth)
{
  using ::s32;

  std::string basePath = BASE_PATH_CAT("1_7_16/05mm/Trial3/scan");

  char temp[200] = {0};
  sprintf(temp, "%s.gps2", basePath.c_str());

  RPGPSReader *gpsReader = new RPGPSReader(temp);
  RPFileHeader header = gpsReader->GetHeader();

  sprintf(temp, "%ssmoothed.gps2", basePath.c_str());
  RPGPSWriter *writer = new RPGPSWriter(temp, &header);

  sprintf(temp, "%ssmoothed.dat", basePath.c_str());
  arma::mat smoothedPoints; smoothedPoints.load(temp);

  for(s32 i=0; i<smoothedPoints.n_rows; i++) {
    arma::vec3 point = (arma::vec3)(smoothedPoints.row(i).t());
    GPS_Data datum = gpsReader->GetNextGPSDatum();
    datum.pos = Vec3d::FromArmaVec(point);
    writer->WriteNextGPSDatum(&datum);
  }

  writer->Cleanup(&header);

  delete writer;
  delete gpsReader;
}

TEST(Command, CorrectTick)
{
  using ::s32;

  f32 aveTimeMs = 180;

  std::string basePath = PATH_CAT("3mm/Trial1/scan");

  char temp[200] = {0};
  sprintf(temp, "%s.gps2", basePath.c_str());

  RPGPSReader *gpsReader = new RPGPSReader(temp);
  RPFileHeader header = gpsReader->GetHeader();
  
  sprintf(temp, "%s.u", basePath.c_str());
  NSCommandReader *commandReader = new NSCommandReader(temp);

  sprintf(temp, "%s_corrected.u", basePath.c_str());
  NSCommandWriter *writer = new NSCommandWriter(temp);

  for(s32 i=0; i<header.frames; i++) {
    NSCommand u = commandReader->GetNextNSCommand();
    u.tick = (u32)(i*aveTimeMs+0.5f);
    writer->WriteNextNSCommand(&u);
  }

  writer->Cleanup();

  delete writer;
  delete gpsReader;
  delete commandReader;
}

static std::tr1::shared_ptr < PlaneCalibrator > recalculatePlanePointsForNewCoordinateFrame(arma::mat pts, arma::mat F1, arma::mat tRef2Robot, arma::mat fgps2, arma::mat &corners)
{
  //pts are \in [p1^T; ...; p_m^T] \in R^{mx3}
  // convert pts to [p1 p2 ... p_m]  \in R^{3xm} for batch transformation
  pts = pts.t();
  //augment with ones for homogenous 
  pts = arma::join_vert(pts, arma::ones(1,pts.n_cols));

  //F1 is frame these points are expressed in
  //tRef2Robot is coordinate frame calibration

  //transform pts to current frame
  pts = tRef2Robot*inv(fgps2)*F1*inv(tRef2Robot)*pts;

  // now convert back to preferred format
  pts = pts.rows(arma::span(0,2));
  pts = pts.t();

  std::tr1::shared_ptr < PlaneCalibrator > planeCalib(new PlaneCalibrator());

  //Set the points
  planeCalib->SetPoints(pts);
  //recalculate plane
  planeCalib->DoCalibration();

  //corners are in format [p1 p2 p3 p4] \in R^{3x4}
  //augment with ones for homogenous
  corners = arma::join_vert(corners, arma::ones(1, corners.n_cols));
  // convert to current coordinate system
  corners = tRef2Robot*inv(fgps2)*F1*inv(tRef2Robot)*corners;

  //now strip ones
  corners = corners.rows(arma::span(0,2));

  return planeCalib;
}

TEST(IO, ForceData)
{
  std::string orientationBasePath = "C:/Joey/Data/ForceData/WithLiver/Orientation";
  std::string baseName = "Trial";
  s32 numOrientations = 3;
  s32 numTrials = 10;

  std::string calibrationBasePath = "Calibration/calib";

  for(s32 j=1; j<=numOrientations; j++) {
    std::string basePath = orientationBasePath+std::to_string((_Longlong)j)+std::string("/")+baseName;
    std::string calibrationPath = orientationBasePath+std::to_string((_Longlong)j)+std::string("/")+calibrationBasePath;
    for(s32 i=1; i<=numTrials; i++) {
      RPFileReaderCollection *rpReaders = new RPFileReaderCollection();

      std::tr1::shared_ptr < ForceReader > forceReader(new ForceReader((basePath+std::to_string((_Longlong)i)+std::string("/scan.force")).c_str()));
      rpReaders->AddForceReader(forceReader.get());
      std::tr1::shared_ptr < RPGPSReader > gpsReader1(new RPGPSReader((basePath+std::to_string((_Longlong)i)+std::string("/scan.gps1")).c_str()));
      rpReaders->AddGPSReader((RPGPSReaderBasic *)gpsReader1.get());
      std::tr1::shared_ptr < RPGPSReader > gpsReader2(new RPGPSReader((basePath+std::to_string((_Longlong)i)+std::string("/scan.gps2")).c_str()));
      rpReaders->AddGPSReader2((RPGPSReaderBasic *)gpsReader2.get());

      arma::mat needlePos;
      arma::mat forces;
      arma::mat torques;

      arma::mat tip; tip.load(calibrationPath+std::string("_tipOffset.m"));
      std::tr1::shared_ptr < EMNeedleTipCalibrator > tipCalib(new EMNeedleTipCalibrator());
      tipCalib->SetSolution(tip);

      RPFileHeader header = gpsReader1->GetHeader();
      for(s32 frame=1; frame<=header.frames; frame++) {
        RPData rp = rpReaders->GetRPData(frame);

        //Reset plane points into coordinate system attached to robot
        if(frame == 1) {
          arma::mat planePoints; planePoints.load(calibrationPath+std::string("_plane.m"));
          arma::mat fgps2 = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation(), rp.gps2.pos).ToArmaMatrix4x4();
          arma::mat tRef2Robot; tRef2Robot.load(calibrationPath+std::string("_robotFrame.m"));
          arma::mat f1; f1.load(calibrationPath+std::string("_GPS2.m"));
          arma::mat corners; corners.load(calibrationPath+std::string("_corners.m"));
          std::tr1::shared_ptr < PlaneCalibrator > planeCalib = recalculatePlanePointsForNewCoordinateFrame(planePoints, f1, tRef2Robot, fgps2, corners);

          corners.save(basePath+std::to_string((_Longlong)i)+std::string("/corners.m"), arma::raw_ascii);
          planeCalib->GetSolution().GetABCD().save(basePath+std::to_string((_Longlong)i)+std::string("/abcd.m"),arma::raw_ascii);
        }
        Vec3d tipPos; Vec3d emPos(0,0,0); Matrix33d tipFrame; Matrix33d emFrame = Matrix33d::I();
        tipCalib->GetSolution(tipPos, tipFrame, emPos, emFrame);
        tipPos=tipPos+Vec3d(0,0,1)*rp.force.slidePosition;

        needlePos = join_vert(needlePos, tipPos.ToArmaVec().t());
        forces = join_vert(forces, rp.force.force.ToArmaVec().t());
        torques = join_vert(torques, rp.force.torque.ToArmaVec().t());
      }

      arma::mat forceTorques = join_horiz(forces, torques);
      forceTorques.save(basePath+std::to_string((_Longlong)i)+std::string("/forceTorques.m"), arma::raw_ascii);
      needlePos.save(basePath+std::to_string((_Longlong)i)+std::string("/needlePos.m"), arma::raw_ascii);
    }
  }
}

TEST(Webcam, Vine)
{
  cv::VideoCapture cam(0);

  if(!cam.isOpened()) {
    return;
  }
  
  cv::Mat im;
  while(true) {
    cam.read(im);
    IplImage imim = (IplImage)im;
    cvShowImage("Yep", &imim);
    im.release();
    cvWaitKey(1000);
  }

=======
#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "RPFileReader.h"
#include "RPFileWriter.h"
#include "RPUlterius.h"
#include "Calibration.h"
#include <memory>
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

using namespace Nf;

typedef struct {
  char ip[100];
  u32 typeMask;
  u32 nframes;
  Vec2d mpp;
  Vec2d origin;
} BasicsRPUlteriusTest;

TEST(IO, UlteriusCaptureRate)
{
	BasicsRPUlteriusTest tests[] = {
    {"192.168.1.129", RPF_BPOST8|RPF_GPS, 200, Vec2d(87.0,87.0), Vec2d(330.0, 78.0)}
	};

	for(s32 n=0; n<sizeof(tests)/sizeof(tests[0]); n++){
		RPUlteriusReaderCollection reader(tests[n].ip, tests[n].mpp, tests[n].origin);

    reader.EnableMask(tests[n].typeMask);

    u32 t0 = GetTickCount();
    s32 i=0;
    printf("\n");
    while(i < tests[n].nframes) {
      RPData curr = reader.GetNextRPData();
      if(curr.FullSet(tests[n].typeMask)) {
        curr.Release();
        i++;
      }
      printf("\rFrame %d", i);
    }
    u32 t1 = GetTickCount();

    f32 msPerFrame = (f32)(t1-t0)/(f32)tests[n].nframes;
    f32 sPerFrame = msPerFrame/1000.0f;
    f32 fps = 1/sPerFrame;

    NTrace("Average fps %f over %d frames\n", fps, tests[n].nframes);
  }
}

TEST(Scratch, ReverseIterator)
{
  std::map < int, int > yep;

  for(s32 i=5; i<20; i++) {
    yep[i] = i;
  }

  for(std::map<int,int>::reverse_iterator i = yep.rbegin(); i!=yep.rend(); i++)
    NTrace("curr %d\n", i->first);
 
  s32 del = 8;

  std::map<int,int>::reverse_iterator i = yep.rbegin();
  for(; i!=yep.rend(); i++) {
    if(i->first == del)
      break;
  }

  
  yep.erase(std::next(i).base());

  NTrace("After deletion of %d\n", del);
  for(std::map<int,int>::reverse_iterator i = yep.rbegin(); i!=yep.rend(); i++)
    NTrace("curr %d\n", i->first);

  for(s32 i=5; i<20; i++) {
    if(i == del)
      assert(yep.find(i) == yep.end());
    else
      assert(yep.find(i) != yep.end());
  }
}

TEST(GPS, Smooth)
{
  using ::s32;

  std::string basePath = BASE_PATH_CAT("1_7_16/05mm/Trial3/scan");

  char temp[200] = {0};
  sprintf(temp, "%s.gps2", basePath.c_str());

  RPGPSReader *gpsReader = new RPGPSReader(temp);
  RPFileHeader header = gpsReader->GetHeader();

  sprintf(temp, "%ssmoothed.gps2", basePath.c_str());
  RPGPSWriter *writer = new RPGPSWriter(temp, &header);

  sprintf(temp, "%ssmoothed.dat", basePath.c_str());
  arma::mat smoothedPoints; smoothedPoints.load(temp);

  for(s32 i=0; i<smoothedPoints.n_rows; i++) {
    arma::vec3 point = (arma::vec3)(smoothedPoints.row(i).t());
    GPS_Data datum = gpsReader->GetNextGPSDatum();
    datum.pos = Vec3d::FromArmaVec(point);
    writer->WriteNextGPSDatum(&datum);
  }

  writer->Cleanup(&header);

  delete writer;
  delete gpsReader;
}

TEST(Command, CorrectTick)
{
  using ::s32;

  f32 aveTimeMs = 180;

  std::string basePath = PATH_CAT("3mm/Trial1/scan");

  char temp[200] = {0};
  sprintf(temp, "%s.gps2", basePath.c_str());

  RPGPSReader *gpsReader = new RPGPSReader(temp);
  RPFileHeader header = gpsReader->GetHeader();
  
  sprintf(temp, "%s.u", basePath.c_str());
  NSCommandReader *commandReader = new NSCommandReader(temp);

  sprintf(temp, "%s_corrected.u", basePath.c_str());
  NSCommandWriter *writer = new NSCommandWriter(temp);

  for(s32 i=0; i<header.frames; i++) {
    NSCommand u = commandReader->GetNextNSCommand();
    u.tick = (u32)(i*aveTimeMs+0.5f);
    writer->WriteNextNSCommand(&u);
  }

  writer->Cleanup();

  delete writer;
  delete gpsReader;
  delete commandReader;
}

static std::tr1::shared_ptr < PlaneCalibrator > recalculatePlanePointsForNewCoordinateFrame(arma::mat pts, arma::mat F1, arma::mat tRef2Robot, arma::mat fgps2, arma::mat &corners)
{
  //pts are \in [p1^T; ...; p_m^T] \in R^{mx3}
  // convert pts to [p1 p2 ... p_m]  \in R^{3xm} for batch transformation
  pts = pts.t();
  //augment with ones for homogenous 
  pts = arma::join_vert(pts, arma::ones(1,pts.n_cols));

  //F1 is frame these points are expressed in
  //tRef2Robot is coordinate frame calibration

  //transform pts to current frame
  pts = tRef2Robot*inv(fgps2)*F1*inv(tRef2Robot)*pts;

  // now convert back to preferred format
  pts = pts.rows(arma::span(0,2));
  pts = pts.t();

  std::tr1::shared_ptr < PlaneCalibrator > planeCalib(new PlaneCalibrator());

  //Set the points
  planeCalib->SetPoints(pts);
  //recalculate plane
  planeCalib->DoCalibration();

  //corners are in format [p1 p2 p3 p4] \in R^{3x4}
  //augment with ones for homogenous
  corners = arma::join_vert(corners, arma::ones(1, corners.n_cols));
  // convert to current coordinate system
  corners = tRef2Robot*inv(fgps2)*F1*inv(tRef2Robot)*corners;

  //now strip ones
  corners = corners.rows(arma::span(0,2));

  return planeCalib;
}

TEST(IO, ForceData)
{
  std::string orientationBasePath = "C:/Joey/Data/ForceData/WithLiver/Orientation";
  std::string baseName = "Trial";
  s32 numOrientations = 3;
  s32 numTrials = 10;

  std::string calibrationBasePath = "Calibration/calib";

  for(s32 j=1; j<=numOrientations; j++) {
    std::string basePath = orientationBasePath+std::to_string((_Longlong)j)+std::string("/")+baseName;
    std::string calibrationPath = orientationBasePath+std::to_string((_Longlong)j)+std::string("/")+calibrationBasePath;
    for(s32 i=1; i<=numTrials; i++) {
      RPFileReaderCollection *rpReaders = new RPFileReaderCollection();

      std::tr1::shared_ptr < ForceReader > forceReader(new ForceReader((basePath+std::to_string((_Longlong)i)+std::string("/scan.force")).c_str()));
      rpReaders->AddForceReader(forceReader.get());
      std::tr1::shared_ptr < RPGPSReader > gpsReader1(new RPGPSReader((basePath+std::to_string((_Longlong)i)+std::string("/scan.gps1")).c_str()));
      rpReaders->AddGPSReader((RPGPSReaderBasic *)gpsReader1.get());
      std::tr1::shared_ptr < RPGPSReader > gpsReader2(new RPGPSReader((basePath+std::to_string((_Longlong)i)+std::string("/scan.gps2")).c_str()));
      rpReaders->AddGPSReader2((RPGPSReaderBasic *)gpsReader2.get());

      arma::mat needlePos;
      arma::mat forces;
      arma::mat torques;

      arma::mat tip; tip.load(calibrationPath+std::string("_tipOffset.m"));
      std::tr1::shared_ptr < EMNeedleTipCalibrator > tipCalib(new EMNeedleTipCalibrator());
      tipCalib->SetSolution(tip);

      RPFileHeader header = gpsReader1->GetHeader();
      for(s32 frame=1; frame<=header.frames; frame++) {
        RPData rp = rpReaders->GetRPData(frame);

        //Reset plane points into coordinate system attached to robot
        if(frame == 1) {
          arma::mat planePoints; planePoints.load(calibrationPath+std::string("_plane.m"));
          arma::mat fgps2 = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation(), rp.gps2.pos).ToArmaMatrix4x4();
          arma::mat tRef2Robot; tRef2Robot.load(calibrationPath+std::string("_robotFrame.m"));
          arma::mat f1; f1.load(calibrationPath+std::string("_GPS2.m"));
          arma::mat corners; corners.load(calibrationPath+std::string("_corners.m"));
          std::tr1::shared_ptr < PlaneCalibrator > planeCalib = recalculatePlanePointsForNewCoordinateFrame(planePoints, f1, tRef2Robot, fgps2, corners);

          corners.save(basePath+std::to_string((_Longlong)i)+std::string("/corners.m"), arma::raw_ascii);
          planeCalib->GetSolution().GetABCD().save(basePath+std::to_string((_Longlong)i)+std::string("/abcd.m"),arma::raw_ascii);
        }
        Vec3d tipPos; Vec3d emPos(0,0,0); Matrix33d tipFrame; Matrix33d emFrame = Matrix33d::I();
        tipCalib->GetSolution(tipPos, tipFrame, emPos, emFrame);
        tipPos=tipPos+Vec3d(0,0,1)*rp.force.slidePosition;

        needlePos = join_vert(needlePos, tipPos.ToArmaVec().t());
        forces = join_vert(forces, rp.force.force.ToArmaVec().t());
        torques = join_vert(torques, rp.force.torque.ToArmaVec().t());
      }

      arma::mat forceTorques = join_horiz(forces, torques);
      forceTorques.save(basePath+std::to_string((_Longlong)i)+std::string("/forceTorques.m"), arma::raw_ascii);
      needlePos.save(basePath+std::to_string((_Longlong)i)+std::string("/needlePos.m"), arma::raw_ascii);
    }
  }
>>>>>>> 5d357b4115338b1acf33585953bc13009af1531d:src/UnitTests/testIO.cpp
}