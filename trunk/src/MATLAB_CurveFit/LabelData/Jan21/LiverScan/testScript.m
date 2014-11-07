% Needle:  282.401367 -8.036719 0.446484
% 
% Image Coordinates:  
% cenX	408.37726	float
% cenY	168.90001	float
% 
% Transducer: 
clear; clc; close all;
% pos =  [282.401367 -8.259961 0.892969]';
% % pose = [-0.985474 0.160034 -0.056641; 0.153931 0.702393 -0.694946; -0.071533 -0.693481 -0.716919];
% pose = [0.018921 -0.023926 0.999390; 0.067261 0.997437 0.022583; -0.997559 0.066772 0.020508];
% 
% Needle = [282.401367 -8.036719 0.446484];
% ul = [90 0]';
% mpp = 83;
% 
% imagePt = [408.37726; 168.90001];
% 
% imageMM = (imagePt-ul)*83/1000;
% 
% calibration = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
% calibMod = [calibration(:,3) calibration(:,2) calibration(:,1)];
% sensor = calibration*[1 imageMM(2) imageMM(1)]';
% world = (pose*sensor)+pos;
% 
% display(world);
% Needle = Needle';
% display(Needle);
% map = pose*calibMod
% 
% ul = [90 0]';
% mpp = 83;
% 
% 
% needle = [328.612500 33.932812 -4.018359]'
% image = [377.03973 180.5]';
% pos =  [337.095703 14.064258 19.422070]'
% pose = [0.037231 -0.075928 0.996338; 0.109375 0.991333 0.071411; -0.993286 0.106201 0.045166];
% imageMM = (image-ul)*83/1000;
% 
%calibration = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
%calibMod = [calibration(:,3) calibration(:,2) calibration(:,1)];
% sensor = calibration*[1 imageMM(2) imageMM(1)]';
% world = (pose*sensor)+pos
% map = pose*calibMod;
% map(:,3)+pos
% pos = [238.645898 78.134766 -87.064453]'
% pose = [-0.039063 0.999023 -0.016113; -0.998779 -0.039551 -0.031494; -0.032104 0.014771 0.999268];
% 
% needle1 = [231.948633 59.605664 -87.287695]';
% needle2 = [273.471680 62.061328 -84.385547]';
% 
% map = pose*calibMod;
% map(:,3)+pos

needle = [284.857031 27.458789 30.360937]';
pos =  [280.392181 15.626953 54.247852]';
pose = [0.169800 -0.251465 0.952759; -0.030762 0.964966 0.260132; -0.984985 -0.073486 0.156128];

calibration = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
imagePt = [319.59454 157.46234]';
midPt = [320 0]';
imageMM = (imagePt-midPt)*83/1000;

sensor = calibration*[1 imageMM(2) imageMM(1)]';
world = (pose*sensor)+pos;




