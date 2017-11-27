% A,B = [a1; a2; ... an] \in R^{nx3}
function diffs = angleDiffSigns(A,B)
signs = cross(A,B,2);
signs = signs(:,3);

mags = dot(A,B,2);
mags = acos(mags);

diffs = signs.*mags;
end