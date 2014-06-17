% $Id: FindDistance.m,v 1.2 2005/11/28 19:45:14 anton Exp $


% //-----------------------------------------------------------------------------
% // function:	FindDistance () 
% // file:		FindDistance.m
% // author:		siddharth
% // date:		
% // task:		find the distance between two points
% //				
% // parameter:  two points
% // return value: distance calculated
% // NOTE:		
% //-----------------------------------------------------------------------------
function [distance] = FindDistance (p1, p2)
	dX = 0; dY = 0; distance = 0;
    if( (p1(1,1) > 0) && (p1(1,2) > 0) && (p2(1,1) > 0) && (p2(1,2) > 0))
        dX = (p1(1,1) - p2(1,1)) * (p1(1,1) - p2(1,1));

        dY = (p1(1,2) - p2(1,2)) * (p1(1,2) - p2(1,2));

        distance = sqrt (dX+dY);
    else
        distance = 10000;
    end

 % $Log: FindDistance.m,v $
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