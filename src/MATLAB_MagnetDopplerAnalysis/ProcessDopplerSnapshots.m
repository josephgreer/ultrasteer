clear; clc; close all;

%filename
baseDir = 'C:\';
files = dir(strcat(baseDir,'*.png'));


responses = [];
fnames = {};

for fi = 1:length(files)
    fname = files(fi).name;
    fnames{fi} = fname;
    im = imread(strcat(baseDir,fname));
    imshow(im);
    h = imrect;
    position = wait(h);
    position = round(position);
    im = im([position(2):position(2)+position(4)],[position(1):position(1)+position(3)],:);
    imR = im(:,:,1); imG = im(:,:,2); imB = im(:,:,3);
    trueMat = (abs(im(:,:,1)-im(:,:,2))<4 & abs(im(:,:,2)-im(:,:,3))<4 & abs(im(:,:,1)-im(:,:,3)<4));
    indices = find(trueMat > 0);
    imR(indices) = 0;
    imG(indices) = 0;
    imB(indices) = 0;
    im(:,:,1) = imR;  im(:,:,2) = imG; im(:,:,3) = imB;
    responses(fi) = sum(sum(imR+imG+imB))/3;
    imshow(im);
end

saveName = strcat(baseDir,'snapshots.mat');
clearvars -except responses fnames saveName
save(saveName);