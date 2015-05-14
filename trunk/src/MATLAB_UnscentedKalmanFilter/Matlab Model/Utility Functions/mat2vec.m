function [r] = mat2vec(R)
%MAT2VEC Convert rotation matrix (3x3) into axis-angle vector (3x1)
w = vrrotmat2vec(R);
r = w(1:3)'.*w(4);
end
