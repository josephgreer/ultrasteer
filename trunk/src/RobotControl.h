#pragma once
#include "stdafx.h"
#include "vtkQImageToImageSource.h"
#include "vtkSmartPointer.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkThresholdImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryStatisticsOpeningImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkRGBToLuminanceImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkLabelGeometryImageFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "vtkImageLuminance.h"
#include "NeedleSteeringRobot.h"
#include "itkAccumulateImageFilter.h"
#include <QDebug>
#include <time.h>
#include <vnl/vnl_vector.h>
#include "UnscentedKalmanFilter.h"

class Propello;

class RobotControl 
{
public:
    RobotControl(Propello* theParent);
    ~RobotControl();
	void processSweep();
	void InitializeRoll(QString Com);
	void InitializeInsertion(QString Com);
	void setVibration(bool OnOff);
	void InsertIncremental(double inc);
	void SetInsertionVelocity(double v);
	void SetRotationVelocity(double v);
	void RotateIncremental(double inc);
	void homeRoll();
	void homeInsertion();
	void setTargetX(int);
	void setTargetY(int);
	void setTargetZ(int);	
	void enableKalmanFilter(bool);
	void callFunctionDebug();
	void resetSystem();
	
private:
	int processDopplerFrame(double&,double&);
	int processBandDopplerFrames(double&,double&);
	int determineRadiusAndAngle(std::vector<double> &v_scX, std::vector<double> &v_scY, std::vector<double> &v_scZ, double &R, double &del_theta);
	void steeringPlanner(vnl_vector<double> x_est, vnl_vector<double> t, vnl_vector<double> &u);
	void scanConvertPoints(double &scX,double &scY,double &scZ, double imgX, double imgY, int frame);
	void mapRadiusToDC( double RadiusMM, double &DC);
	void measureNeedleTipPose(std::vector<double> &v_scX, std::vector<double> &v_scY, std::vector<double> &v_scZ, vnl_vector<double> &z, vnl_vector<double> &a, vnl_vector<double> &b);

private:
	int m_index;
	int fpV;
	int volumes;
	QImage *m_colorimage;
	QImage *m_Bimage;
	double thetas[100];
	NeedleSteeringRobot m_robot;
	Propello* ParentWindow;
	vnl_vector<double> T;
	bool m_useUKF;
	UnscentedKalmanFilter UKF;
	vnl_vector<double> u;
	int m_stepNumber;
};


