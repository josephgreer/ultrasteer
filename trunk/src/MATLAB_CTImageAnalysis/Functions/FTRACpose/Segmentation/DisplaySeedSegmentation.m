% $Id: DisplaySeedSegmentation.m,v 1.3 2007/04/02 20:06:29 anton Exp $

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
    global SeedSegmentation;

    cla;
    % refresh the image
    % CurrentImageHandle = imshow(histeq(I(seedROIStartX:seedROIStartX + seedROIXWidth, seedROIStartY:seedROIStartY + seedROIYWidth)), 255);
    ICopy = SeedSegmentation.I;
    CurrentImageHandle = imshow(ICopy(SeedSegmentation.ROI.StartX:SeedSegmentation.ROI.StartX + SeedSegmentation.ROI.SizeX, SeedSegmentation.ROI.StartY:SeedSegmentation.ROI.StartY + SeedSegmentation.ROI.SizeY));
    hold on;
    if (~isempty(SeedSegmentation.DataSet))
        % final data set: [region/seed-number CGx CGy Orientation OrientationValidity PartOfCluster] 
        % note that the CGx is in image coordinate system, while plotting, x & y
        % should be swapped
        for iCG = 1:length(SeedSegmentation.DataSet(:,1))
            CG = SeedSegmentation.DataSet(iCG,1:2);
            if (SeedSegmentation.DataSet(iCG,5) == 1)
                plot(CG(1,2) - SeedSegmentation.ROI.StartY + 1, CG(1,1) - SeedSegmentation.ROI.StartX + 1, 'mo', 'MarkerSize', 12);
            else
                plot(CG(1,2) - SeedSegmentation.ROI.StartY + 1, CG(1,1) - SeedSegmentation.ROI.StartX + 1, 'r.', 'MarkerSize', 8);        
            end
        end
    end
    hold off;


% $Log: DisplaySeedSegmentation.m,v $
% Revision 1.3  2007/04/02 20:06:29  anton
% DisplaySeedSegmentation.m: Changed color and size of marks on seeds to
% avoid green over almost white (was barely visible).
%
% Revision 1.2  2007/01/02 21:47:23  anton
% SeedSegmentation: Swap definition of X and Y to make more sense
%
% Revision 1.1  2007/01/02 16:19:13  anton
% DisplaySeedSegmentation: Function for seeds only, ROI independant
%
% Revision 1.3  2005/12/23 00:51:17  jain
% The five files from Siddharth that I got are added here. Also I checked if these files
% are from which version - looks like cvs does not change the dates of the files and
% hence the confusion. This version is stable with bad cases, but output is still to be
% taken care of - uiwait will be added in the next version.
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
