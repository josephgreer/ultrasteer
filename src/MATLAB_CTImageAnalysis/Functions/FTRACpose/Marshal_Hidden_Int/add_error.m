%Function to add an error of magnitude 'err' to any set of 2D points. A
%direction is chosen randomly for each point and 'err' added in that chosed
%direction.
function ptsn = add_error(pts, err);

num = size(pts, 1);

th = 2*pi*rand(num, 1);
dis = err.*rand(num, 1);
ptsn = pts + [dis*3.234 dis*3.083].*[cos(th) sin(th)];
ptsn