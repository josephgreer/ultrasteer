% $Id: CalculateLineDirectionEstimateFromEndPoints.m,v 1.2 2005/11/28 19:45:14 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	CalculateLineDirectionEstimateFromEndPoints() 
% // file:		CalculateLineDirectionEstimateFromEndPoints.m
% // author:		siddharth
% // date:		
% // description: from the initial guess of line start and end points,
% which was the corresponding BB location itself, first see which line has got 
% both start and end points valid, then in that case tries to find the
% initial closer guess to the direction of the line., using two point
% equation of line & getting the slope..this will serve as heuristic
% information to the line tracking algorithm.

% // input:	

% // output:

% // calling function: during the process of Fiducial segementation, after
% pressing of the button Select region by FiducialSelectROI_Callback

% // NOTE: Since all three lines are parallel by physical construction,
% they show up nearly parallel in the image too, so getting line direction
% estimate for only one of the lines is sufficient
% //-----------------------------------------------------------------------------
function [bUseDirectionEstimate, InitialDirectionEstimate] = CalculateLineDirectionEstimateFromEndPoints(Line1StartingPoint, Line1EndPoint, Line2StartingPoint, Line2EndPoint, Line3StartingPoint, Line3EndPoint, iX_Dimension, iY_Dimension)

bUseDirectionEstimate = false;

InitialDirectionEstimate = 0;

if( (Line1StartingPoint(1,1) > 0) && (Line1StartingPoint(1,2) > 0) && (Line1EndPoint(1,1) > 0) && (Line1EndPoint(1,2) > 0) && (Line1EndPoint(1,1) ~= 1) && (Line1StartingPoint(1,1) ~= iX_Dimension))
    if (Line1StartingPoint (1,2) - Line1EndPoint(1,2) == 0)
        InitialDirectionEstimate = 90;
    else
        InitialDirectionEstimate = atan( ((Line1StartingPoint(1,1) - Line1EndPoint(1,1))/(Line1StartingPoint(1,2) - Line1EndPoint(1,2))))*180/pi;
    end
    
    bUseDirectionEstimate = true;
end

if( (Line2StartingPoint(1,1) > 0) && (Line2StartingPoint(1,2) > 0) && (Line2EndPoint(1,1) > 0) && (Line2EndPoint(1,2) > 0) && (Line2EndPoint(1,1) ~= 1) && (Line2StartingPoint(1,1) ~= iX_Dimension))

    if (Line2StartingPoint (1,2) - Line2EndPoint(1,2) == 0)
        InitialDirectionEstimate = 90;
    else
        InitialDirectionEstimate = atan( ((Line2StartingPoint(1,1) - Line2EndPoint(1,1))/(Line2StartingPoint(1,2) - Line2EndPoint(1,2))))*180/pi;
    end
    
    bUseDirectionEstimate = true;
    
end

if( (Line3StartingPoint(1,1) > 0) && (Line3StartingPoint(1,2) > 0) && (Line3EndPoint(1,1) > 0) && (Line3EndPoint(1,2) > 0) && (Line3EndPoint(1,1) ~= 1) && (Line3StartingPoint(1,1) ~= iX_Dimension))
    
    if (Line3StartingPoint (1,2) - Line3EndPoint(1,2) == 0)
        InitialDirectionEstimate = 90;
    else
        InitialDirectionEstimate = atan( ((Line3StartingPoint(1,1) - Line3EndPoint(1,1))/(Line3StartingPoint(1,2) - Line3EndPoint(1,2))))*180/pi;
    end
    
    bUseDirectionEstimate = true;

end

if(bUseDirectionEstimate)
    if(InitialDirectionEstimate < 0)
        InitialDirectionEstimate = 90 + (90+InitialDirectionEstimate);
    end
end


% $Log: CalculateLineDirectionEstimateFromEndPoints.m,v $
% Revision 1.2  2005/11/28 19:45:14  anton
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
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%