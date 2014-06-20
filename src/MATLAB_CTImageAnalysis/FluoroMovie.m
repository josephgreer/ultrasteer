clear; clc; close all;

inDir = 'C:\Users\Rotator\Documents\Matlab_CTImageAnalysis\FL_-_ANGIO_0051';
outDir = 'C:\Users\Rotator\Documents\Matlab_CTImageAnalysis\Movies';
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
writeObj = VideoWriter(outPath, profiles(1).Name); %uncompressed AVI
framerate = 1000/info.FrameTime;
writeObj.FrameRate = framerate;
open(writeObj);

for i=1:info.NumberOfFrames
    display(['Frame ' num2str(i)]);
    im = mat2gray(data(:,:,1,i));
    writeVideo(writeObj,im);
end

close(writeObj);