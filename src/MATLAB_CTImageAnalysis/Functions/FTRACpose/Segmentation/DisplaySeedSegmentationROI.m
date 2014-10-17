% $Id: DisplaySeedSegmentationROI.m,v 1.3 2005/12/23 00:51:17 jain Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplaySeedSegmentationROI() 
% // file:		DisplaySeedSegmentationROI.m
% // author:		siddharth
% // date:		
% // description:	display the results of seed segmentation overlaid on
% the region of interest and also attaches Left-click event listener to the
% image area displayed & to the points plotted
% 
% // assumed input:	
% global variable I (image buffer)
% variables that identify the region of interest coordinates
%           global seedROIStartX; 
%           global seedROIStartY; 
%           global seedROIXWidth; 
%           global seedROIYWidth;
%           global SeedSegmentationROITranslationVector;
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that identifies the current image handle
%             global CurrentImageHandle;
% 
% output: none
% 
% // calling function: called when Select region button is pressed, after
% performing segmentation, in threshold callback, after user performs some
% action i.e some change, results need to be visually updated, then this
% function is called.
% // NOTE: 
% //-----------------------------------------------------------------------------
function DisplaySeedSegmentationROI()
% global variable required:
global  I;
global seedROIStartX; global seedROIStartY; global seedROIXWidth; global seedROIYWidth;
global CurrentImageHandle;
global FloatingSeedSegmentationFinalInformationDataSet;


cla;
% refresh the image
CurrentImageHandle = imshow(I(seedROIStartX:seedROIStartX + seedROIXWidth, seedROIStartY:seedROIStartY + seedROIYWidth));

set(CurrentImageHandle, 'ButtonDownFcn', 'ImageAxesClick_Callback');

CurrentAxesHandle = gca;
hold on;

if (~isempty(FloatingSeedSegmentationFinalInformationDataSet))

    % final data set: [region/seed-number CGx CGy Orientation OrientationValidity PartOfCluster] 
    % note that the CGx is in image coordinate system, while plotting, x & y
    % should be swapped
    for iCG = 1:length(FloatingSeedSegmentationFinalInformationDataSet(:,1))
        CG = FloatingSeedSegmentationFinalInformationDataSet(iCG,1:2);
        if (FloatingSeedSegmentationFinalInformationDataSet(iCG,5) == 1)
            plot(CG(1,2), CG(1,1), 'r.','ButtonDownFcn', 'ImageAxesClick_Callback');
        else
            plot(CG(1,2), CG(1,1), 'g.', 'ButtonDownFcn', 'ImageAxesClick_Callback');        
        end

    end

end

hold off;

pause(0.5);


% $Log: DisplaySeedSegmentationROI.m,v $
% Revision 1.3  2005/12/23 00:51:17  jain
% The five files from Siddharth that I got are added here. Also I checked if these files
% are from which version - looks like cvs does not change the dates of the files and
% hence the confusion. This version is stable with bad cases, but output is still to be
% taken care of - uiwait will be added in the next version.
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%