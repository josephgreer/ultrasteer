% $Id: GetLineStartAndEndPointsFromBBs.m,v 1.4 2007/08/03 22:42:21 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	GetLineStartAndEndPointsFromBBs() 
% // file:		GetLineStartAndEndPointsFromBBs.m
% // author:	siddharth
% // date:		
% // description: finds which BBs could be the start and end point of one
% line.; assumes that image is nearly vertical i.e. BBs on one line do not
% have a 'y' coordinate displacement of more than 3-4 pixel w.r.t each
% other, labels start point as the point which is lowest in x-cordinate
% below the half of the image, since by physical construction, below the
% half there are more than one BBs on one line; if lower point (start) is not
% found, then puts its x = xDimension, the end of image; if upper point not
% found, then puts its x = 1; it may also happen that neither start nor end
% point is visible

% // input:	BBLocations for BB positions, iX_dimension for determining
% upper bound

% // output: 

% // calling function: called when Select region button is pressed from Fiducial segmentation, 

% // NOTE: This algorithm assumes that BB Locations found are correct; if
% those are incorrect then, the further algorithm which heavily relies on
% start point/end point information will not work
% //-----------------------------------------------------------------------------
function [Line1StartingPoint, Line1EndPoint, Line2StartingPoint, Line2EndPoint, Line3StartingPoint, Line3EndPoint] = GetLineStartAndEndPointsFromBBs (BBLocations, iX_Dimension, resRatioToNTSC)
Line1StartingPoint = [];
Line2StartingPoint = [];
Line3StartingPoint = [];
Line1EndPoint = [];
Line2EndPoint = [];
Line3EndPoint = [];

for iBBNumber = 1: length(BBLocations(:,1))

    CurrentPoint = BBLocations(iBBNumber, :);
    CurrentPointX = CurrentPoint(1,1);
    CurrentPointY = CurrentPoint(1,2);

    if( (CurrentPointX > 0) && (CurrentPointY > 0))

        if (CurrentPointX > iX_Dimension/2)

            if (length(Line1StartingPoint) == 0)
                Line1StartingPoint = [Line1StartingPoint; CurrentPoint];
            else
                CurrentLine1Point = Line1StartingPoint(1,:);
                CurrentLine1Y = CurrentLine1Point(1,2);
                CurrentLine1X = CurrentLine1Point(1,1);
                if ( (abs(CurrentPointY - CurrentLine1Y) <= (10 * resRatioToNTSC)) )
                    if (CurrentPointX < CurrentLine1X)
                        Line1StartingPoint = CurrentPoint;
                    end
                else
                    if(length(Line2StartingPoint) == 0)
                        Line2StartingPoint = [Line2StartingPoint; CurrentPoint];
                    else
                        CurrentLine2Point = Line2StartingPoint(1,:);
                        CurrentLine2Y = CurrentLine2Point(1,2);
                        CurrentLine2X = CurrentLine2Point(1,1);
                        if ( (abs(CurrentPointY - CurrentLine2Y) <= (10 * resRatioToNTSC)) )
                            if (CurrentPointX < CurrentLine2X)
                                Line2StartingPoint = CurrentPoint;
                            end
                        else
                            if(length(Line3StartingPoint) == 0)
                                Line3StartingPoint = [Line3StartingPoint; CurrentPoint];
                            else
                                CurrentLine3Point = Line3StartingPoint(1,:);
                                CurrentLine3Y = CurrentLine3Point(1,2);
                                CurrentLine3X = CurrentLine3Point(1,1);
                                if  (abs(CurrentPointY - CurrentLine1Y) <= (10 * resRatioToNTSC))
                                    if  (CurrentPointX < CurrentLine1X)
                                        Line3StartingPoint = CurrentPoint;
                                    end
                                end
                            end
                        end
                    end
                end
            end
        end
    end
end


