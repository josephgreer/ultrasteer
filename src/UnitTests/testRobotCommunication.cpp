#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "Segment.h"
#include "NeedleSteeringRobot.h"
#include "qtimer.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

using namespace Nf;

TEST(Basic, Robot)
{
  NTrace("Initializing robot\n");

  // Create a robot
  NeedleSteeringRobot robot;
  // Initialize roll
  robot.InitializeRoll("COM5");

  NTrace("Homing insertion\n");
  
  // Initialize insertion
  robot.InitializeInsertion("COM7");
  // Wait for robot to home
  Sleep(5000);
  
  NTrace("Moving robot\n");
  
  // Move insertion in a bit
  robot.InsertIncremental(15.0);
  // Move rotation around a bit
  robot.RotateIncremental(5.0);
  // Wait for commands to go through
  Sleep(2000);

  NTrace("Starting a query timer\n");
/*
  QTimer* timer = new QTimer;
  connect(timer, SIGNAL(timeout()), 


    connect(m_preScanTimer.get(), SIGNAL(timeout()), this, SLOT(onManualTimeout()));
*/
  
  
  NTrace("Polling rotation and insertion\n");
  
  int i = 0;
  // Continuously query current position
  while(true){
    BEGIN_TIMING(QueryJoints, 50)
      NTrace("th =  %f\n", robot.getRollAngle());
    NTrace("l =  %f\n", robot.getInsMM());
    END_TIMING(QueryJoints, 50)
      i++;

    //if( i%50 == 0 ){
    //  if( i%100 == 0 ){
    //    robot.RotateIncremental(45.0);
    //  }else{
    //    robot.RotateIncremental(-45.0);
    //  }
    //}    

    //if( i%100 == 0 ){
    //  if( i%200 == 0 ){
    //    robot.InsertIncremental(10.0);
    //  }else{ 
    //    robot.InsertIncremental(-10.0);
    //  }
    //}
  }

}

