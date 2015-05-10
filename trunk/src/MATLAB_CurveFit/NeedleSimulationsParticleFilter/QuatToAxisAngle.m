function v = QuatToAxisAngle(q)
assert(sum(size(q) == [4,1]) == 2);
if(q(1) == 1)
    v = [0;0;0];
    return;
end
alpha = 2*acos(q(1));
v = q(2:4)/sqrt(1-q(1)^2);
v = v*alpha;
end