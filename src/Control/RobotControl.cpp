#include "stdafx.h"
#include "RobotControl.h"
#include "NeedleSteeringRobot.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_svd.h>
#include "Propello.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkRegionalMaximaImageFilter.h"
#include "itkNormalizedCorrelationImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkImageKernelOperator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkLaplacianRecursiveGaussianImageFilter.h"
#include "itkCastImageFilter.h"
#include "QuickView.h"
#include <sstream>


#define PI 3.14159265
#define NEEDLE_MIN_R 51.4
#define MIN_DC 0.0
#define MAX_DC 1.0
#define DCINSERTION 5.0
#define N_INIT	3


typedef itk::Image< unsigned char, 2 > ImageType;
typedef itk::Image<float, 2> FloatImageType;

static void CreateKernel(FloatImageType::Pointer kernel);
static void ShowPoint(ImageType::Pointer Image,int x, int y);

RobotControl::RobotControl(Propello* theParent): ParentWindow(theParent)

{
	m_colorimage = 0;
	m_Bimage = 0;
	m_index = 0;
	T.set_size(3);
	u.set_size(3);
	u.put(0,0);
	u.put(1,NEEDLE_MIN_R);
	u.put(2,0);
	m_useUKF = true;
	m_stepNumber = 1;
}

RobotControl::~RobotControl()
{
}

void RobotControl::InitializeRoll(QString Com)
{
	m_robot.InitializeRoll(Com);
}

void RobotControl::InitializeInsertion(QString Com)
{
	m_robot.InitializeInsertion(Com);
}

/*
* This function will connect the given vtkImageExport filter to
* the given itk::VTKImageImport filter.
*/
template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
	importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
	importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
	importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
	importer->SetSpacingCallback(exporter->GetSpacingCallback());
	importer->SetOriginCallback(exporter->GetOriginCallback());
	importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
	importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
	importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
	importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
	importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
	importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
	importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

void RobotControl::processSweep(void)
{
	// intialize variables to characterize sweep
	int i, frames = portaGetDisplayFrameCount(0);
	fpV = portaGetParam(prmMotorFrames);
	volumes = frames / fpV;

	//Open files to write results to for offline analysis
	ofstream impts_file, scpts_file, x_file, z_file, u_file, a_file, b_file, t_file;

	std::ostringstream impts_name, scpts_name, x_name, z_name, u_name, a_name, b_name, t_name;
	impts_name << "impts_" << m_stepNumber << ".txt";
	scpts_name << "scpts_" << m_stepNumber << ".txt";
	x_name << "x_" << m_stepNumber << ".txt";
	z_name << "z_" << m_stepNumber << ".txt";
	u_name << "u_" << m_stepNumber << ".txt";
	a_name << "a_" << m_stepNumber << ".txt";
	b_name << "b_" << m_stepNumber << ".txt";
	t_name << "t_" << m_stepNumber << ".txt";
		
	impts_file.open(impts_name.str());
	scpts_file.open (scpts_name.str());
	x_file.open (x_name.str());
	z_file.open (z_name.str());
	u_file.open (u_name.str());
	a_file.open (a_name.str());
	b_file.open (b_name.str());
	t_file.open (t_name.str());

	// initialize QImages to hold data
	int w,h;
	portaGetDisplayDimensions(0, w, h);
	m_colorimage = new QImage(w, h, QImage::Format_RGB32);
	m_Bimage = new QImage(w, h, QImage::Format_RGB32);
	unsigned char* buffer;
	unsigned char* Bbuffer;
	buffer = m_colorimage->bits();
	Bbuffer = m_Bimage->bits();
	std::vector<double> v_scX, v_scY, v_scZ;

	// clocks for timing
	clock_t start, finish;

	// process each image in turn
	for (i = 4; i < (volumes * fpV); i++)
	{
		portaProcessCineImage(0, i);
		bool ret = portaGetColorImageOnly (m_index, buffer);
		ret = portaGetBwImage(m_index, Bbuffer, true);
		int frameNumber = i+1;
		double imgX,imgY; // image coordinates
		double scX,scY,scZ;	// scan-converted cartesian coordinates
		// start a timer
		start = clock();
		bool DopplerFound = processDopplerFrame(imgX, imgY);
		finish = clock();
		double runtime = (finish - start)/double(CLOCKS_PER_SEC);
		if(DopplerFound)
		{
			// Scan convert the point
			scanConvertPoints(scX,scY,scZ,imgX,imgY,frameNumber);
			// Write the point to debug output
			qDebug() << "imgX =" << imgX << "imgY" << imgY;
			qDebug() << "scX =" << scX << "scY =" << scY << "scZ =" << scZ;
			// Write the point to files
			impts_file << imgX << "," << imgY<< "," << frameNumber << ",";
			scpts_file << scX << "," << scY << "," << scZ << ",";
			// Put the scan converted points into dynamic vectors for storage
			v_scX.push_back(scX);
			v_scY.push_back(scY);
			v_scZ.push_back(scZ);
		}
	}
	// Produce a steerable needle tip pose "measurement" based on the scan converted points
	vnl_vector<double> z(6), a(4), b(4);
	measureNeedleTipPose(v_scX, v_scY, v_scZ, z, a, b);		
	z_file << z[0] << "," << z[1] << "," << z[2] << "," << z[3] << "," << z[4] << "," << z[5];
	a_file << a[0] << "," << a[1] << "," << a[2] << "," << a[3];
	b_file << b[0] << "," << b[1] << "," << b[2] << "," << b[3];
	t_file << T[0] << "," << T[1] << "," << T[2];

	// Print the measurement to screen
	qDebug() << "z(" << m_stepNumber << ") = [" << z[0] << "," << z[1] << "," << z[2] << "," 
		<< z[3] << "," << z[4] << "," << z[5] << "]";

	// Update the Kalman filter with the current measurement and the previous command
	UKF.updateUKF(u, z);
	
	// Declare boolean to check if the needle tip is past the target
	bool past;
	
	// Decide whether we will use the Kalman filter estimate or the measurements directly
	if (m_useUKF)
	{
		// for the first few steps, zero the orientation estimate, since it's bad info
		if ( m_stepNumber <= N_INIT)
		{
			UKF.zeroRotationEstimate();
		}
		// use the UKF
		vnl_vector<double> x_est;
		UKF.getCurrentStateEstimate(x_est);
		steeringPlanner(x_est,T,u);
		x_file << x_est[0] << "," << x_est[1] << "," << x_est[2] << "," 
			<< x_est[3] << "," << x_est[4] << "," << x_est[5];

		// Print the estimate to screen
		qDebug() << "x_est(" << m_stepNumber << ") = [" << x_est[0] << "," << x_est[1] << "," << x_est[2] << "," 
			<< x_est[3] << "," << x_est[4] << "," << x_est[5] << "]";

		// Check if the needle is past the target
		past = x_est[2] >= T[2];

	}
	else
	{
		// use the measurements directly
		steeringPlanner(z,T,u);
		x_file << z[0] << "," << z[1] << "," << z[2] << "," 
			<< z[3] << "," << z[4] << "," << z[5];

		// Check if the needle is past the target
		past = z[2] >= T[2];
	}

	// Limit the insertion increment so the next increment will recieve the correct command
	if( u[2] > DCINSERTION )
		u[2] = DCINSERTION;

	// For the first few scans, zero the command so we just improve the filter's estimate
	if ( m_stepNumber <= N_INIT)
	{
		u[0] = 0;
		u[2] = 0;
	}

	// Print the command to screen and to file
	qDebug() << "u(" << m_stepNumber << ") = [" << u[0] << "," << u[1] << "," << u[2] << "]";
	u_file << u[0] << "," << u[1] << "," << u[2];

	// Seperate the individual commands for the next insertion
	double d_th = u.get(0), ro = u.get(1), l = u.get(2);
	start = clock();
	finish = clock();
	double runtime = (finish - start)/double(CLOCKS_PER_SEC);
	if( !past )
	{
		// Insert the robot towards the target
		double ang_command = fmod(m_robot.getRollAngle(),360.0) - (180.0*d_th/PI); 
		// negative d_th b/c tip frame and robot have opposite signs

		// Map the desired radius into duty cycle
		double DCcommand;
		mapRadiusToDC( ro, DCcommand);
		
		// Determine how much to insert
		double insertionDepth;
		if( l > DCINSERTION )
		{ insertionDepth = DCINSERTION; }
		else
		{ insertionDepth = l; }
		m_robot.DutyCycleSteer(DCcommand,ang_command,insertionDepth);
	}
	else
	{
		ParentWindow->DisplayErrorBox(1);
	}

	// delete temp image.
	delete m_colorimage;

	// close all the files
	impts_file.close();
	scpts_file.close();
	x_file.close();
	z_file.close();
	u_file.close();
	a_file.close();
	b_file.close();
	t_file.close();

	// increase index for file numbering
	m_stepNumber = m_stepNumber + 1;
}

