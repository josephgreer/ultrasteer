
%Returns the coordinates of the ellipses. 12 points on each ellipse are returned. 
%Input is the diameter of the shaft, resolution - points per mm, the angle of the 
%ellipse. Output are the points.
function [ellipse1_points, ellipse2_points] = ellipse_points(D, N, th)

%Draw the closer ellipse (on the left). To do so draw a vertical ellipse with the required
%major and minor axis. Rotate this ellipse along the x-axis by theta to get the ellipse 
%which we desire. Rotate it along some other axis also to get some other ellipse. The ellipse
%is drawn by drawing a half ellipse and flip it. The lengths are calculated by looking at the
%distance of this point from the neighbouring point.
tmp_ellp_pts = [];
for count = -pi/2 : (1/N)*(4*cos(th))/(D*(1+cos(th))) : pi/2
    x = (D/2)*(1/sqrt(1 + cos(th)*cos(th)*tan(count)*tan(count)));
    y = x*tan(count);
    tmp_ellp_pts = [tmp_ellp_pts ; x y];
end
tmp_ellp_pts = [tmp_ellp_pts ; -tmp_ellp_pts];
tmp_ellp_pts = [tmp_ellp_pts zeros(size(tmp_ellp_pts, 1), 1) ones(size(tmp_ellp_pts, 1), 1)];

xform = xyz_xform([-30*pi/180 0 0], [0 0 18]');
tmp_ellp_pts = (xform*tmp_ellp_pts')';
tmp_ellp_pts = tmp_ellp_pts(:,1:3)./(tmp_ellp_pts(:,4)*[1 1 1]);
diff_vects = tmp_ellp_pts - [tmp_ellp_pts(end, :) ; tmp_ellp_pts(1:end-1, :)];
weights = sqrt((sum((diff_vects.*diff_vects)'))');
tmp_ellp_pts = [tmp_ellp_pts weights];

ellipse1_points = tmp_ellp_pts(:,1:3);

%Draw the farther ellipse (on the right). The method is same as above...just rotate it by -theta.
tmp_ellp_pts = [];
for count = -pi/2 : (1/N)*(4*cos(th))/(D*(1+cos(th))) : pi/2
    x = (D/2)*(1/sqrt(1 + cos(th)*cos(th)*tan(count)*tan(count)));
    y = x*tan(count);
    tmp_ellp_pts = [tmp_ellp_pts ; x y];
end
tmp_ellp_pts = [tmp_ellp_pts ; -tmp_ellp_pts];
tmp_ellp_pts = [tmp_ellp_pts zeros(size(tmp_ellp_pts, 1), 1) ones(size(tmp_ellp_pts, 1), 1)];

xform = xyz_xform([30*pi/180 0 0], [0 0 41]');
tmp_ellp_pts = (xform*tmp_ellp_pts')';
tmp_ellp_pts = tmp_ellp_pts(:,1:3)./(tmp_ellp_pts(:,4)*[1 1 1]);
diff_vects = tmp_ellp_pts - [tmp_ellp_pts(end, :) ; tmp_ellp_pts(1:end-1, :)];
weights = sqrt((sum((diff_vects.*diff_vects)'))');
tmp_ellp_pts = [tmp_ellp_pts weights];

ellipse2_points = tmp_ellp_pts(:,1:3);
