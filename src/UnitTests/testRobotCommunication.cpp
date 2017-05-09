#include "CppUnitLite/Test.h"
#include "RTCore.h"
#include "Segment.h"
#include "NeedleSteeringRobot.h"
#include "qtimer.h"
#include "ControlAlgorithms.h"

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

TEST(Basic, Insertion)
{
  NTrace("Initializing robot\n");

  // Create a robot
  NeedleSteeringRobot robot;
  // Initialize roll
  robot.InitializeRoll("COM7");

  //NTrace("Homing insertion\n");
  //
  //// Initialize insertion
  //robot.InitializeInsertion("COM6");
  //// Wait for robot to home
  //Sleep(5000);

  for(s32 i=0; i<10; i++) {

    NTrace("Moving robot\n");

    //robot.SetRotationVelocity(120);
    robot.SetInsertionVelocity(1);
    //robot.GetInsertionDevice()->SetVelocity(139);

    Sleep(10000);

    //robot.SetInsertionVelocity(0);
    robot.SetRotationVelocity(0);
    //robot.GetInsertionDevice()->SetVelocity(0);

    Sleep(1000);


    //NTrace("Raw encoder ticks %d\n", robot.GetInsertionDevice()->GetPosition());


    NTrace("Insertion amount: %f\n", robot.getInsMM());
  }

}

