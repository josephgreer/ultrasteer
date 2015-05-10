function R = QuatToRotationMatrix(q)
assert(sum(size(q) == [4,1]) == 2);
v = QuatToAxisAngle(q);
alpha = norm(v);
if(alpha == 0)
    R = eye(3);
    return;
end
v = v/norm(v);

ca = cos(alpha);  sa = sin(alpha);
R = ca*eye(3)+sa*[0 -v(3) v(2); v(3) 0 -v(1); -v(2) v(1) 0]+(1-ca)*(v*v');
end