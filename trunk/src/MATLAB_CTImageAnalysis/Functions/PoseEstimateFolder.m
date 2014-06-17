function [] = PoseEstimateFolder(folderpath)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    StereoXrayParameters = load([folderpath '/StereoXrayParameters.mat']);
    StereoXrayParameters = StereoXrayParameters.StereoXrayParameters;
    if exist([folderpath '/Prostate'],'dir') 
        PoseEstimateOrgan(folderpath,'/Prostate/',StereoXrayParameters);
    end
    PoseEstimateOrgan(folderpath,'/Kidney/',StereoXrayParameters)    
    PoseEstimateOrgan(folderpath,'/Liver/',StereoXrayParameters)   

end

