% $Id: DisplaySeedSegmentationWholeImage.m,v 1.4 2006/04/20 20:40:34 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplaySeedSegmentationWholeImage() 
% // file:		DisplaySeedSegmentationWholeImage.m
% // author:		siddharth
% // date:		
% // description:	display the results of seed segmentation overlaid on
% the whole image
% 
% // assumed input:	
% 
% global variable I (image buffer)
% variables that identify the coordinate system of the results
%           global SeedSegmentationROITranslationVector;
% variable which contains the final results, 
%             global SeedSegmentationFinalInformationDataSet;
% 
% 
% output: none
% 
% // calling function: called when Select region button is pressed, after
% performing segmentation, in threshold callback, after user clicks 'Accept
% results'
% // NOTE: 
% //-----------------------------------------------------------------------------
function DisplaySeedSegmentationWholeImage()
    global rufSegmentationData;
    if (rufSegmentationData.SeedSegmentationPerformed == true)        
        % global variable required:
        global SeedSegmentationROITranslationVector;
        global SeedSegmentationFinalInformationDataSet;
        CurrentAxesHandle = gca;
        hold on;

        % final data set: [region/seed-number CGx CGy Orientation OrientationValidity PartOfCluster] 
        % note that the CGx is in image coordinate system, while plotting, x & y
        % should be swapped
        for iCG = 1:length(SeedSegmentationFinalInformationDataSet(:,1))
            CG = SeedSegmentationFinalInformationDataSet(iCG,1:2);
            if (SeedSegmentationFinalInformationDataSet(iCG,5) == 1)
                plot(CG(1,2) + SeedSegmentationROITranslationVector(1,2), CG(1,1) + SeedSegmentationROITranslationVector(1,1), 'r.');
            else
                plot(CG(1,2) + SeedSegmentationROITranslationVector(1,2), CG(1,1) + SeedSegmentationROITranslationVector(1,1), 'g.');        
            end
        end
        hold off;
    end

% $Log: DisplaySeedSegmentationWholeImage.m,v $
% Revision 1.4  2006/04/20 20:40:34  anton
% Segmentation: DisplayXyzSegmentationWholeImage, use global flag to
% determine if should draw or not, tabify/format.
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
