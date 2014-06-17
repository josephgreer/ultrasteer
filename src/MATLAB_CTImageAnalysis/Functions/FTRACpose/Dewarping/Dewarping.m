%$Id: Dewarping.m,v 1.2 2006/08/14 20:26:08 anton Exp $
% script to compute the dewarping function

%read the image on to a matrix buffer
disp('Reading the input image');
im = imread('img-0001.tiff');
[rows,cols] = size(im);
% process the image
[labelIm, Origin] = ProcessImage(im, 60, 1);
im = im2double(im);

% calculate the centers - 
method = 2; % method can take values from 1-4 
centers = CalculateCentroid(im, labelIm, method);
% find the orientation of the grid
disp('Finding the orientation of the grid');
entropy = FindEntropy(centers, 0);
index = 1;
for i = -90:90
    entropy(index, 1) = i;
    entropy(index,2) = FindEntropy(centers, i);
    index = index + 1;
end
% find the orientation for which the entropy is minimum
orientation = entropy(find(entropy(:,2) == min(entropy(:,2))), 1) ;
% change theta from degrees to radians
theta = orientation *pi/180;

% find correspondences between the segmented points and the physical
% grid points
disp('Computing the correspondences');
[segPts, gridPts] = FindCorrespondences(centers, Origin, theta);

% compute the polynomial coefficients
disp('Computing the polynomial' );
PolyOrder = 5;
normalizedGridPts = NormalizeToImageBox(gridPts, rows, cols);
PolyCoeff = ComputeBernsteinCoefficients(segPts, normalizedGridPts, PolyOrder);

% undistort/dewarp the image
disp('Dewarping the image');
dewarpIm = dewarpimage(im, PolyCoeff, PolyOrder);

%$Log: Dewarping.m,v $
%Revision 1.2  2006/08/14 20:26:08  anton
%Dewarping: Match cases for algorithms (to avoid warnings on recent Matlab).
%
%Revision 1.1  2005/12/07 22:37:38  gouthami
%Adding the file to CVS for the first time
%