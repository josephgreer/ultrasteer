%..........................................................................
%% LOAD IMAGES AND STEREO DATA
%..........................................................................
addpath('/Users/annmajewicz/Documents/MATLAB/MultipleViewGeometry');
addpath('/Users/annmajewicz/Documents/MATLAB/MultipleViewGeometry/vgg_examples');
addpath('/Users/annmajewicz/Documents/MATLAB/MultipleViewGeometry/vgg_multiview');
addpath('/Users/annmajewicz/Documents/MATLAB/MultipleViewGeometry/vgg_numerics');
addpath('/Users/annmajewicz/Documents/MATLAB/MultipleViewGeometry/vgg_ui');



l_img_name = 'IMAGES/DEWARPED/85/test001.tiff';
r_img_name = 'IMAGES/DEWARPED/95/test001.tiff';
l_img_back = 'IMAGES/DEWARPED/85/test001_back.tiff';
r_img_back = 'IMAGES/DEWARPED/95/test001_back.tiff';
organ = '/Kidney/';
load([folderpath organ 'IMAGES/PoseEstimate/stereoPair_test001.mat']);

 
LeftI = imread([folderpath organ l_img_name], 'tiff');
LeftI = im2double(LeftI);

LeftIback = imread([folderpath organ l_img_back], 'tiff');
LeftIback = im2double(LeftIback);

RightI = imread([folderpath organ r_img_name], 'tiff');
RightI = im2double(RightI);

RightIback = imread([folderpath organ r_img_back], 'tiff');
RightIback = im2double(RightIback);


camera1.ratio = [0.45; 0.45];               %
camera1.origin = stereo.left.cc';
camera1.focal = -1*stereo.left.fc(1);

camera2.ratio = [0.45; 0.45];
camera2.origin = stereo.right.cc';
camera2.focal = -1*stereo.right.fc(1);

cameras{1} = camera1;
cameras{2} = camera2;

poses{1} = stereo.G_world2left;
poses{2} = stereo.G_world2right; 

% reformating camera parameters
for i = 1:2
      ex(i,1) = cameras{i}.ratio(1);
      ey(i,1) = cameras{i}.ratio(2);
      xo(i,1) = cameras{i}.origin(1);
      yo(i,1) = cameras{i}.origin(2);
      focal(i,1) = cameras{i}.focal;
end

Kl=[-1*focal(1)/ex(1),0,xo(1),0;0,-1*focal(1)/ey(1),yo(1),0;0,0,1,0];
Kr=[-1*focal(2)/ex(2),0,xo(2),0;0,-1*focal(2)/ey(2),yo(2),0;0,0,1,0];

Pl = Kl*poses{1};
Pr = Kr*poses{2};

% Compute F from P's
F = vgg_F_from_P(Pl,Pr);

% Test with known pixels
ptl = [275; 40; 1];
%ptl = [292; 125; 1];

%ptl = [168; 290; 1];
lr = F*ptl;

% Display
%vgg_gui_F(LeftI, RightI , F');  % WHY F TRANSPOSE??
%disp('Computed epipolar geometry. Move the mouse to verify')


figure(1);  
imshow(LeftI)
hold on;
plot(ptl(2), ptl(1), 'r*'); 

% Getting the epipolar line on the RIGHT image:
[m n] = size(LeftI);
ptr = zeros(2,2*n);
ptr(1,:)= 1:2*n;

% Using the eqn of line: ax+by+c=0; y = (-c-ax)/b
ptr(2,:) = (-lr(3)-lr(1)*ptr(1,:))/lr(2);

figure(2); 
imshow(RightI);
hold on; plot(ptr(2,:), ptr(1,:), 'r');


% Now finding the other epipolar line on the left image itself:
    
% We know that left epipole is the 3rd column of V.
% We get V from svd of F. F=UDV'
[FU, FD, FV] = svd(F);
left_epipole = F(:,3);
left_epipole = left_epipole/left_epipole(3);

% Hence using the left epipole and the given input point on left
% image we plot the epipolar line on the left image
left_epipolar_x = 1:2*m;
left_epipolar_y =  ptl(2) + (left_epipolar_x-ptl(1))*(left_epipole(2)-ptl(2))/(left_epipole(1)-ptl(1));
        
figure(1); hold on; plot(left_epipolar_y, left_epipolar_x, 'r');

%% TEST MATCH
[corres_pixels, correspondance_metric] = find_corres_pix2  (ptl(1:2), 'left',...
    (RightIback-RightI), 'right', stereo, [], 1)

figure(2)
hold on
plot(corres_pixels(1,:), corres_pixels(2,:), '*b')
