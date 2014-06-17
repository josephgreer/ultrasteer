% $Id: IsOnLine.m,v 1.3 2005/11/28 19:45:14 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	IsOnLine
% // file:		IsOnLine.m
% // author:	siddharth
% // date:		
% LOGIC: if the point(BB) is near to the line, then project the point on
% the line, look for 3 pixels in upward & downward direction, find the
% highest intensity point using the interpolate function, in the 7 subpixel
% points

% // input:	intensity image, line equation, whether the line is valid or
% not

% // output: flag indicating whether it lies on given line equation or not,
% if yes then corresponding brightest/darkest point

% // calling function: RecalculateBBLocationsAndEstablishCorrespondence() 

% // NOTE: 
% //-----------------------------------------------------------------------------
function [bLiesOnLine, BrightestPoint] = IsOnLine(IROIFiducial, BBPoint, lLineEq, bLineValid)

bLiesOnLine = false;

BrightestPoint = BBPoint;

iX_Dimension = length (IROIFiducial(:,1));

iY_Dimension = length (IROIFiducial(1,:));

% LOGIC: if the point(BB) is near to the line, then project the point on
% the line, look for 3 pixels in upward & downward direction, find the
% highest intensity point using the interpolate function, in the 7 subpixel
% points

if(bLineValid)


    iResult = lLineEq(1)*BBPoint(1,1) + lLineEq(2)*BBPoint(1,2) + lLineEq(3);

    if(abs(iResult) <= 3)
    %   the point lies on the line
        bLiesOnLine = true;
    
%   project the point on the line
%   to do that, we define a line which passes through this point and is
%   perpendicular to the given line, then we find the intersection point of
%   two lines
    
        DesiredSlope = lLineEq(2)/lLineEq(1);
    
        a1 = lLineEq(1); b1 = lLineEq(2); c1 = lLineEq(3);
    
        a2 = lLineEq(2); b2 = -lLineEq(1); c2 = BBPoint(1,2)*lLineEq(1) - lLineEq(2)*BBPoint(1,1);
    
        ProbablePointMid(1,1) = (b1*c2 - b2*c1)/(a1*b2 - a2*b1);
    
        ProbablePointMid(1,2) = (c1*a2 - c2*a1)/(a1*b2 - a2*b1);
        
%       find 1 points above % 1 point below on the line
        ProbablePointTop(1,1) = ProbablePointMid(1,1) - 1;
        if (lLineEq(2) ~= 0)
            ProbablePointTop(1,2) = -(a1*ProbablePointTop(1,1) + c1)/b1;
        else
            ProbablePointTop(1,2) = ProbablePointMid(1,2);
        end

        ProbablePointBelow(1,1) = ProbablePointMid(1,1) + 1;
        if (lLineEq(2) ~= 0)
            ProbablePointBelow(1,2) = -(a1*ProbablePointTop(1,1) + c1)/b1;
        else
            ProbablePointBelow(1,2) = ProbablePointMid(1,2);
        end
        
%       define a region of interest for interpolation around the
%       projected point as center of the region
        ROI_StartX = round(ProbablePointMid(1,1) - 5); ROI_EndX = round(ProbablePointMid(1,1) + 5); 
        ROI_StartY = round(ProbablePointMid(1,2) - 5); ROI_EndY = round(ProbablePointMid(1,2) + 5); 
        
        if ( (ROI_StartX > 0) && (ROI_StartY > 0) && (ROI_EndX <= iX_Dimension) && (ROI_EndY <= iY_Dimension))
            
            Iroi = IROIFiducial(ROI_StartX:ROI_EndX, ROI_StartY:ROI_EndY);
        
            IntensityArray = [];
        
            for iX_Index = ProbablePointMid(1,1)-5:0.1:ProbablePointMid(1,1)+5
                if (lLineEq(2) ~= 0)
                    iY_Index = -(a1*iX_Index + c1)/b1;
                else
                    iY_Index = ProbablePointMid(1,2);
                end
            
                IntensityArray = [IntensityArray; [interp2(Iroi, (iY_Index-ROI_StartY+1),  (iX_Index-ROI_StartX+1),'cubic') iX_Index iY_Index]];
                
            end

            
            iMinIndex = find(IntensityArray(:,1) == min(IntensityArray(:,1)));
         
            BrightestPoint = IntensityArray(iMinIndex(1), 2:3);
        else
            BrightestPoint = ProbablePointMid;
        
        end

            
    
    end

end


% $Log: IsOnLine.m,v $
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
% Revision 1.2  2005/11/08 00:47:51  svikal
% BB correspondence after edit/user changes
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%