for iBBNumber = 1: length(BBLocations(:,1))

    CurrentPoint = BBLocations(iBBNumber, :);
    CurrentPointX = CurrentPoint(1,1);
    CurrentPointY = CurrentPoint(1,2);

    if( (CurrentPointX > 0) && (CurrentPointY > 0))

        if (CurrentPointX < iX_Dimension/2)

            if( (length(Line1StartingPoint) ~=0) && (abs(CurrentPointY - Line1StartingPoint(1,2)) <= (15 * resRatioToNTSC)) )

                if (length(Line1EndPoint) == 0)
                    Line1EndPoint = [Line1EndPoint; CurrentPoint];
                else
                    CurrentLine1Point = Line1EndPoint(1,:);
                    CurrentLine1Y = CurrentLine1Point(1,2);
                    CurrentLine1X = CurrentLine1Point(1,1);
                    if ( (abs(CurrentPointY - CurrentLine1Y) <= (10 * resRatioToNTSC)) )
                        if (CurrentPointX < CurrentLine1X)
                            Line1EndPoint = CurrentPoint;
                        end

                    end

                end

            else
                if( (length(Line2StartingPoint) ~=0) && (abs(CurrentPointY - Line2StartingPoint(1,2)) <= (15 * resRatioToNTSC)) )


                    if(length(Line2EndPoint) == 0)
                        Line2EndPoint = [Line2EndPoint; CurrentPoint];
                    else
                        CurrentLine2Point = Line2EndPoint(1,:);
                        CurrentLine2Y = CurrentLine2Point(1,2);
                        CurrentLine2X = CurrentLine2Point(1,1);
                        if ( (abs(CurrentPointY - CurrentLine2Y) <= (3 * resRatioToNTSC)))
                            if (CurrentPointX < CurrentLine2X)
                                Line2EndPoint = CurrentPoint;
                            end
                        end
                    end

                else
                    if( (length(Line3StartingPoint) ~=0) && (abs(CurrentPointY - Line3StartingPoint(1,2)) <= (15  * resRatioToNTSC)) )

                        if(length(Line3EndPoint) == 0)
                            Line3EndPoint = [Line3EndPoint; CurrentPoint];
                        else
                            CurrentLine3Point = Line3EndPoint(1,:);
                            CurrentLine3Y = CurrentLine3Point(1,2);
                            CurrentLine3X = CurrentLine3Point(1,1);
                            if  (abs(CurrentPointY - CurrentLine1Y) <= (3 * resRatioToNTSC))
                                if  (CurrentPointX < CurrentLine1X)
                                    Line3EndPoint = CurrentPoint;
                                end
                            end
                        end

                    else
                        if (length(Line1StartingPoint) ==0 && length(Line1EndPoint) == 0)
                            Line1EndPoint = [Line1EndPoint; CurrentPoint];
                        else

                            if (length(Line2StartingPoint) ==0 && length(Line2EndPoint) == 0)
                                Line2EndPoint = [Line2EndPoint; CurrentPoint];
                            else
                                if (length(Line3StartingPoint) ==0 && length(Line3EndPoint) == 0)
                                    Line3EndPoint = [Line3EndPoint; CurrentPoint];
                                end
                            end
                        end
                    end
                end
            end
        end
    end
end



if (length(Line1EndPoint) == 0)
    Line1EndPoint (1,1) = 1;
    if (length (Line1StartingPoint) > 1 )
        Line1EndPoint(1,2) = Line1StartingPoint(1,2);
    else
        Line1EndPoint(1,2) = -1;
    end

end

if (length(Line2EndPoint) == 0)
    Line2EndPoint (1,1) = 1;

    if (length (Line2StartingPoint) > 1 )
        Line2EndPoint(1,2) = Line2StartingPoint(1,2);
    else
        Line2EndPoint(1,2) = -1;
    end

end

if (length(Line3EndPoint) == 0)
    Line3EndPoint (1,1) = 1;

    if (length (Line3StartingPoint) > 1 )
        Line3EndPoint(1,2) = Line3StartingPoint(1,2);
    else
        Line3EndPoint(1,2) = -1;
    end

end

if (length(Line1StartingPoint) == 0)
    Line1StartingPoint (1,1) = iX_Dimension;
    if (Line1EndPoint(1,2) > 1 )
        Line1StartingPoint(1,2) = Line1EndPoint(1,2);
    else
        Line1StartingPoint (1,1) = -1;
        Line1StartingPoint (1,2) = -1;
    end

end

if (length(Line2StartingPoint) == 0)
    Line2StartingPoint (1,1) = iX_Dimension;
    if (Line2EndPoint(1,2) > 1 )
        Line2StartingPoint(1,2) = Line2EndPoint(1,2);
    else
        Line2StartingPoint (1,1) = -1;
        Line2StartingPoint (1,2) = -1;
    end

end


if (length(Line3StartingPoint) == 0)
    Line3StartingPoint (1,1) = iX_Dimension;
    if (Line3EndPoint(1,2) > 1 )
        Line3StartingPoint(1,2) = Line3EndPoint(1,2);
    else
        Line3StartingPoint (1,1) = -1;
        Line3StartingPoint (1,2) = -1;
    end

end

    
% $Log: GetLineStartAndEndPointsFromBBs.m,v $
% Revision 1.4  2007/08/03 22:42:21  anton
% GetLineStartAndEndPointsFromBBs.m: Added support for resolution ratio to
% NTSC.  Removed useless blank lines.
%
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
