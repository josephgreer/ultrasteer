function [] = CalibrateFolder(folderpath)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

if ~exist([folderpath '/StereoXrayParameters.mat'])
       XrayCalibration(folderpath);
end

end