void RobotControl::scanConvertPoints(double &scX, double &scY, double &scZ, double imgX, double imgY, int frame)
{
	// Determine the incremental frame position in number of frames
	int position = frame % (2*fpV);

	// Calculate degrees between frames
	probeInfo nfo;
	portaGetProbeInfo(nfo);
	int steps = portaGetParam(prmMotorSteps);
	double degPerStep = (double)nfo.motorFov / (double)nfo.motorSteps / 1000.0;
	double degPerFrm = double(steps)*degPerStep;

	// Calculate angular position in degrees; 0 deg is straight down
	double theta;
	int middleFrm = ceil(double(fpV)/2.0); 
	if (position < fpV)
		theta = (position - middleFrm)*degPerFrm; 
	else
		theta = (middleFrm - (position-fpV) )*degPerFrm;

	// Get origin and pixel size for image
	int mX,mY,oX,oY;
	portaGetMicronsPerPixel(m_index,mX,mY);
	portaGetPixelCoordinates(m_index, 64, 1, oX, oY, 0);

	// Get image dimensions
	int w,h;
	portaGetDisplayDimensions(0, w, h);

	// Calculate cartesian position of the points in mm
	scX = (imgX-oX)*double(-mX/1000.0);
	double imgY_mm = ((h-imgY)-oY)*double(mY/1000.0);
	double probeRadius = nfo.motorRadius / 1000.0;
	scY = (probeRadius + imgY_mm)*cos(theta*PI/180);
	scZ = (probeRadius + imgY_mm)*sin(theta*PI/180);
}

