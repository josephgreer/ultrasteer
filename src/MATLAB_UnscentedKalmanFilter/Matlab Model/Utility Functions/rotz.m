function [ T ] = rotz( gamma )
%ROTZ return the homogeneous transformation matrix for a rotation of gamma
%about the z-axis.

T = homoT(Rz(gamma),zeros(3,1));

end

