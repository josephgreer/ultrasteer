function DewarpFolder(folderpath)

if ~exist([folderpath '/DEWARPEDCal85.tiff']) %TEST
    cal85 = imread([folderpath '/Cal85.tiff']);
    cal95 = imread([folderpath '/Cal95.tiff']);

    [dewarpedImg85, dewarpParams85] = Dewarp(cal85);
    [dewarpedImg95, dewarpParams95] = Dewarp(cal95);

    imwrite(dewarpedImg85,[folderpath '/DEWARPEDCal85.tiff'], 'tiff');
    imwrite(dewarpedImg95,[folderpath '/DEWARPEDCal95.tiff'], 'tiff');

    d85 = imtransform(cal85, dewarpParams85);
    d95 = imtransform(cal95, dewarpParams95);

    imwrite(d85,[folderpath '/DEWARPEDCal85_xfm.tiff'], 'tiff');
    imwrite(d95,[folderpath '/DEWARPEDCal95_xfm.tiff'], 'tiff');
end

 pFolder = dir([folderpath '/Prostate/']);
    if ~isempty(pFolder) 
        dewarpedP = dir([folderpath '/Prostate/IMAGES/DEWARPED' '/*5']);
        if isempty(dewarpedP)
           DewarpOrgan(dewarpParams85,dewarpParams95,folderpath,'/Prostate/');
        end
    end
    dewarpedK = dir([folderpath '/Kidney/IMAGES/DEWARPED' '/*5']);
    dewarpedL = dir([folderpath '/Liver/IMAGES/DEWARPED' '/*5']);
    if isempty(dewarpedK)
        DewarpOrgan(dewarpParams85,dewarpParams95,folderpath,'/Kidney/')    
    end
    if isempty(dewarpedL)
        DewarpOrgan(dewarpParams85,dewarpParams95,folderpath,'/Liver/')    
    end
    
end