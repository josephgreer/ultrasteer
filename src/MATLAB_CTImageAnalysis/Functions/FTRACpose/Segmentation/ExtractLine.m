% $Id: ExtractLine.m,v 1.3 2005/11/28 19:45:14 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	ExtractLine() 
% // file:		ExtractLine.m
% // author:		siddharth
% // date:		
% // description: performs the final line segmentation in a series of steps
%               1) locate actual start & end point on the line from initial
%               BB guess locations, using connectivity
%               2) extract connected line points from the information of
%               initial start point, assuming direction to be nearly
%               vertical & using heurisitic of closer initial direction guess, 
%               by A-algorithm based on graph search, on edge image, which
%               also requires calculation of the threshold for a good edge
%               3) For all the points received, first locate consistent
%               line segments, form a set of points which are consistent
%               segments
%               4) For each point in that set, try to find a good point in
%               intensity image, by looking for a particular intensity
%               profile within 1 pixel window of that point.
%               5) Collect all such good points, do a least square line
%               fitting, to obtain equation of the line

% // input:	Intensity image, edge image, Estimate line start and end
% points, Initial direction estimate, and a flag indicating whether to use
% that initial direction estimate or not.

% // output: equation of the line & a flag indicating whether that equation
% is usable or not.

% // calling function: during the process of Fiducial segementation, after
% pressing of the button Select region by FiducialSelectROI_Callback

% // NOTE: 
% //-----------------------------------------------------------------------------
function [bLineFound, lLinePointsEq, bStartPointValid, bEndPointValid] = ExtractLine(IintensityImage, IedgeImage,EstimatedLineStartingPoint, EstimatedLineEndPoint, bUseDirectionEstimate, InitialDirectionEstimate)
% find the edge image thresholds
[iUpperThreshold iLowerThreshold] = FindThresholds(IedgeImage, 20);

iX_Dimension = length(IedgeImage(:,1));
    
iY_Dimension = length(IedgeImage(1,:));
    
pStart = EstimatedLineStartingPoint;

pEnd = EstimatedLineEndPoint;

lLinePoints = [];

lLinePointsEq = [];

bStartPointValid = false;

bEndPointValid = false;

bLineFound = false;

% from the given guess of start & end point find, which is BB center, which will not be a connected point on the line,
% find good connected points

if (iLowerThreshold > 20)
%   if the edge is strong enough then use a stronger threshold to check connectivity  
    if( (pStart (1,1) ~= iX_Dimension) && (pStart(1,1) > 0) && (pStart(1,2) > 0))
        [bStartPointValid pStart] = LocateUpConnectedPoint(EstimatedLineStartingPoint, (iLowerThreshold + iUpperThreshold)/2, (iLowerThreshold + iUpperThreshold)/4, IedgeImage);
    end

    if( (pEnd (1,1) ~= 1) && (pEnd(1,1) > 0) && (pEnd(1,2) > 0) )
        [bEndPointValid pEnd] = LocateDownConnectedPoint(EstimatedLineEndPoint, (iLowerThreshold + iUpperThreshold)/2, (iLowerThreshold + iUpperThreshold)/4, IedgeImage);
    end
else
%   if the edge is not strong then use a lower threshold to check connectivity
    if( (pStart (1,1) ~= iX_Dimension) && (pStart(1,1) > 0) && (pStart(1,2) > 0))
        [bStartPointValid pStart] = LocateUpConnectedPoint(EstimatedLineStartingPoint, iLowerThreshold, iLowerThreshold/4, IedgeImage);
    end

    if( (pEnd (1,1) ~= 1) && (pEnd(1,1) > 0) && (pEnd(1,2) > 0) )
        [bEndPointValid pEnd] = LocateDownConnectedPoint(EstimatedLineEndPoint, iLowerThreshold, iLowerThreshold/4, IedgeImage);
    end
end

% get the egde by graphSearch
if (bStartPointValid && ~bEndPointValid)
    lLinePoints = GetEdgeByGraphSearch (IedgeImage, iLowerThreshold, pStart, pStart, pEnd, false, bUseDirectionEstimate, InitialDirectionEstimate);
