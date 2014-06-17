function [] = SegmentFolder(folderpath)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    if exist([folderpath '/Prostate'],'dir') 
        SegmentOrgan(folderpath,'/Prostate/');
    end
    SegmentOrgan(folderpath,'/Kidney/')    
    SegmentOrgan(folderpath,'/Liver/')   

end