% $Id: FindGoodPoint.m,v 1.3 2006/09/22 21:54:38 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	FindGoodPoint() 
% // file:		FindGoodPoint.m
% // author:		siddharth
% // date:		
% // description: With a given point, average intensity , and intensity
% image, tried to look for a single peak intensity profile in 1 pixel
% left-right window of the given pixel; if the constraints are met then
% that point is returned as good point

% // input:	intensity image, Given point, average intensity along given
% list of points

% // output: flag indicating whether or not the point was found, followed
% by a point found

% // calling function: ExtractLine(..)

% // NOTE: This is because we are looking for points exactly in the center
% of the line; sinc ethe line is quite thick, the points obtained by
% line-graph traversal algorithm may or may not be on the center of the
% line.
% //-----------------------------------------------------------------------------
function [bPointFound, pGoodPoint] = FindGoodPoint(IintImage, iAvgIntensity, PointDetected)
iHighCount = 0;
iLowCount = 0;

bPointFound = false;

pGoodPoint = PointDetected;

if( (PointDetected (1,1) > 0) && (PointDetected(1,2) > 0))
    
    
% window -1 0 1
iWindowIntensityArray = [IintImage(PointDetected(1,1), (PointDetected(1,2)-1)); IintImage(PointDetected(1,1), PointDetected(1,2)); IintImage(PointDetected(1,1), (PointDetected(1,2)+1))];

iHighCount = find(iWindowIntensityArray >= iAvgIntensity);

iLowCount = find(iWindowIntensityArray < iAvgIntensity);

iIndex =  find(iWindowIntensityArray == max(iWindowIntensityArray));

% if(length(iHighCount)== 1)
if(length(iIndex == 1))
    if(iIndex == 1)
        %if (iWindowIntensityArray(iIndex) > IintImage(IintImage(PointDetected(1,1), (PointDetected(1,2)-2))))
        if (iWindowIntensityArray(iIndex) > IintImage(PointDetected(1,1), (PointDetected(1,2)-2)))
            bPointFound = true;
            pGoodPoint = [PointDetected(1,1) (PointDetected(1,2)-1)];
        end
    else
        if(iIndex == 2)
            bPointFound = true;
             pGoodPoint = [PointDetected(1,1) PointDetected(1,2)];
        else
            if(iIndex == 3)
              
 %               if (iWindowIntensityArray(iIndex) > IintImage(IintImage(PointDetected(1,1), (PointDetected(1,2)+2))))
                 if (iWindowIntensityArray(iIndex) > IintImage(PointDetected(1,1), (PointDetected(1,2)+2)))
                    bPointFound = true;
                    pGoodPoint = [PointDetected(1,1) (PointDetected(1,2)+1)];
                end
                
            end
        end
    end
    
end
% end

end
% $Log: FindGoodPoint.m,v $
% Revision 1.3  2006/09/22 21:54:38  anton
% Segmentation FindGoodPoint: Typos added with bad copy/paste
%
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