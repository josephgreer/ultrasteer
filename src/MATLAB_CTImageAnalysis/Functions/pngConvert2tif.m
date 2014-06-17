% Convert .png to .tiff

clear all
close all
clc


folder = 'Robot Set Up Testing/FTRAC Tests/';
filenameBase = 'dvgrab-';
files = dir([folder filenameBase '*.png']);
nFiles = length(files);

for i = 1:nFiles
    numberzeros = 3-length(int2str(i));
    pad = [];
    for j=1:numberzeros
        pad = [pad '0'];
       
    end
    x = imread([folder filenameBase pad int2str(i) '.png']);
    %imshow(x);
    imwrite(x,[folder filenameBase  pad int2str(i) '.tiff'],'tiff');
end

