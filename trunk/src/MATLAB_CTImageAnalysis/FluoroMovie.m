for ii = [26 30 31 37 39 40 42 44 45 46 48 49 50 52 53]
    clc; close all;
    
    display(sprintf('Sequence Number: %d', ii));
    inDir = strcat('F:\NEEDLE_GUIDANCE_HENRIETTA_123456\__20140611_180621_062000\DR_OVERVIEW_00',num2str(ii));
    outDir = 'F:\NEEDLE_GUIDANCE_HENRIETTA_123456\FluoroMovies';
    idxs = strfind(inDir, '\');
    outFile = inDir(idxs(length(idxs))+1:length(inDir));
    inFile = dir(inDir);
    assert(length(inFile) == 3);
    inFile = inFile(3).name;
    inPath = strcat(inDir,'/',inFile);
    outPath = strcat(outDir,'/',outFile,'.avi');
    
    info = dicominfo(inPath);
    data = dicomread(inPath);
    
    profiles = VideoWriter.getProfiles();
    writeObj = VideoWriter(outPath, profiles(4).Name); %uncompressed AVI
    framerate = 1000/info.FrameTime;
    writeObj.FrameRate = framerate;
    open(writeObj);
    
    for i=1:info.NumberOfFrames
        display(['Frame ' num2str(i)]);
        im = mat2gray(data(:,:,1,i));
        writeVideo(writeObj,im);
    end
    
    close(writeObj);
end