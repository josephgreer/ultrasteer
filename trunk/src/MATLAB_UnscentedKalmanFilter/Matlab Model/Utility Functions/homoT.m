function [T] = homoT(R,p)
%HOMOT Create homogeneous transform matrix (4x4) from position vector (3x1)
%and rotation matrix (R)
T = [R p; 0 0 0 1];
end

