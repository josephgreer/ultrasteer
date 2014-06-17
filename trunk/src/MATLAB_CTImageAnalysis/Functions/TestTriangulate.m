%--------------------------------------------------------------------------
% TEST TRIANGULATE!!
%--------------------------------------------------------------------------

%..........................................................................
%% LOAD IMAGES AND STEREO DATA
%..........................................................................
l_img_name = 'IMAGES/DEWARPED/85/test001.tiff';
r_img_name = 'IMAGES/DEWARPED/95/test001.tiff';
organ = '/Kidney/';
load([folderpath organ 'IMAGES/PoseEstimate/stereoPair_test001.mat']);
 
LeftI = imread([folderpath organ l_img_name], 'tiff');
LeftI = im2double(LeftI);

RightI = imread([folderpath organ r_img_name], 'tiff');
RightI = im2double(RightI);

%..........................................................................
% DEFINE FTRAC MODEL POINTS
%..........................................................................
D = 20;
th = 30*pi/180;
[model_pts, model_line_pts, model_ellipse1_pts, model_ellipse2_pts] = Fiducial_points(D, th);

figure
plot3(model_pts(:,1),model_pts(:,2),model_pts(:,3),'.b');
hold on
plot3(model_line_pts(:,1),model_line_pts(:,2),model_line_pts(:,3),'.r');
hold on
plot3(model_ellipse1_pts(:,1),model_ellipse1_pts(:,2),model_ellipse1_pts(:,3),'.g');
hold on 
plot3(model_ellipse2_pts(:,1),model_ellipse2_pts(:,2),model_ellipse2_pts(:,3),'.c');
hold off
axis equal


%..........................................................................
% DEFINE PARAMETERS FOR TRIANGUALTION
%..........................................................................
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
%%
% TEST REPROJECTION OF FTRAC MODEL
%..........................................................................
ftrac3D = [model_pts;model_line_pts; model_ellipse1_pts; model_ellipse2_pts];
%ftrac2D{1}=sm_backproj_general(ftrac3D,poses{1},ex(1),ey(1),xo(1),yo(1),focal(1));
%ftrac2D{2}=sm_backproj_general(ftrac3D,poses{2},ex(2),ey(2),xo(2),yo(2),focal(2));
fiducialDirectory = [folderpath organ 'IMAGES/FTRAC_Segmentation/'];
BBsFileLeft = strcat(fiducialDirectory,'85/', 'BBs_test001.txt');
BBsFileRight = strcat(fiducialDirectory,'95/', 'BBs_test001.txt');
BBsLeft = load(BBsFileLeft, '-ascii');
BBsRight = load(BBsFileRight, '-ascii');
ftrac2D{1} = BBsLeft;
ftrac2D{2} = BBsRight;
[ftracErr, ftrac3Drecon, ftrac2Dproj] = calculate_bpe_final(ftrac2D,poses,ex,ey,xo,yo,focal,2);


err3D = sum(1/9*sqrt(sum((ftrac3Drecon-ftrac3D(1:9,:)).^2,2)))*0.45          % ERROR IN MM
err2D_1 = sum(1/9*sqrt(sum((ftrac2Dproj{1}-ftrac2D{1}).^2,2)))*0.45 
err2D_2 = sum(1/9*sqrt(sum((ftrac2Dproj{2}-ftrac2D{2}).^2,2)))*0.45 


figure
subplot(1,2,1)
imshow(LeftI)
hold on
plot(ftrac2Dproj{1}(:,2),ftrac2Dproj{1}(:,1),'.b');
hold on 
plot(BBsLeft(:,2),BBsLeft(:,1),'.r');
hold off
subplot(1,2,2)
imshow(RightI)
hold on
plot(ftrac2Dproj{2}(:,2),ftrac2Dproj{2}(:,1),'.b');
hold on
plot(BBsRight(:,2),BBsRight(:,1),'.r');
hold off

%% .........................................................................
% TEST MATCHED NEEDLE POINTS
%..........................................................................

leftCentersTest = [285,285,285,285,285,285,285,285,285,285,285,285,285,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284;268,269,270,271,272,273,274,275,276,277,278,279,280,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257]';
rightCentersTest = [264,266,267,268,269,270,271,272,273,273,274,275,276,237,239,240,241,241,243,244,245,246,247,248,249,250,251,252,253,254;200,201,201,201,201,201,201,201,201,201,201,201,201,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200]';
rightCentersTest = [rightCentersTest(:,2) rightCentersTest(:,1)];

figure;
imshow(LeftI);
hold on
plot(leftCentersTest(:,2),leftCentersTest(:,1),'.b');
hold off

figure;
imshow(RightI);
hold on
plot(rightCentersTest(:,2),rightCentersTest(:,1),'.b');
hold off

centersTest{1} = leftCentersTest;
centersTest{2} = rightCentersTest;
[centersErr, centers3Drecon, centers2Dproj] = calculate_bpe_final(centersTest,poses,ex,ey,xo,yo,focal,2);

figure
plot3(model_pts(:,1),model_pts(:,2),model_pts(:,3),'.b');
hold on
plot3(model_line_pts(:,1),model_line_pts(:,2),model_line_pts(:,3),'.r');
hold on
plot3(model_ellipse1_pts(:,1),model_ellipse1_pts(:,2),model_ellipse1_pts(:,3),'.g');
hold on 
plot3(model_ellipse2_pts(:,1),model_ellipse2_pts(:,2),model_ellipse2_pts(:,3),'.c');
hold on
plot3(centers3Drecon(:,1),centers3Drecon(:,2),centers3Drecon(:,3),'.k');
axis equal


figure
subplot(1,2,1)
imshow(LeftI)
hold on
plot(centers2Dproj{1}(:,2),centers2Dproj{1}(:,1),'.b');
hold off
subplot(1,2,2)
imshow(RightI)
hold on
plot(centers2Dproj{2}(:,2),centers2Dproj{2}(:,1),'.b');
hold off


% %% .........................................................................
% % COLLECT 3D INPUT POINTS
% %..........................................................................
% nPoints = 3;
% 
% figure;
% imshow(LeftI);
% pix_left = ginput(nPoints)';
% pix_left = cornerfinder(pix_left, LeftI); %??
% 
% figure;
% imshow(RightI);
% pix_right= ginput(nPoints)';
% pix_right = cornerfinder(pix_right, RightI);

% 
% %% TEST CAMERA FRAMES
% figure
% f_3Dframe(stereo.G_world2left*stereo.G_left2world,'Y',1,'FTRAC');
% hold on
% f_3Dframe(stereo.G_world2left,'R',1,'W2L');
% hold on
% f_3Dframe(stereo.G_world2right,'B',1,'W2R');
% hold on
% f_3Dframe(stereo.G_world2right*stereo.G_right2left,'C',1,'W2Rxfm');
% hold off
% axis([-70 0 0 10 0 900])

