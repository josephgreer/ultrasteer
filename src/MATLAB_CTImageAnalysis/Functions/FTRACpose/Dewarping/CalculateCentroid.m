% $Id: CalculateCentroid.m,v 1.1 2005/12/07 22:37:38 gouthami Exp $

function centers = CalculateCentroid(im, labelIm, method)
% CalculateCentroid(im, labelIm, method) finds the approximate physical centers of the
% BB's in the image. Current implementation supports four different ways of
% extracting the BB centers.
% im - input gray scale image
% labelIm -  binary image with labelled BB's
% method - select the method to compute centroids, takes on values 1, 2, 3
%           1 - calculates centroids by interpolating values within a
%           bounding box of the bb's and takes the minimum intensity value
%           as the centroid.
%           2 - calculates centroids using Matlab's Centroid calculation.
%           This is based on geometry, does not consider intensities
%           3 - calculates centroids using weighted intensity metric.
%           4 - calculates centroids by projecting the bb's onto a
%           horizontal axis and fits a curve to the projected intensities

[rows, cols] = size(im);
numBBs = max(max(labelIm));
centers = zeros(numBBs, 2);
switch method
    case 1
        stats = regionprops(labelIm, 'BoundingBox');
        BoundingBox = cat(1,stats.BoundingBox);
        for i = 1:size(stats,1)
            boundingbox = BoundingBox(i,:);
            ul_corner = round([boundingbox(1), boundingbox(2)]);
            widthxy = round(boundingbox(3:4));
            width = max(widthxy);
            lr_corner = ul_corner+width; 
            interpWindowUl_y = min(rows, max(1, ul_corner(2)-width));
            interpWindowLr_y = max(1, min(rows, lr_corner(2)+width));
            interpWindowUl_x = min(cols, max(1, ul_corner(1)-width));
            interpWindowLr_x = max(1, min(cols, lr_corner(1)+width));
            range_X = interpWindowLr_x - interpWindowUl_x;
            range_Y = interpWindowLr_y - interpWindowUl_y;
            if (range_X < 2 || range_Y <2)
                continue;
            end
            
            [XI, YI] = meshgrid([ul_corner(1): 0.25: lr_corner(1)], [ul_corner(2): 0.25: lr_corner(2)]);
            XI = reshape(XI, [size(XI,1)*size(XI,2), 1]);
            XI = XI - interpWindowUl_x + 1;
            YI = reshape(YI, [size(YI,1)*size(YI,2), 1]);
            YI = YI - interpWindowUl_y + 1;
            ZI = interp2(im(interpWindowUl_y:interpWindowLr_y, interpWindowUl_x: interpWindowLr_x), XI, YI);
            minIntensityIndex = find(ZI == min(ZI));
            count = length(minIntensityIndex);
            centers(i,:) = [YI(minIntensityIndex(1)) + interpWindowUl_y - 1, XI(minIntensityIndex(1)) + interpWindowUl_x - 1];
            if (count > 1)
                lim = zeros(size(im));
                [py,px] = find(im(interpWindowUl_y:interpWindowLr_y, interpWindowUl_x: interpWindowLr_x) == min(ZI));
                py = py + interpWindowUl_y -1;
                px = px +interpWindowUl_x-1;
                for j = 1:size(px,1)
                    lim(px(j), py(j)) = 1;
                end
                sts = regionprops(lim, 'Centroid');
                centers(i,:) = [sts(1).Centroid(1), sts(1).Centroid(2)];
            end
        end
    case 2
        stats = regionprops(labelIm, 'Centroid');
        c = cat(1,stats.Centroid);
        centers = [c(:,2), c(:,1)];
    case 3
        for i= 1:max(max(labelIm))
            [r,c] = find(labelIm==i);
            avgIntensity = 0;
            for index =1:size(c)
                intensity = double(im(r(index), c(index)));
                avgIntensity = avgIntensity + intensity;
                centers(i,1) = centers(i,1) + r(index)*intensity;
                centers(i,2) = centers(i,2) + c(index)*intensity;
            end
            centers(i,:) = centers(i,:) / avgIntensity;
        end
    case 4
        for i= 1:max(max(labelIm))
            [r,c] = find(labelIm==i);
            bb = [r,c];
            xindices = [min(r):1:max(r)];
            yindices = [min(c):1:max(c)];
            clear xintensity;
            for index =1:size(xindices,2)
                xintensity(index) = sum(double(im(xindices(index), yindices) .*labelIm(xindices(index), yindices)));
            end
            clear yintensity;
            for index = 1:size(yindices,2)
                yintensity(index) = sum(double(im(xindices, yindices(index)) .* labelIm(xindices, yindices(index))));
            end
            xPoly = polyfit(xindices, xintensity, 2);
            syms x ;
            fx = xPoly(1)*x^2+ xPoly(2)*x+xPoly(3);
            dfx = diff(fx);
            centers(i,1) = double(solve(dfx));
            syms y;
            yPoly = polyfit(yindices, yintensity, 2);
            fy = yPoly(1)*y^2+yPoly(2)*y+yPoly(3);
            dfy = diff(fy);
            centers(i,2) = double(solve(dfy));
            
%             center(i,1) = center(i,1) + r(index)*intensity;
%             center(i,2) = center(i,2) + c(index)*intensity;
%             center(i,:) = center(i,:) / avgIntensity;
        end
    otherwise
        disp('Select appropriate method value from 1 - 4 ');
end

% $Log: CalculateCentroid.m,v $
% Revision 1.1  2005/12/07 22:37:38  gouthami
% Adding the file to CVS for the first time
%