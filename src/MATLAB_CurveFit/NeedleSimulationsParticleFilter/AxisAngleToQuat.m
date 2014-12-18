function q = AxisAngleToQuat(v)
alpha = norm(v);
e = v/alpha;

q = [cos(alpha/2); e*sin(alpha/2)];
end