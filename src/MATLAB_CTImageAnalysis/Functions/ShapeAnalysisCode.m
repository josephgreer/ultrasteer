% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: ShapeAnalysisCode(scanFolder)
% -------------------------------------------------------------------------
% Inputs: scanFolder - string path for the folder containing the scan files
% Outputs: shape - shape analysis structure for the particular scan
% Sub_Functions Called: xxxxxx
% Synopsis:
%   This program will generate a sturcture for curvature and RMSE of curve
%   fitting for a particular scan. The scan folder contains files of the
%   name scan0x-nnn.dcm where x is the scan number and nnn is a three digit
%   number of a frame associated with the scan. 
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: ShapeAnalysisCode()
% =========================================================================

function [shape] = ShapeAnalysisCode(scanFolder,inVivo,ROIdata)

% -------------------------------------------------------------------------
% Initialization of variables
% -------------------------------------------------------------------------

    close all;
    

    % Initialize the shape analysis structure.
    shape = struct('curve','curveSTDERR','line','lineRESNORM','Image','Data');
    shape.curve = [];
    shape.curveSTDERR = [];
    shape.line = [];
    shape.lineRESNORM = [];
    shape.Image = [];
    shape.Data = [];
    
  if inVivo == 1  
    file = dir(scanFolder);
    
    
    % -------------------------------------------------------------------------
    % Generate Region of Intrest (matrix of XYZ points)
    % -------------------------------------------------------------------------
    
    % Read in the files with dicomread. The number after the scan0x-nnn is
    % three digits so it needs to be padded to be read properly. 
    for j = 3:length(file)
        x(:,:,j-2) = dicomread(strcat(scanFolder,'/',file(j).name));
       
    end
    
    numberFiles = length(file)-2;

    % Run the thresholding method to expose just the bone and needle. 
    [image v] = Threshold(x,numberFiles);
    shape.Image = image;                                               % Save image in shape struct

    % Run the segment method to zoom in on the needle.
    [ROImatrix] = Segment(v,inVivo);
    
  else
      
      ROImatrix = Segment(ROIdata,inVivo);
  end
  
% -------------------------------------------------------------------------
% Fit a curve to the region of interest
% -------------------------------------------------------------------------
[m n p] = size(ROImatrix);
if m*n*p < 10
    disp('ERROR - TOO SMALL REGION')                                   % Generate an error of the RIO is too small.

else                                                                   % IF NOT TOO SMALL THEN CONTINUE WITH CURVE FITTING
    [x0n,an,rn,conv,d,sigmah] = FitCurve(ROImatrix);

    % REMOVE OUTLIERS
    % -----------------------------------------------------------------
    cutoff = 1.5;                                                      % number of standard deviations away from the mean for cutoff
    curvePoints = ROImatrix;
    curvePointsCulled_new = removeOutliers(curvePoints,d,cutoff); 
    curvePointsCulled = ROImatrix;

    while(length(curvePointsCulled) ~= length(curvePointsCulled_new)...
          && ...
          length(curvePointsCulled_new) > (length(ROImatrix)/2))

        curvePointsCulled = curvePointsCulled_new;

        [x0n,an,rn,conv,d,sigmah] = FitCurve(curvePointsCulled);       % Fit a new circle

        curvePointsCulled_new = removeOutliers(curvePointsCulled,d, cutoff);  % remove the outliers


    end
    shape.Data = curvePointsCulled_new;                                % Save the data points after outlier removal. 

    [x0n,an,rn,conv,d,sigmah] = FitCurve(curvePointsCulled_new);

    % FITTING LINE OR CIRCLE
    % -----------------------------------------------------------------
    if conv == 1                                                       % Fit a circle if the circle fitting algorithm converged.
       % if rn >= 40      % need to ask if this is okay... 
        shape.curve = [rn; x0n; an ];
        shape.curveSTDERR = sigmah;
        %else
        %[Xpts,Ypts,Zpts,x1, a1,d1,normd1] = FitLine(curvePointsCulled_new);

        %shape.line = [x1; a1];
        %shape.lineRESNORM = normd1;
        %end

        % OPTIONAL PLOTTING OF ISOSURFACE WITH FIT CIRCLE 
        % -------------------------------------------------------------
        %  figure
        %  isosurface(image)
        %  hold on
        %  circle3(x0n,an,rn)
    else
       [Xpts,Ypts,Zpts,x1, a1,d1,normd1] = FitLine(curvePointsCulled_new); % Fit a line if the circle fitting algorithm did not converge.

        shape.line = [x1; a1];
        shape.lineRESNORM = normd1;

        % OPTIONAL PLOTTING OF ISOSURFACE WITH FIT LINE
        % -------------------------------------------------------------
        %  figure
        %  isosurface(image)
        %  hold on
        %  plot3(Xpts,Ypts,Zpts)
    end
end
    
end

% =========================================================================
% END FUNCTION: ShapeAnalysisCode()
% =========================================================================

