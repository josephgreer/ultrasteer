function SegmentationTest_YO(Image_Filename)
    resRatioToNTSC = 1;
    % Open image.
%    figure(1);
    Img = imread( Image_Filename );
    figure(1);
    imshow(Img);
    
    Ith = imtophat(Img, strel('disk', ceil(1 * resRatioToNTSC)));
    Ithbin = Ith > 3;
    Img2 = bwareaopen(Ithbin, round(15 * resRatioToNTSC));
    figure(2);
    imshow(Ith);

    %since the seeds are dark in the image, we need to invert the image
    Ineg = imcomplement(Img);

    %convert uint into double
    InegD = double(Ineg);

    % a) perform opening with 3x3 square
    Iop = imopen(Ineg, strel('square', round(3 * resRatioToNTSC)));
    % Iop = imopen(Ineg, strel('disk', 3, 8));
    Iop2 = imopen(Ineg, strel('ball', round(3 * resRatioToNTSC), round(1 * resRatioToNTSC)));

    % b) subtract from the original image to get the final gray scale image
    iThreshold = 22;
    Iopth = imsubtract(Ineg, Iop);
    Iopth2 = imsubtract (Ineg, Iop2);
    Idiff = imsubtract(Iopth2, Iopth);
    Idiffbin = Idiff > iThreshold;
    Idiffbin = bwareaopen(Idiffbin, 8);
    Idiffbin = imclearborder(Idiffbin);
    figure(3);
    imshow(Idiffbin);
end