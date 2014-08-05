clear; clc; close all;

inDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\NS Porcine Cadaver Study\1 - Data\NEEDLE_GUIDANCE_HENRIETTA_123456\__20140611_180621_062000\FL_-_ANGIO_0038';
outDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\NS Porcine Cadaver Study\1 - Data\NEEDLE_GUIDANCE_HENRIETTA_123456\__20140611_180621_062000\FluoroMovies';
idxs = strfind(inDir, '\');
outFile = inDir(idxs(length(idxs))+1:length(inDir));;
inFile = dir(inDir);
assert(length(inFile) == 3);
inFile = inFile(3).name;
inPath = strcat(inDir,'/',inFile);
outPath = strcat(outDir,'/',outFile,'.mp4');

info = dicominfo(inPath);
data = dicomread(inPath);

writeObj = VideoWriter(outPath);
open(writeObj);
repeat = 10;

for i=1:info.NumberOfFrames
    im = mat2gray(data(:,:,1,i));
    for j=1:repeat
        writeVideo(writeObj,im);
    end
end

close(writeObj);