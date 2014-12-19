function q = AxisAngleToQuat(v)
alpha = norm(v);
if(alpha == 0)
    q = [1; 0; 0; 0];
    return;
end
e = v/alpha;

q = [cos(alpha/2); e*sin(alpha/2)];
end