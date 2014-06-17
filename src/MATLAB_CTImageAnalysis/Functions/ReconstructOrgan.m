
function [] = ReconstructOrgan(folderpath,organ,index)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here


if ~exist([folderpath organ 'IMAGES/Reconstruction'],'dir')
    mkdir([folderpath organ 'IMAGES/Reconstruction']);
    startIDX = 1;
else
    files = dir([folderpath organ 'IMAGES/Reconstruction/needleErr*']);
    
    if isempty(files)
        startIDX = 1;
    else
        startIDX = length(files)+1;
    end
end
if startIDX <= 9
    imageReconstruction(startIDX,[folderpath organ 'IMAGES/DEWARPED/'],[folderpath organ 'IMAGES/PoseEstimate/'],[folderpath organ 'IMAGES/Segmentation/'],[folderpath organ 'IMAGES/Reconstruction/'],index);
end


end

