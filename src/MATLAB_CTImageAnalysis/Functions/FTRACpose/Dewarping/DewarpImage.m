% $Id: DewarpImage.m,v 1.4 2007/07/19 23:16:48 anton Exp $
function [dewarpIm, Xmapped, Ymapped] = DewarpImage(im, PolyOrder, PolyCoeff)
% function dewarpIm = DewarpImage(im, PolyCoeff, PolyOrder)
% DewarpImage dewarps the image im by fitting in a bernstein polynomial of
% PolyOrder with given coefficients PolyCoeff


% create an array of image pixels
[rows, cols] = size(im);
[X, Y] = meshgrid(1:cols, 1:rows);
X = reshape(X, [rows*cols,1]);
Y = reshape(Y, [rows*cols, 1]);
imgPts = [Y, X]; 

% normalize the image pixels
imgPts = NormalizeToImageBox(imgPts, rows, cols);
Bezier = ComputeBezierTerms(imgPts, PolyOrder);
clear imgPts;
% undistort the image pixels
corrImgPts = Bezier * PolyCoeff;

% interpolate the image to get the intensity values at the corrected image
% points to get the undistorted image
XI = reshape(corrImgPts(:,1), [rows, cols]);
YI = reshape(corrImgPts(:,2), [rows, cols]);
clear corrImgPts;

% zero the distortion at the image center rather than the center BB
imgCenter = [rows, cols]/2;
normalizedImgCenter = NormalizeToImageBox(imgCenter, rows, cols);
Bezier = ComputeBezierTerms(normalizedImgCenter, PolyOrder);
corrImgCenter = Bezier * PolyCoeff;
dispAtCenter = corrImgCenter - imgCenter;
Xnew = XI + dispAtCenter(1);
Ynew = YI + dispAtCenter(2);

clear XI;
clear YI;

if (nargout > 1)
    Xmapped = Xnew;
    Ymapped = Ynew;
end
dewarpIm = interp2(im, Ynew, Xnew);

% eliminate NaN values introduced by interpolation
dewarpIm(isnan(dewarpIm)) = 0;

% $Log: DewarpImage.m,v $
% Revision 1.4  2007/07/19 23:16:48  anton
% DewarpImage.m: Added a couple of clear to free memory
%
% Revision 1.3  2007/07/18 19:51:53  anton
% ComputeImage.m:  added clear when possible to reduce memory footprint
%
% Revision 1.2  2006/03/07 20:39:53  gouthami
% Changed the input parameter order from (im, PolyCoeff, PolyOrder) to (im, PolyOrder, PolyCoeff),
% and also added the code to make the image center to have zero distortion.
%
% Revision 1.1  2005/12/07 22:37:38  gouthami
% Adding the file to CVS for the first time
%