else
    if (bEndPointValid && ~bStartPointValid)
        lLinePoints = GetEdgeByGraphSearch (IedgeImage, iLowerThreshold, pEnd, pStart, pEnd, false, bUseDirectionEstimate, InitialDirectionEstimate);
        
    else
        if (bStartPointValid && bEndPointValid)
            lLinePoints = GetEdgeByGraphSearch (IedgeImage, iLowerThreshold, pStart, pStart, pEnd, true, bUseDirectionEstimate, InitialDirectionEstimate);
        end
    end
        
end

% smooth out the line, choose the good points, then with the good points,
% do a least squares fitting & find the equation

  if (length (lLinePoints) > 1)
   
%      plot line point locations on image
%      CurrentAxesHandle = gca;
%      hold on;

     lGoodLinePoints = [];
      
     iIntensityArray = [];
     
     for iPoint = 1: length(lLinePoints(:,1))
          iX_Index = lLinePoints(iPoint,1);
          iY_Index = lLinePoints (iPoint,2);
          if( (iX_Index >0) && (iY_Index >0))
            iIntensityArray = [iIntensityArray; IintensityImage(iX_Index, iY_Index)];
          end
%           plot(lLinePoints(iPoint,2), lLinePoints(iPoint,1), 'y.');
      end
       
     iAvgIntensity = mean(iIntensityArray);
     
     iLineSegment = 1;
     
     lConsistentSegments = [];
     
     for i = 1*iLineSegment: length(lLinePoints(:,1))
         
         if(iLineSegment + 15 > length(lLinePoints(:,1)))
            AvgY = mean(lLinePoints(iLineSegment:length(lLinePoints(:,1)), 2));
            stdY = std(lLinePoints(iLineSegment:length(lLinePoints(:,1)), 2));
            if(stdY < 0.5)
                lConsistentSegments = [lConsistentSegments; lLinePoints(iLineSegment:length(lLinePoints(:,1)), :)];
            end

             break;
         end
         
         AvgY = mean(lLinePoints(iLineSegment:iLineSegment+15, 2));
         stdY = std(lLinePoints(iLineSegment:iLineSegment+15, 2));
         if(stdY <= 0.6)
             lConsistentSegments = [lConsistentSegments; lLinePoints(iLineSegment:iLineSegment+15, :)];
         end
         
         iLineSegment = iLineSegment + 15;
         
     end
     
     if(length(lConsistentSegments) > 1)
        
         for iPoint = 1: length(lConsistentSegments(:,1))
 
            [bPointFound pGoodPoint] = FindGoodPoint(IintensityImage, iAvgIntensity, lConsistentSegments(iPoint, :));
         
            if(bPointFound)
                 lGoodLinePoints = [lGoodLinePoints; pGoodPoint];
%                plot(pGoodPoint(1,2), pGoodPoint(1,1), 'r.');
            end
         
         end
         
         if (length (lGoodLinePoints) >= 4)
     
            lLineEq = least_squares_line(lGoodLinePoints);
      
            lLinePointsEq = lLineEq;
            
            bLineFound = true;
        else
            bLineFound = false;
        end
            
             
         
     end
     
      
end
 


% $Log: ExtractLine.m,v $
% Revision 1.3  2005/11/28 19:45:14  anton
% General Update from Siddharth:
% *: Handling failure cases in fiducial segmentation, when only partial region
%    is selected as region of interest for fiducial...however, note that the
%    region should not be too small
% *: Now the BB numbers, line numbers and ellipse numbers are displayed after
%    establishing correspondence to have a visual check for user that
%    correspondence established is correct; Note tht these are displayed only
%    on whole image, not zoomed up region of interest, because there user can
%    make changes....so just on whole image after segmentation & after user
%    says he accepts the results
% *: Few cases prev. unhandled in user intervention in fiducial segmentation
%    validation
%
% Revision 1.2  2005/11/03 15:53:47  svikal
% changes made for establishing correspondence of BBs
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%