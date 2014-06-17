function [] = ReconstructFolder(folderpath,index)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    if exist([folderpath '/Prostate'],'dir') 
        ReconstructOrgan(folderpath,'/Prostate/',index);
    end
    ReconstructOrgan(folderpath,'/Kidney/',index)    
    ReconstructOrgan(folderpath,'/Liver/',index)   

end

