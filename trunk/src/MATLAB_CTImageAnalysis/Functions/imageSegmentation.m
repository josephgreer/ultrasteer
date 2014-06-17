function [] = imageSegmentation(startIDX,imgDirectory,segmentationDirectory)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

for i=startIDX:length(dir([imgDirectory '/85/test00*_back.tiff']))
    close all
    imagePrefix = ['test00' int2str(i)];
    
    %......................................................................
    % LOAD IMAGES AND STEREO DATA
    %......................................................................
   
    leftImg = im2double(imread([imgDirectory '85/' imagePrefix '.tiff']));
    rightImg = im2double(imread([imgDirectory '95/' imagePrefix '.tiff']));
    leftBack = im2double(imread([imgDirectory '85/' imagePrefix '_back.tiff']));
    rightBack = im2double(imread([imgDirectory '95/' imagePrefix '_back.tiff']));
    
    centers = cell(1,2);
        
    [centers{1}, centers{2},leftImgNeedle,rightImgNeedle] = centerSegmentation(leftImg,rightImg,leftBack,rightBack); % centers{1} = left centers, centers{2} = right centers
    close all
    figure;
    subplot(1,2,1)
    imshow(leftImg);
    hold on
    scatter(centers{1}(2,:),centers{1}(1,:),'.w');
    hold off
    subplot(1,2,2)
    imshow(rightImg);
    hold on
    scatter(centers{2}(2,:),centers{2}(1,:),'.w');
    %subplot(2,2,3)
    %imshow(leftImgNeedle);
    %hold on
    %subplot(2,2,4)
    %imshow(rightImgNeedle);
    %hold on
   
    hold off
    uiwait
   
    save([segmentationDirectory 'centers' int2str(i) '.mat'], 'centers');  
    imwrite(leftImgNeedle,[segmentationDirectory 'leftImgNeedleThin_test00' int2str(i)  '.tiff'], 'tiff');
    imwrite(rightImgNeedle,[segmentationDirectory 'rightImgNeedleThin_test00' int2str(i)  '.tiff'], 'tiff');
end

end

