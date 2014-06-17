function FiducialSegmentationFolder(folderpath)

    if exist([folderpath '/Prostate'],'dir') 
           FiducialSegmentationOrgan(folderpath,'/Prostate/');
    end
    FiducialSegmentationOrgan(folderpath,'/Kidney/')    
    FiducialSegmentationOrgan(folderpath,'/Liver/')    
  

end


 