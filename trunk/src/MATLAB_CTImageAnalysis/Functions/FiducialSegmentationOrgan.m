function [] = FiducialSegmentationOrgan(folderpath, organ)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
if ~exist([folderpath organ 'IMAGES/FTRAC_Segmentation'],'dir')
    mkdir([folderpath organ 'IMAGES/FTRAC_Segmentation']);
    mkdir([folderpath organ 'IMAGES/FTRAC_Segmentation/85']);
    mkdir([folderpath organ 'IMAGES/FTRAC_Segmentation/95']);
    start85 = 1;
    start95 = 1;
else
    files85 = dir([folderpath organ 'IMAGES/FTRAC_Segmentation/85/*' '_test*' '.txt']);
    files95 = dir([folderpath organ 'IMAGES/FTRAC_Segmentation/95/*' '_test*' '.txt']);
    
    if isempty(files85)
        start85 = 1;
    else
        bbfiles = dir([folderpath organ 'IMAGES/FTRAC_Segmentation/85/Ellipse2_test*' '.txt']);
        start85 = length(bbfiles)+1;
    end
    
    
    if isempty(files95)
        start95 = 1;
    else
        bbfiles = dir([folderpath organ 'IMAGES/FTRAC_Segmentation/95/Ellipse2_test*' '.txt']);
        start95 = length(bbfiles)+1;
    end
    

end

imageFiducialSegmentation(start85,[folderpath organ 'IMAGES/DEWARPED/85'],[folderpath organ 'IMAGES/FTRAC_Segmentation/85']); % OLD MAY HAVE TO REDO!
imageFiducialSegmentation(start95,[folderpath organ 'IMAGES/DEWARPED/95'],[folderpath organ 'IMAGES/FTRAC_Segmentation/95']);

end

   
   
    