int RobotControl::processDopplerFrame(double &x, double &y)
{
	clock_t start = clock();
	try
	{
		if (portaGetCurrentMode() == ColourMode)
		{
			if(m_colorimage)
			{
				// Read the QImage data taken from propello using vtk
				vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource =
					vtkSmartPointer<vtkQImageToImageSource>::New();
				qimageToImageSource->SetQImage(m_colorimage);
				qimageToImageSource->Update();

				// Set up an itk RGBA image to read the color propello image
				typedef itk::RGBAPixel< unsigned char > PixelType;
				typedef itk::Image<PixelType, 2> InputType;
				typedef itk::Image< unsigned char, 2 > ImageType;

				// Connect vtk to itk, get result as an RGB itk image
				vtkImageExport* vtkExporter = vtkImageExport::New();
				vtkExporter->SetInput(qimageToImageSource->GetOutput());

				typedef itk::VTKImageImport<InputType> ImageImportType;
				ImageImportType::Pointer itkImporter = ImageImportType::New();
				ConnectPipelines(vtkExporter, itkImporter);

				InputType::Pointer RGBImage = InputType::New();
				RGBImage = itkImporter->GetOutput();
				RGBImage->Update();

				// Convert RGB image to intensity image for processing
				typedef itk::RGBToLuminanceImageFilter<InputType, ImageType> LuminanceFilterType;
				LuminanceFilterType::Pointer LuminanceFilter = LuminanceFilterType::New();
				LuminanceFilter->SetInput(RGBImage);

				ImageType::Pointer Image = ImageType::New();
				Image = LuminanceFilter->GetOutput();
				Image->Update();

				// Define a region that will surround the entire image
				ImageType::RegionType region;
				ImageType::IndexType start;
				start[0] = 0;
				start[1] = 0;
				ImageType::SizeType size;
				size[0] = Image->GetLargestPossibleRegion().GetSize()[0];
				size[1] = Image->GetLargestPossibleRegion().GetSize()[1];
				region.SetSize(size);
				region.SetIndex(start);

				// Threshold the intensity image
				typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> 
					BinaryThresholdImageFilterType;
				BinaryThresholdImageFilterType::Pointer thresholdFilter
					= BinaryThresholdImageFilterType::New();
				thresholdFilter->SetInput(Image);
				int lowerThreshold = 5;
				thresholdFilter->SetLowerThreshold(lowerThreshold);
				thresholdFilter->SetUpperThreshold(255);
				thresholdFilter->SetInsideValue(255);
				thresholdFilter->SetOutsideValue(0);
				thresholdFilter->Update();

				// Remove groups with less than 300 pixels
				typedef itk::BinaryStatisticsOpeningImageFilter<ImageType, ImageType> 
					BinaryOpeningType;
				BinaryOpeningType::Pointer opening = BinaryOpeningType::New();
				opening->SetInput(thresholdFilter->GetOutput());
				opening->SetFeatureImage(Image);
				opening->SetBackgroundValue(0);
				opening->SetForegroundValue(255);
				opening->SetLambda(300);
				opening->SetAttribute( BinaryOpeningType::LabelObjectType::NUMBER_OF_PIXELS);
				opening->Update();

				// Apply the mask defined by the previous two steps to the original intensity image
				typedef itk::MaskImageFilter< ImageType, ImageType > MaskFilterType;
				MaskFilterType::Pointer maskFilter = MaskFilterType::New();
				maskFilter->SetInput(Image);
				maskFilter->SetMaskImage(opening->GetOutput());
				maskFilter->Update();
				ImageType::Pointer finalResultImage = maskFilter->GetOutput();

				// Calculate the centroid of the resulting Doppler data
				ImageType::Pointer binaryMaskImage = ImageType::New();
				binaryMaskImage->SetRegions(region);
				binaryMaskImage->Allocate();

				typedef itk::BinaryImageToLabelMapFilter<ImageType> BinaryImageToLabelMapFilterType;
				BinaryImageToLabelMapFilterType::Pointer binaryImageToLabelMapFilter = BinaryImageToLabelMapFilterType::New();
				binaryImageToLabelMapFilter->SetInput(binaryMaskImage);
				binaryImageToLabelMapFilter->Update();

				typedef itk::LabelMapToLabelImageFilter<BinaryImageToLabelMapFilterType::OutputImageType, ImageType> LabelMapToLabelImageFilterType;
				LabelMapToLabelImageFilterType::Pointer labelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
				labelMapToLabelImageFilter->SetInput(binaryImageToLabelMapFilter->GetOutput());
				labelMapToLabelImageFilter->Update();

				typedef itk::LabelGeometryImageFilter< LabelMapToLabelImageFilterType::OutputImageType, ImageType > LabelGeometryImageFilterType;
				LabelGeometryImageFilterType::Pointer labelGeometryImageFilter = LabelGeometryImageFilterType::New();
				labelGeometryImageFilter->SetInput( labelMapToLabelImageFilter->GetOutput() );
				labelGeometryImageFilter->SetIntensityInput( finalResultImage );
				labelGeometryImageFilter->CalculatePixelIndicesOn();
				labelGeometryImageFilter->Update();
				LabelGeometryImageFilterType::LabelsType allLabels = labelGeometryImageFilter->GetLabels();

				// Find the quarter point using an image iterator
				int y_quarter = 0;
				long totalMass = 0;
				double IntegratedIntensity = labelGeometryImageFilter->GetIntegratedIntensity(*(allLabels.begin()));
				int h = region.GetSize()[1];
				int* RowMass = new int[h]();
				int* CumRowMass = new int[h]();
				itk::ImageRegionIterator<ImageType> imageIterator(finalResultImage,finalResultImage->GetLargestPossibleRegion());
				while(!imageIterator.IsAtEnd())
				{
					int CurrentRow = imageIterator.GetIndex()[1];
					ImageType::PixelType grayPixel;
					grayPixel = imageIterator.Get();
					totalMass += grayPixel;
					RowMass[CurrentRow] = RowMass[CurrentRow] + grayPixel;
					if(RowMass[CurrentRow])
					{
						int Troy = RowMass[CurrentRow];
						int Debug = 0;
					}
					++imageIterator;
				}
				CumRowMass[0] = RowMass[0];
				for(int i = 0; i < h-1 ; i++)
				{
					int CurrentRowMass = RowMass[i+1] + CumRowMass[i];
					CumRowMass[i+1] = CurrentRowMass;
					if (   (CurrentRowMass > int(3*totalMass/4)) & !y_quarter   )
					{
						y_quarter = i+1;
					}

					qDebug() << "Row: " << i << "Val: " << RowMass[i] << "Cum: " << CumRowMass[i] << "yq: " << y_quarter;
				}
				delete [] RowMass;
				delete [] CumRowMass;
				vtkExporter->Delete();
				// Display the result in the debugger, if there's at least some Doppler response
				//qDebug() << "Integrated intensity =" << IntegratedIntensity;
				if (IntegratedIntensity > 0)
				{
					x = labelGeometryImageFilter->GetWeightedCentroid(*(allLabels.begin()))[0];
					//y = labelGeometryImageFilter->GetWeightedCentroid(*(allLabels.begin()))[1];
					y = (double) y_quarter;
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	catch ( itk::ExceptionObject &err)
	{
		qDebug() << "ExceptionObject caught !";
		qDebug() << err.what();
	}
	catch (const std::exception &exc)
	{
		qDebug() << exc.what();
	}
	catch( ... )
	{
		qDebug() << "Caught an unknown error";
	}
	// Print the runtime for the analysis
	clock_t finish = clock();
	double runtime = (finish - start)/double(CLOCKS_PER_SEC);
	qDebug() << "runtime  =" << runtime << "s";
}

int RobotControl::processBandDopplerFrames(double &x, double &y)
{
	clock_t start = clock();
	try
	{
		if (portaGetCurrentMode() == ColourMode)
		{
			if(m_colorimage && m_Bimage)
			{
				// Read the QImage data taken from propello using vtk
				vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource_C =
					vtkSmartPointer<vtkQImageToImageSource>::New();
				qimageToImageSource_C->SetQImage(m_colorimage);
				qimageToImageSource_C->Update();

				vtkSmartPointer<vtkQImageToImageSource> qimageToImageSource_B =
					vtkSmartPointer<vtkQImageToImageSource>::New();
				qimageToImageSource_B->SetQImage(m_Bimage);
				qimageToImageSource_B->Update();

				// Set up itk RGBA image to read the color and B-mode propello images
				typedef itk::RGBAPixel< unsigned char > PixelType;
				typedef itk::Image<PixelType, 2> InputType;
				
				// Connect vtk to itk, get results as RGB itk images
				vtkImageExport* vtkExporter_C = vtkImageExport::New();
				vtkImageExport* vtkExporter_B = vtkImageExport::New();
				vtkExporter_C->SetInput(qimageToImageSource_C->GetOutput());
				vtkExporter_B->SetInput(qimageToImageSource_B->GetOutput());

				typedef itk::VTKImageImport<InputType> ImageImportType;
				ImageImportType::Pointer itkImporter_C = ImageImportType::New();
				ImageImportType::Pointer itkImporter_B = ImageImportType::New();
				ConnectPipelines(vtkExporter_C, itkImporter_C);
				ConnectPipelines(vtkExporter_B, itkImporter_B);

				InputType::Pointer RGBImage_C = InputType::New();
				RGBImage_C = itkImporter_C->GetOutput();
				RGBImage_C->Update();

				InputType::Pointer RGBImage_B = InputType::New();
				RGBImage_B = itkImporter_B->GetOutput();
				RGBImage_B->Update();

				// Convert RGB images to intensity images for processing
				typedef itk::RGBToLuminanceImageFilter<InputType, ImageType> LuminanceFilterType;
				LuminanceFilterType::Pointer LuminanceFilter_C = LuminanceFilterType::New();
				LuminanceFilter_C->SetInput(RGBImage_C);

				ImageType::Pointer Image_C = ImageType::New();
				Image_C = LuminanceFilter_C->GetOutput();
				Image_C->Update();

				LuminanceFilterType::Pointer LuminanceFilter_B = LuminanceFilterType::New();
				LuminanceFilter_B->SetInput(RGBImage_B);

				ImageType::Pointer Image_B = ImageType::New();
				Image_B = LuminanceFilter_B->GetOutput();
				Image_B->Update();

				// Define a region that will surround the entire color image
				ImageType::RegionType region;
				ImageType::IndexType start;
				start[0] = 0;
				start[1] = 0;
				ImageType::SizeType size;
				size[0] = Image_C->GetLargestPossibleRegion().GetSize()[0];
				size[1] = Image_C->GetLargestPossibleRegion().GetSize()[1];
				region.SetSize(size);
				region.SetIndex(start);

				// Threshold the intensity Doppler image
				typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> 
					BinaryThresholdImageFilterType;
				BinaryThresholdImageFilterType::Pointer thresholdFilter
					= BinaryThresholdImageFilterType::New();
				thresholdFilter->SetInput(Image_C);
				int lowerThreshold = 5;
				thresholdFilter->SetLowerThreshold(lowerThreshold);
				thresholdFilter->SetUpperThreshold(255);
				thresholdFilter->SetInsideValue(255);
				thresholdFilter->SetOutsideValue(0);
				thresholdFilter->Update();

				// Remove groups with less than 300 pixels
				typedef itk::BinaryStatisticsOpeningImageFilter<ImageType, ImageType> 
					BinaryOpeningType;
				BinaryOpeningType::Pointer opening = BinaryOpeningType::New();
				opening->SetInput(thresholdFilter->GetOutput());
				opening->SetFeatureImage(Image_C);
				opening->SetBackgroundValue(0);
				opening->SetForegroundValue(255);
				opening->SetLambda(300);
				opening->SetAttribute( BinaryOpeningType::LabelObjectType::NUMBER_OF_PIXELS);
				opening->Update();

				// Dilate the mask
				typedef itk::BinaryBallStructuringElement<ImageType::PixelType,2> StructuringElementType;
				StructuringElementType structuringElement;
				structuringElement.SetRadius(15);
				structuringElement.CreateStructuringElement();

				typedef itk::BinaryDilateImageFilter <ImageType, ImageType, StructuringElementType>
					BinaryDilateImageFilterType;

				BinaryDilateImageFilterType::Pointer dilateFilter = BinaryDilateImageFilterType::New();
				dilateFilter->SetInput(opening->GetOutput());
				dilateFilter->SetKernel(structuringElement);

				// Apply the mask defined by the previous two steps to the original B-mode image 
				typedef itk::MaskImageFilter< ImageType, ImageType > MaskFilterType;
				MaskFilterType::Pointer maskFilter = MaskFilterType::New();
				maskFilter->SetInput(Image_B);
				maskFilter->SetMaskImage(dilateFilter->GetOutput());
				maskFilter->Update();
				ImageType::Pointer finalResultImage = maskFilter->GetOutput();

				// Apply a correlation kernel to detect the needle
				typedef itk::CastImageFilter<ImageType,FloatImageType> CastFilterType;
				typedef itk::MinimumMaximumImageCalculator <ImageType>
					MinimumMaximumImageCalculatorType;

				MinimumMaximumImageCalculatorType::Pointer minimumMaximumImageCalculatorFilter
					= MinimumMaximumImageCalculatorType::New ();
				minimumMaximumImageCalculatorFilter->SetImage(finalResultImage);
				minimumMaximumImageCalculatorFilter->Compute();

				itk::Index<2> maximumCorrelationPatchCenter = minimumMaximumImageCalculatorFilter->GetIndexOfMaximum();
				ShowPoint(finalResultImage,maximumCorrelationPatchCenter[0],maximumCorrelationPatchCenter[1] );
				QuickView viewer;

				// Calculate the centroid of the resulting Doppler data
				ImageType::Pointer binaryMaskImage = ImageType::New();
				binaryMaskImage->SetRegions(region);
				binaryMaskImage->Allocate();

				typedef itk::BinaryImageToLabelMapFilter<ImageType> BinaryImageToLabelMapFilterType;
				BinaryImageToLabelMapFilterType::Pointer binaryImageToLabelMapFilter = BinaryImageToLabelMapFilterType::New();
				binaryImageToLabelMapFilter->SetInput(binaryMaskImage);
				binaryImageToLabelMapFilter->Update();

				typedef itk::LabelMapToLabelImageFilter<BinaryImageToLabelMapFilterType::OutputImageType, ImageType> LabelMapToLabelImageFilterType;
				LabelMapToLabelImageFilterType::Pointer labelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
				labelMapToLabelImageFilter->SetInput(binaryImageToLabelMapFilter->GetOutput());
				labelMapToLabelImageFilter->Update();

				typedef itk::LabelGeometryImageFilter< LabelMapToLabelImageFilterType::OutputImageType, ImageType > LabelGeometryImageFilterType;
				LabelGeometryImageFilterType::Pointer labelGeometryImageFilter = LabelGeometryImageFilterType::New();
				labelGeometryImageFilter->SetInput( labelMapToLabelImageFilter->GetOutput() );
				labelGeometryImageFilter->SetIntensityInput( finalResultImage );
				labelGeometryImageFilter->CalculatePixelIndicesOn();
				labelGeometryImageFilter->Update();
				LabelGeometryImageFilterType::LabelsType allLabels = labelGeometryImageFilter->GetLabels();

				// Find the quarter point using an image iterator
				int y_quarter = 0;
				long totalMass = 0;
				double IntegratedIntensity = labelGeometryImageFilter->GetIntegratedIntensity(*(allLabels.begin()));
				int h = region.GetSize()[1];
				int* RowMass = new int[h]();
				int* CumRowMass = new int[h]();
				itk::ImageRegionIterator<ImageType> imageIterator(finalResultImage,finalResultImage->GetLargestPossibleRegion());
				while(!imageIterator.IsAtEnd())
				{
					int CurrentRow = imageIterator.GetIndex()[1];
					ImageType::PixelType grayPixel;
					grayPixel = imageIterator.Get();
					totalMass += grayPixel;
					RowMass[CurrentRow] = RowMass[CurrentRow] + grayPixel;
					if(RowMass[CurrentRow])
					{
						int Troy = RowMass[CurrentRow];
						int Debug = 0;
					}
					++imageIterator;
				}
				CumRowMass[0] = RowMass[0];
				for(int i = 0; i < h-1 ; i++)
				{
					int CurrentRowMass = RowMass[i+1] + CumRowMass[i];
					CumRowMass[i+1] = CurrentRowMass;
					if (   (CurrentRowMass > int(3*totalMass/4)) & !y_quarter   )
					{
						y_quarter = i+1;
					}

					qDebug() << "Row: " << i << "Val: " << RowMass[i] << "Cum: " << CumRowMass[i] << "yq: " << y_quarter;
				}
				delete [] RowMass;
				delete [] CumRowMass;
				// Display the result in the debugger, if there's at least some Doppler response
				//qDebug() << "Integrated intensity =" << IntegratedIntensity;
				if (IntegratedIntensity > 0 && maximumCorrelationPatchCenter[0]!=0 && maximumCorrelationPatchCenter[0]!=1 )
				{
					//x = labelGeometryImageFilter->GetWeightedCentroid(*(allLabels.begin()))[0];
					//// = labelGeometryImageFilter->GetWeightedCentroid(*(allLabels.begin()))[1];
					//y = (double) y_quarter;
					x = maximumCorrelationPatchCenter[0];
					y = maximumCorrelationPatchCenter[1];
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	catch ( itk::ExceptionObject &err)
	{
		qDebug() << "ExceptionObject caught !";
		qDebug() << err.what();
	}
	catch (const std::exception &exc)
	{
		qDebug() << exc.what();
	}
	catch( ... )
	{
		qDebug() << "Caught an unknown error";
	}
	// Print the runtime for the analysis
	clock_t finish = clock();
	double runtime = (finish - start)/double(CLOCKS_PER_SEC);
	qDebug() << "runtime  =" << runtime << "s";
}

void ShowPoint(ImageType::Pointer Image,int x, int y)
{
	itk::ImageRegionIterator<ImageType> imageIterator(Image,Image->GetLargestPossibleRegion());

	while(!imageIterator.IsAtEnd())
	{
		/*if( ((imageIterator.GetIndex()[0]-x)^2 + (imageIterator.GetIndex()[1]-y)^2) <= (10)  )
		*/
		if( abs(imageIterator.GetIndex()[0]-x)<3 && abs(imageIterator.GetIndex()[1]-y)<3  )
		{
			imageIterator.Set(255);
		}

		++imageIterator;
	}
	
}

int RobotControl::determineRadiusAndAngle(std::vector<double> &v_scX, std::vector<double> &v_scY, std::vector<double> &v_scZ, double &R, double &del_theta)
{
	//Open file to write point to for validation
	ofstream polyfile, vectorfile;
	polyfile.open ("polynomialCoefficients.txt");
	vectorfile.open("vectors.txt");

	// Transfer to vnl vectors, and define matrix for polynomial fit
	int N = v_scX.size();
	// If there aren't any Doppler points get out
	if (N <= 0)
		return 0;
	// Otherwise continue with analysis

	// Define vectors for polynomial fit
	vnl_vector<double> v_X(N), v_Y(N), v_Z(N);
	vnl_vector<double> a(4), b(4);
	vnl_matrix<double> A(N,4);
	for (int i = 0; i < N; i++)
	{
		v_X[i] = v_scX[i]; v_Y[i] = v_scY[i]; v_Z[i] = v_scZ[i];
		A(i,0) = 1; A(i,1) = v_Z[i]; 
		A(i,2) = v_Z[i]*v_Z[i]; A(i,3) = v_Z[i]*v_Z[i]*v_Z[i];
	}

	// Define vectors for linear fit to last M points
	int M = N;
	if (N > 10)
		M = 10;
	vnl_vector<double> v_Xl(M), v_Yl(M), v_Zl(M);
	vnl_vector<double> a_l(2), b_l(2);
	vnl_matrix<double> A_l(M,2);
	for (int i = 0; i < M; i++)
	{
		v_Xl[i] = v_scX[i]; v_Yl[i] = v_scY[i]; v_Zl[i] = v_scZ[i];
		A_l(i,0) = 1; A_l(i,1) = v_Zl[i];
	}

	// Perform least-squares polynomial fit for order 3
	vnl_matrix<double> Adag(4,N), At(N,4);
	At = vnl_transpose(A);
	Adag = vnl_matrix_inverse<double>(At*A)*At;
	a = Adag*v_X;
	b = Adag*v_Y;

	// Perform least-squares polynomial fit for order 1
	vnl_matrix<double> A_ldag(2,N), A_lt(N,2);
	A_lt = vnl_transpose(A_l);
	A_ldag = vnl_matrix_inverse<double>(A_lt*A_l)*A_lt;
	a_l = A_ldag*v_Xl;
	b_l = A_ldag*v_Yl;

	// Find the tip
	vnl_vector<double> tip(3);
	double z_tip = v_Z[0];
	tip[2] = z_tip;
	tip[0] = a[0] + a[1]*z_tip + a[2]*pow(z_tip,2) + a[3]*pow(z_tip,3); 
	tip[1] = b[0] + b[1]*z_tip + b[2]*pow(z_tip,2) + b[3]*pow(z_tip,3);

	// Find the equation of the tangent line at the tip
	double mx, my, bx, by;
	mx = a[1] + 2*a[2]*z_tip + 3*a[3]*pow(z_tip,2); 
	my = b[1] + 2*b[2]*z_tip + 3*b[3]*pow(z_tip,2);
	bx = tip[0] - mx*tip[2];
	by = tip[1] - my*tip[2];

	// Find a point 10 mm along the tangent line from the tip
	vnl_vector<double> p_tan(3);
	p_tan[2] = v_Z[0] - 10;
	p_tan[0] = mx*p_tan[2] + bx;
	p_tan[1] = my*p_tan[2] + by;
	//p_tan[0] = a_l[1]*(-10) + v_X[0];
	//p_tan[1] = b_l[1]*(-10) + v_Y[0];
	double insertionDist = v_Z[N-1] - v_Z[0];
	if(insertionDist < 12.0) // If we're not very far into the tissue
	{
		p_tan[2] = tip[2] - 10; // Assume the tangent line is straight along the Z-axis
		p_tan[0] = tip[0];		// from the tip.
		p_tan[1] = tip[1];
	}

	// Define the plane containing the tangent line and the target
	vnl_vector<double> v1_target, v2_target, n_target;
	v1_target = T - tip;
	v2_target = p_tan - tip;
	n_target = vnl_cross_3d(v2_target,v1_target);
	n_target = n_target.normalize();

	// Define the plane containing the needle
	// Initialize the orientation of the needle plane normal
	// Assumes bevel tip is always initially facing down
	vnl_vector<double> n_needle_orig(3), n_needle(3);
	n_needle_orig[0] = 1; n_needle_orig[1] = 0; n_needle_orig[2] = 0;

	double needle_ang = fmod(m_robot.getRollAngle(),360.0);
	vnl_quaternion<double> needleRot(-(v2_target.normalize()),(needle_ang*PI/180.0));
	n_needle = needleRot.rotate(n_needle_orig);

	// Calculate the angle between the normals
	double del_theta_rad = fabs(acos( dot_product(n_needle,n_target) ));

	// To determine the sign of the rotation in robot coordinates, compare positive,
	// and negative sign using axis-angle rotation around needle tip
	vnl_quaternion<double> posRot(-(v2_target.normalize()),del_theta_rad);
	posRot = posRot.normalize();
	vnl_vector<double> posErrorVec = posRot.rotate(n_needle)-n_target;
	vnl_quaternion<double> negRot(-(v2_target.normalize()),-del_theta_rad);
	negRot = negRot.normalize();
	vnl_vector<double> negErrorVec = negRot.rotate(n_needle)-n_target;
	double posErrorNorm, negErrorNorm;
	posErrorNorm = posErrorVec.two_norm();
	negErrorNorm = negErrorVec.two_norm();
	if(posErrorNorm > negErrorNorm)
		del_theta = -1.0*del_theta_rad*180.0/PI;
	else
		del_theta = del_theta_rad*180.0/PI;

	// Find the radial vector (perpendicular to the tangent and target normals
	vnl_vector<double> n_radial;
	n_radial = vnl_cross_3d(v2_target,n_target);
	if( dot_product(n_radial,v1_target) < 0 )
		n_radial = -1.0*n_radial; 
	n_radial.normalize();

	// Step along the radial vector to look for the centerpoint
	double step = 0.01, max = 300.0, epsilon = 0.01, d1, d2;
	R = 0.0;
	vnl_vector<double> r1, r2, c;
	for( double s = step; s <= max; s += step)
	{
		c = tip + s*n_radial;
		r1 = c - tip;	d1 = r1.two_norm();
		r2 = T - c;		d2 = r2.two_norm();
		if( abs(d1-d2) < epsilon )
		{
			R = d1;
			break;
		}
	}
	// Save polynomial fit parameters to datafile for validation
	polyfile << a[3] << "," << a[2] << "," << a[1] << "," << a[0] << ",";
	polyfile << b[3] << "," << b[2] << "," << b[1] << "," << b[0];
	polyfile.close();
	// Save vectors and points to datafile for validation
	vectorfile << tip[0] << "," << tip[1] << "," << tip[2] << ",";
	vectorfile << T[0] << "," << T[1] << "," << T[2] << ",";
	vectorfile << v1_target[0] << "," << v1_target[1] << "," << v1_target[2] << ",";
	vectorfile << v2_target[0] << "," << v2_target[1] << "," << v2_target[2] << ",";
	vectorfile << 999 << "," << 999 << "," << 999 << ",";
	vectorfile << 999 << "," << 999 << "," << 999 << ",";
	vectorfile << n_needle[0] << "," << n_needle[1] << "," << n_needle[2] << ",";
	vectorfile << n_target[0] << "," << n_target[1] << "," << n_target[2] << ",";
	vectorfile << n_radial[0] << "," << n_radial[1] << "," << n_radial[2] << ",";
	vectorfile << posErrorVec[0] << "," << posErrorVec[1] << "," << posErrorVec[2] << ",";
	vectorfile << negErrorVec[0] << "," << negErrorVec[1] << "," << negErrorVec[2] << ",";
	// Save final command points
	vectorfile << del_theta << "," << R;
	vectorfile.close();

	// Check if the needle is already too far
	int remainingDepth = tip[2] - T[2];
	return remainingDepth;
}

void RobotControl::measureNeedleTipPose(std::vector<double> &v_scX, std::vector<double> &v_scY, std::vector<double> &v_scZ, vnl_vector<double> &z, vnl_vector<double> &a, vnl_vector<double> &b)
{
	// Transfer to vnl vectors, and define matrix for polynomial fit
	int N = v_scX.size();
	// If there aren't any Doppler points get out
	if (N <= 0)
		return;
	// Otherwise continue with analysis

	// Build design matrix for SVD fitting
	vnl_matrix<double> D(N,4);
	
	// Define vectors for polynomial fit
	vnl_vector<double> v_X(N), v_Y(N), v_Z(N);
	vnl_matrix<double> A(N,4);
	for (int i = 0; i < N; i++)
	{
		v_X[i] = v_scX[i]; v_Y[i] = v_scY[i]; v_Z[i] = v_scZ[i];
		A(i,3) = 1; 
		A(i,2) = v_Z[i]; 
		A(i,1) = v_Z[i]*v_Z[i]; 
		A(i,0) = v_Z[i]*v_Z[i]*v_Z[i];
	}
	
	// Perform least-squares polynomial fit for order 3
	vnl_matrix<double> Adag(4,N), At(N,4);
	At = vnl_transpose(A);
	Adag = vnl_matrix_inverse<double>(At*A)*At;
	a = Adag*v_X;
	b = Adag*v_Y;

		vnl_vector<double> dbg;
		dbg = a;
		qDebug() << "a:";
		qDebug() << dbg[0] << " " << dbg[1] << " " << dbg[2] << " " << dbg[3];
		qDebug() << "b:";
		dbg = b;
		qDebug() << dbg[0] << " " << dbg[1] << " " << dbg[2] << " " << dbg[3];
		
	// Find the tip point p
	vnl_vector<double> p(3);
	double z_tip = v_Z[N-1];
	p[2] = z_tip;
	p[0] = a[3] + a[2]*z_tip + a[1]*pow(z_tip,2) + a[0]*pow(z_tip,3); 
	p[1] = b[3] + b[2]*z_tip + b[1]*pow(z_tip,2) + b[0]*pow(z_tip,3);

	qDebug() << "p:";
	dbg = p;
	qDebug() << dbg[0] << " " << dbg[1] << " " << dbg[2];

	// Find the first derivative at the tip, this is the z-axis of the tip frame
	vnl_vector<double> z_axis(3);
	z_axis[0] = a[2] + 2*a[1]*z_tip + 3*a[0]*pow(z_tip,2); 
	z_axis[1] = b[2] + 2*b[1]*z_tip + 3*b[0]*pow(z_tip,2);
	z_axis[2] = 1;
	z_axis = z_axis.normalize();

	// Find the second derivative at the tip, this is the y-axis and z-axis combined
	vnl_vector<double> y_axis(3);
	y_axis[0] = 2*a[1] + 6*a[0]*z_tip; 
	y_axis[1] = 2*b[1] + 6*b[0]*z_tip;
	y_axis[2] = 0;
	y_axis = y_axis.normalize();

	// Remove z-axis components from y_axis
	y_axis = y_axis - dot_product(y_axis,z_axis)*z_axis;
	y_axis = y_axis.normalize();

	// Create rotation matrix representation of the frame
	vnl_vector<double> x_axis = vnl_cross_3d(y_axis,z_axis);
	vnl_matrix<double> R(3,3);
	R.set_column(0,x_axis);
	R.set_column(1,y_axis);
	R.set_column(2,z_axis);

	// Create quaternion and axis-angle representation
	vnl_quaternion<double> q(R.transpose());
	vnl_vector<double> r = q.axis();
	r = r*q.angle();

	// Format the final output
	for( int i = 0; i < 3; i++)
	{
		z[i] = p[i];
		z[i+3] = r[i];
	}
}

void RobotControl::steeringPlanner(vnl_vector<double> x_est, vnl_vector<double> t, vnl_vector<double> &u)
{
	// define x,y,z axes as vectors
	vnl_matrix<double> I(3,3); I.set_identity();
	vnl_vector<double> z_axis = I.get_column(2);

	// isolate the position and orientation components of the input vector
	vnl_vector<double> p = x_est.extract(3,0);
	vnl_vector<double> r = x_est.extract(3,3);
	// change rotation vector representation to quaternion
	vnl_vector<double> r_norm = r;
	r_norm.normalize();
	vnl_quaternion<double> q(r_norm,-r.magnitude());
	// describe the target point, t, relative to the current tip frame
	vnl_vector<double> t_rel = q.rotate(t-p);
	// project the relative vector onto the x-y plane of the tip frame
	vnl_vector<double> v = t_rel.extract(2,0);
	v = v.normalize();
	// solve for the necessary change in rotation angle
	double d_th = atan2(-v.get(0),v.get(1));
	// describe the target in the rotated tip frame
	vnl_quaternion<double> q_z_dth(z_axis,-d_th);
	vnl_vector<double> t_rel2 = q_z_dth.rotate(t_rel);
	// solve for the radius of the relative path
	double two_norm = t_rel2.two_norm();
	double ro = pow(two_norm,2.0)/2.0/t_rel2.get(1);
	// solve for the distance remaining to the target
	double l = two_norm;
	if( fabs(t[2]-p[2]) < l )
	{ l = fabs(t[2]-p[2]); }
	// format everything we have solved for into the command vector
	u.put(0,d_th);
	u.put(1,ro);
	u.put(2,l);

	vnl_vector<double> db;
	db = t_rel;
	qDebug() << "t_rel:";
	qDebug() << db[0] << " " << db[1] << " " << db[2] << endl;
	db = t_rel2;
	qDebug() << "t_rel:";
	qDebug() << db[0] << " " << db[1] << " " << db[2] << endl;
	db = t;
	qDebug() << "t:";
	qDebug() << db[0] << " " << db[1] << " " << db[2] << endl;
	db = p;
	qDebug() << "t_rel:";
	qDebug() << db[0] << " " << db[1] << " " << db[2] << endl;


}

void RobotControl::mapRadiusToDC( double RadiusMM, double &DC)
{
	// maps the desired radius of curvature for the needle to a duty cycle percentage
	DC = 1.0 - (NEEDLE_MIN_R/RadiusMM);

	if(DC > 1.0)
		DC = 1.0;
	else if(DC < 0.0)
		DC = 0.0;
}

void RobotControl::setVibration(bool OnOff)
{
	// Activates or deactivates vibration coil using digital output of roll controller
	m_robot.setVibration(OnOff);
}

void RobotControl::InsertIncremental(double inc)
{
	m_robot.InsertIncremental(inc);
}

void RobotControl::SetInsertionVelocity(double v)
{
	m_robot.SetInsertionVelocity(v);
}

void RobotControl::SetRotationVelocity(double v)
{
	m_robot.SetRotationVelocity(v);
}

void RobotControl::RotateIncremental(double inc)
{
	m_robot.RotateIncremental(inc);
}

void RobotControl::homeRoll(void)
{
	m_robot.HomeRoll();
}

void RobotControl::homeInsertion(void)
{
	m_robot.HomeInsertion();
}

void RobotControl::setTargetX(int X)
{
	T[0] = X; 
}

void RobotControl::setTargetY(int Y)
{
	T[1] = Y; 
}

void RobotControl::setTargetZ(int Z)
{
	T[2] = Z;
}

void RobotControl::enableKalmanFilter(bool ukfON)
{
	m_useUKF = ukfON;
}

void RobotControl::callFunctionDebug()
{
	//// Sigmas-------------------------------
	//vnl_vector<double> x(6,0.0);
	//for( int i = 0; i < 6; i++ )
	//{ x[i] = i*0.333;}
	//vnl_matrix<double> P(6,6,0.0);
	//P.fill_diagonal(1.6);
	//double c = 289.2;
	//vnl_matrix<double> X(6,2*6+1);
	//UKF.sigmas(x, P, c, X);
	//for( int i = 0; i<6; i++ )
	//{
	//	qDebug() << "X, row " << i << ":  "; 
	//	qDebug() << X(i,0) << " " << X(i,1) << " " << X(i,2) << " " << X(i,3) << " " << X(i,4) << " "
	//		<< X(i,5) << " " << X(i,6) << " " << X(i,7) << " " << X(i,8) << " " << X(i,9) << " " 
	//		<< X(i,10) << " " << X(i,11) << " " << X(i,12) << endl;
	//}
	//vnl_vector<double> v(13,2.0);
	//vnl_vector<double> ans = X*v;
	//qDebug() << "Multiplication answer:";
	//qDebug() << ans[0] << " " << ans[1] << " " << ans[2] << " " << ans[3] << " " << ans[4] << " " << ans[5];
	//qDebug() << "Troy2";

	//// f-------------------------------
	//vnl_vector<double> x(6,0.0);
	////for( int i = 0; i < 6; i++ )
	////{ x[i] = i*0.333;}

	//vnl_vector<double> u(3);
	//u[0] = 54.55;
	//u[1] = 20.5;
	//u[2] = 3.5;

	//vnl_vector<double> x2(6);

	//UKF.f(x, u, x2);
	//
	//// Display the vectors
	//qDebug() << "input:";
	//qDebug() << x[0] << " " << x[1] << " " << x[2] << " " << x[3] 
	//<< " " << x[4] << " " << x[5] << endl;

	//qDebug() << "result:";
	//qDebug() << x2[0] << " " << x2[1] << " " << x2[2] << " " << x2[3] 
	//  << " " << x2[4] << " " << x2[5] << endl;

	//// UpdateUKF-------------------------------
	//vnl_vector<double> x2(6);
	//vnl_vector<double> u(3);
	//u[0] = 0;
	//u[1] = 25.6;
	//u[2] = 5;

	//vnl_vector<double> z(6,12.6);

	//UKF.updateUKF(u,z);
	//UKF.getCurrentStateEstimate(x2);

	//// Display the vectors
	//qDebug() << "result:";
	//qDebug() << x2[0] << " " << x2[1] << " " << x2[2] << " " << x2[3] 
	//<< " " << x2[4] << " " << x2[5] << endl;

	//// SteeringPlanner-------------------------------
	//vnl_vector<double> x(6);
	//vnl_vector<double> u(3);
	//x[0] = 1.5;
	//x[1] = 3.5;
	//x[2] = 4.5;
	//x[3] = 0.4;
	//x[4] = 0.5;
	//x[5] = 0.6;
	//vnl_vector<double> t(3);
	//t[0] = 0.0;
	//t[1] = 180.5;
	//t[2] = 47.5;

	//steeringPlanner(x,t,u);

	// measureNeedleTipPose --------------------------

	//vnl_vector<double> a(4), b(4);

	//a[0] = 1e-3; a[1] = 2e-3; a[2] = 1e-3; a[3] = 67.3;
	//b[0] = 1e-3; b[1] = 1e-3; b[2] = 4e-3; b[3] = 101;

	//std::vector<double> v_scX, v_scY, v_scZ;
	//for(double s = 25.5; s >= 0.0; s = s-0.5)
	//{
	//	v_scX.push_back(a[3] + a[2]*s + a[1]*pow(s,2) + a[0]*pow(s,3));
	//	v_scY.push_back(b[3] + b[2]*s + b[1]*pow(s,2) + b[0]*pow(s,3));
	//	v_scZ.push_back(s);
	//}

	//vnl_vector<double> z(6);

	//measureNeedleTipPose(v_scX, v_scY, v_scZ, z);

	//// Display the vectors
	//qDebug() << "z:";
	//qDebug() << z[0] << " " << z[1] << " " << z[2] << " " << z[3] << " " << z[4] << " " << z[5] << endl;
 //
	qDebug() << "Troy" << endl;
}

void RobotControl::resetSystem()
{
	// Reset system for new set of incremental scans
	m_stepNumber = 1;
	// Reset prior for X
	UKF.resetEstimate();

}