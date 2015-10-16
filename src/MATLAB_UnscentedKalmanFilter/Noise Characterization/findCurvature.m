function [ r,c,circ_pts, planar_err ] = findcurvature(pts,k)
%FINDCURVATURE fit a 3D circle to N x 3 matrix of points

% translate the data to an origin
m = mean(pts);
pts_m = pts - repmat(m,length(pts),1);
% calculate the covariance matrix
N = size(pts,1);
covar = pts_m'*pts_m/N;
% find the rotation matrix
[U,S,V] = svd(covar);
% reduce the data
reduced_pts = pts_m*V(:, 1:2);
planar_err = (pts_m*V(:, 3))';
% fit the circle
[z,r] = fitcircle(reduced_pts);
% calculate k points on the 2D circle
a = reduced_pts(1,:);
b = reduced_pts(end,:);
% find the angles at the beginning and end points
c = a'-z;
theta_a = wrapTo360( atan2d(c(2),c(1)) );
c = b'-z;
theta_b = wrapTo360( atan2d(c(2),c(1)) );

% find circle points along the curve
theta = linspace(theta_a,theta_b,k)';
circ_x = z(1) + r*cosd(theta);
circ_y = z(2) + r*sind(theta);
% project back in to 3D
circ_pts = [circ_x circ_y zeros(k,1)]*inv(V)+ repmat(m,k,1);
c = [z' 0]*inv(V)+m;

% figure(3); clf;
% plot(reduced_pts(:,1),reduced_pts(:,2));

end

