function XrayCalibration(folderpath)

cal85 = imread([folderpath '/DEWARPEDCal85.tiff']);
cal95 = imread([folderpath '/DEWARPEDCal95.tiff']);

[prm85, linesA85, linesB85] = ImageCalibration(cal85);
[prm95, linesA95, linesB95] = ImageCalibration(cal95);

StereoXrayParameters(:,1) = [prm85(1);prm85(2);prm85(3)];
StereoXrayParameters(:,2) = [prm95(1);prm95(2);prm95(3)];

save([folderpath '/StereoXrayParameters.mat'],'StereoXrayParameters');


save 
