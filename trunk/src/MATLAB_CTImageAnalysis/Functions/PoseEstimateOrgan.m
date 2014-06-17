function [] = PoseEstimateOrgan(folderpath,organ,StereoXrayParameters)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

if ~exist([folderpath organ 'IMAGES/PoseEstimate'],'dir')
    mkdir([folderpath organ 'IMAGES/PoseEstimate']);
    startIDX = 1;
else
    files = dir([folderpath organ 'IMAGES/PoseEstimate/stereoPose_test*']);
    
    if isempty(files)
        startIDX = 1;
    else
        startIDX = length(files)+1;
    end
end
if startIDX <= 9
    imagePoseEstimation(startIDX,[folderpath organ 'IMAGES/FTRAC_Segmentation/'],[folderpath organ 'IMAGES/PoseEstimate/'],StereoXrayParameters);
end
