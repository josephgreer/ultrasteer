function [R] = vec2mat(r)
%VEC2MAT Convert axis-angle vector (3x1) into rotation matrix (3x3)
if( norm(r) == 0 )
    R = eye(3);
else
    w = [r'./norm(r) norm(r)];
    R = vrrotvec2mat(w);
end
end


