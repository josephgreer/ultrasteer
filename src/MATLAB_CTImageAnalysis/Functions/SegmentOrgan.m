
function [] = SegmentOrgan(folderpath,organ)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here


if ~exist([folderpath organ 'IMAGES/Segmentation'],'dir')
    mkdir([folderpath organ 'IMAGES/Segmentation']);
    startIDX = 1;
else
    files = dir([folderpath organ 'IMAGES/Segmentation/centers*']);
    
    if isempty(files)
        startIDX = 1;
    else
        startIDX = length(files)+1;
    end
end
if startIDX <= 9
    imageSegmentation(startIDX,[folderpath organ 'IMAGES/DEWARPED/'],[folderpath organ 'IMAGES/Segmentation/']);
end


end