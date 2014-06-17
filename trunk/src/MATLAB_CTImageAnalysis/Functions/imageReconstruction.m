function [] = imageReconstruction(startIDX,imgDirectory, poseDirectory,segmentationDirectory,reconstructionDirectory,index)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

for i=startIDX:length(dir([poseDirectory 'stereoPair*']))
    close all
    imagePrefix = ['test00' int2str(i)];
    
    %......................................................................
    % LOAD IMAGES, STEREO DATA, AND SEGMENTED NEEDLES
    %......................................................................
    load([poseDirectory 'stereoPair_' imagePrefix '.mat']);
    stereo.left.pixel_width = 0.45;
    stereo.right.pixel_width = 0.45;
    leftImg = im2double(imread([imgDirectory '85/' imagePrefix '.tiff']));
    rightImg = im2double(imread([imgDirectory '95/' imagePrefix '.tiff']));
    leftBack = im2double(imread([imgDirectory '85/' imagePrefix '_back.tiff']));
    rightBack = im2double(imread([imgDirectory '95/' imagePrefix '_back.tiff']));
      
    %load([segmentationDirectory 'centers' int2str(i) '.mat']);
    leftImgNeedle = im2double(imread([segmentationDirectory 'leftImgNeedle_' imagePrefix '.tiff']));
    rightImgNeedle = im2double(imread([segmentationDirectory 'rightImgNeedle_' imagePrefix '.tiff']));
    
    % NOTE: ONLY HAVE TO DO THIS FIRST TIME AROUND, SEGMENTATION IS ALREADY FIXED!    
    leftImgNeedle = bwmorph(leftImgNeedle,'thin');
    rightImgNeedle = bwmorph(rightImgNeedle,'thin');
    
    [leftCenters_row, leftCenters_col] = find (leftImgNeedle> 0);
    [rightCenters_row, rightCenters_col] = find (rightImgNeedle> 0);
       
    [lr,li] = sort(leftCenters_row,1,'descend');
    leftCenters = [lr,leftCenters_col(li)]';
    
    [rr,ri] = sort(rightCenters_row,1,'descend');
    rightCenters = [rr,rightCenters_col(ri)]';
    
    centers = cell(1,2);
    centers{1} = leftCenters;
    centers{2} = rightCenters;
    
    save([segmentationDirectory 'centers' int2str(i) '.mat'], 'centers');  % NOTE ONLY HAVE TO DO THIS ONCE TO 
    imwrite(leftImgNeedle,[segmentationDirectory 'leftImgNeedleThin_test00' int2str(i)  '.tiff'], 'tiff');
    imwrite(rightImgNeedle,[segmentationDirectory 'rightImgNeedleThin_test00' int2str(i)  '.tiff'], 'tiff');
        
    [leftCentersMatched, rightCentersMatched,leftCorrespondanceMetric,rightCorrespondanceMetric] = correspondance(stereo,leftCenters,rightCenters,(leftBack-leftImg),(rightBack-rightImg),index);
    
    figure;
    subplot(2,1,1)
    imshow(leftImg);
    hold on
    scatter(leftCenters(2,:),leftCenters(1,:),'.w');
    hold on 
    scatter(leftCentersMatched(1,:),leftCentersMatched(2,:),'.y')
    hold off
    subplot(2,1,2)
    %figure
    imshow(rightImg);
    hold on
    scatter(rightCenters(2,:),rightCenters(1,:),'.w');
    hold on 
    scatter(rightCentersMatched(1,:),rightCentersMatched(2,:),'.y')
    hold off
    title(imagePrefix)
    uiwait
    
    needleErr = cell(1,2);
    needle = cell(1,2);
    [needleErrLeft, needleLeft, needle2DprojLeft] = reconstruct3D(stereo,[leftCentersMatched(2,:)' leftCentersMatched(1,:)'], rightCenters');
    [needleErrRight, needleRight, needle2DprojRight] = reconstruct3D(stereo,leftCenters', [rightCentersMatched(2,:)' rightCentersMatched(1,:)']);

    needleErr{1} = needleErrLeft;
    needleErr{2} = needleErrRight;
    needle{1} = needleLeft;
    needle{2} = needleRight;
    needle2Dproj = {needle2DprojLeft{1} needle2DprojLeft{2}; needle2DprojRight{1} needle2DprojRight{2}};

    
    figure
    plot3(needle{1}(:,1),needle{1}(:,2),needle{1}(:,3),'.r');
    hold on 
    plot3(needle{2}(:,1),needle{2}(:,2),needle{2}(:,3),'.b');
    axis equal
    title(imagePrefix)
    uiwait
    
    figure
    subplot(1,2,1)
    imshow(leftImg)
    hold on
    plot(needle2Dproj{1,1}(:,2),needle2Dproj{1,1}(:,1),'.r');
    hold on
    plot(needle2Dproj{2,1}(:,2),needle2Dproj{2,1}(:,1),'.b');
    subplot(1,2,2)
    imshow(rightImg)
    hold on
    plot(needle2Dproj{1,2}(:,2),needle2Dproj{1,2}(:,1),'.r');
    hold on
    plot(needle2Dproj{2,2}(:,2),needle2Dproj{2,2}(:,1),'.b')
    hold off
    title(imagePrefix)
    uiwait
   
    save([reconstructionDirectory 'needleErr' int2str(i) '.mat'], 'needleErr');   
    save([reconstructionDirectory 'needle' int2str(i) '.mat'], 'needle');   
    save([reconstructionDirectory 'needle2Dproj' int2str(i) '.mat'], 'needle2Dproj');   

end


end


function [leftCentersMatched, rightCentersMatched,leftCorrespondanceMetric,rightCorrespondanceMetric] = correspondance(stereo,leftCenters,rightCenters,leftImg,rightImg,index)
[rightCentersMatched, rightCorrespondanceMetric] = find_corres_pix2  (leftCenters, 'left',rightImg, 'right', stereo, [], 'false',index);
[leftCentersMatched, leftCorrespondanceMetric] = find_corres_pix2  (rightCenters, 'right',leftImg, 'left', stereo, [], 'false',index);


end


function [needleErr, needle, needle2Dproj] = reconstruct3D(stereo, leftCentersMatched, rightCentersMatched)

    graphics = false;
    clear needle 

    %......................................................................
    % DEFINE PARAMETERS FOR TRIANGUALTION
    %......................................................................
    camera1.ratio = [0.45; 0.45];               
    camera1.origin = stereo.left.cc';
    camera1.focal = -1*stereo.left.fc(1);

    camera2.ratio = [0.45; 0.45];
    camera2.origin = stereo.right.cc';
    camera2.focal = -1*stereo.right.fc(1);

    cameras{1} = camera1;
    cameras{2} = camera2;

    poses{1} = stereo.G_world2left;
    poses{2} = stereo.G_world2right; 

    centers{1} = leftCentersMatched;
    centers{2} = rightCentersMatched;
    % reformating camera parameters
    for i = 1:2
          ex(i,1) = cameras{i}.ratio(1);
          ey(i,1) = cameras{i}.ratio(2);
          xo(i,1) = cameras{i}.origin(1);
          yo(i,1) = cameras{i}.origin(2);
          focal(i,1) = cameras{i}.focal;
    end
    
    [needleErr, needle, needle2Dproj] = calculate_bpe_final(centers,poses,ex,ey,xo,yo,focal,2);

end