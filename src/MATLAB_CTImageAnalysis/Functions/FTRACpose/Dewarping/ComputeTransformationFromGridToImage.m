% $Id: ComputeTransformationFromGridToImage.m,v 1.4 2007/07/18 20:27:11 anton Exp $
function RGridToImage = ComputeTransformationFromGridToImage(im, Origin, pixelSize, resRatioToNTSC)
% function RGridToImage = ComputeTransformationFromGridToImage(im, Origin)
% finds the orientation of the physical grid w.r.t the image.

invIm = imcomplement(im);
% perform the morphological top hat operation
% a) perform opening with 3x3 square
% Iop = imopen(Ineg, strel([1 1 1; 1 1 1; 1 1 1]));
% a) perform opening with 5x5 rectangle
SE = strel('disk', round(7 * resRatioToNTSC));
Iop = imopen(invIm, SE);

% Iop = imopen(Ineg, strel([0 0 1 1 1; 0 0 1 1 1; 1 1 1 1 1; 1 1 1 0 0;1 1 1 0 0]));
% b) subtract from the original image to get the final gray scale image
Iopth = imsubtract(invIm, Iop);
%figure, imshow(Iopth);

% threshold the image & convert into binary
% disp(' thresholding the image');
threshIm = Iopth > 0.1;
threshIm = imclearborder(threshIm);

% remove the calibration lines from the image
resRatioSquare = round(resRatioToNTSC * resRatioToNTSC); % to compare surfaces
labelIm = bwlabel(threshIm);
for i = 1:max(max(labelIm))
    c = length(find(labelIm ==i));
    if (c>100 * resRatioSquare)
        labelIm(labelIm==i) =0;
    end
    if (c < 15 * resRatioSquare)
        labelIm(labelIm == i) = 0;
    end
end
% find the axis and origin of the BB plate
labelIm = bwlabel(labelIm);
% disp(' finding the orign and axis of the BB plate');
axisIm = labelIm;
stats = regionprops(axisIm, 'area');
c = cat(1,stats.Area);
[numbins, binpos] = hist(c,2);
bbthresh = binpos(2);
for i = 1:max(max(axisIm))
    if (c(i)<bbthresh)
        axisIm(axisIm==i) =0;
    end
end

axisIm = bwlabel(axisIm);

% find the origin - center of the BB closest to the image center
% disp(' finding the BB centers');
method = 2;
axisCenters = CalculateCentroid(im, axisIm, method);
% separate the x-axis and y-axis
numPtsOnAxis = size(axisCenters, 1);
centersInMM = axisCenters .* repmat(pixelSize, [numPtsOnAxis, 1]);
originInMM = Origin .* pixelSize;
delta = centersInMM - repmat(originInMM, numPtsOnAxis, 1);
delta = delta.*delta; delta = delta(:,1) + delta(:,2); delta = sqrt(delta);
% axisCenters = axisCenters(abs(delta(:,1))>10, :);
% delta = delta(abs(delta(:,1))>10,:);
% dist = sqrt(delta(:,1).^2 + delta(:,2).^2);
ptOnXaxis = axisCenters(delta > 5  & delta < 15, :);
d = ptOnXaxis - Origin;
angle = atan(d(2)/d(1));
angleInDegrees = angle * 180/pi;
if (d(1) < 0)
    RGridToImage = 180 - angleInDegrees;
end
% RGridToImage = [cos(angleInDegrees * pi/180) sin(angleInDegrees * pi/180) 0; -sin(angleInDegrees*pi/180), cos(angleInDegrees * pi/180), 0; 0 0 1];
if (d(1) >= 0 )
    RGridToImage = -angleInDegrees;
end
% RGridToImage = angleInDegrees;

%$Log: ComputeTransformationFromGridToImage.m,v $
%Revision 1.4  2007/07/18 20:27:11  anton
%Dewarping: Added resolution ratio to NTSC to handle different resolutions.
%All sizes (for segmentation) remain in pixels for NTSC resolution as this has
%been our reference so far (i.e. about 9 inch diameter for 480 pixels).
%This should ultimately be modified to define all sizes in mm and use a more
%intuitive definition of the resolution.
%
%Revision 1.3  2007/02/09 16:15:58  anton
%ComputeTransformationFromGridToImage.m: Added histogram to find size of
%large BBs
%
%Revision 1.2  2006/09/27 15:55:12  gouthami
%ComputeTransformationFromGridToImage.m: Takes pixelsize as argument, modified the center positions calculation to handle all orientations
%of the calibration phantom
%
%Revision 1.1  2006/07/15 18:05:33  gouthami
%Adding to CVS
%
