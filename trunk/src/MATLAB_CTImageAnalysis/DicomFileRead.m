% Portable Needle Steering Project
% DICOM File Reader
% Author: Ann Majewicz


clear all
close all
clc
addpath('/Functions');


%folder = 'DICOM/Scan1/';
%filenameBase = 'DICOM/Scan1/Scan1-';

folder = 'DICOM/Scan2/';
filenameBase = 'DICOM/Scan2/Scan2-';

%folder = 'DICOM/Scan3/';
%filenameBase = 'DICOM/Scan3/Scan3-';

file = dir(folder);
numberFiles = length(file)-2;
for i = 1:numberFiles
    numberzeros = 3-length(int2str(i));
    pad = [];
    for j=1:numberzeros
        pad = [pad '0'];
       
    end
    x = dicomread([filenameBase pad int2str(i) '.dcm']);
    data(:,:,i) = x;
   % image = x;
  %  figure;
  %  imshow(image);
end
%figure
isosurface(data)
%figure
%movie(M)

