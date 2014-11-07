function [ T ] = state2trans( x )
%STATE2TRANS Reads state vector [x y z a b g]' and returns homogeneous
%transform

% Quaternion
q = rot2quat(x(4:6));

% Direction cosine matrix
R = quat2dcm(q);
   
% Combine into transformation matrix
T = [ R'         x(1:3); 
      zeros(1,3) 1; ];
end

