function phi = lengthsToPhiNonUniform(l,psis)
a = l(2)-l(1);
b = cos(psis(1))-cos(psis(2));
c = sin(psis(1))-sin(psis(2));
d = l(3)-l(2);
e = cos(psis(2))-cos(psis(3));
f = sin(psis(2))-sin(psis(3));
phi = atan2(d*b-a*e,a*f-d*c);
end