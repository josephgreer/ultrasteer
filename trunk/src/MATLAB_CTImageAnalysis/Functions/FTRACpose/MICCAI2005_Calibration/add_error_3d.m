%Function to add an error of magnitude 'err' to any set of 2D points. A
%direction is chosen randomly for each point and 'err' added in that chosed
%direction.
function [ptsn] = add_error_3d(pts, err)

num = size(pts, 1);

th = 2*pi*rand(num, 1);
phi = pi*(rand(num, 1)*2 -1);
dis = err.*rand(num, 1);
ptsn = pts + [3.243*dis 3.083*dis dis].*[cos(phi).*cos(th) cos(phi).*sin(th) sin(phi)];