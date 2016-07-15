function phi = lengthsToPhi(l)
assert(length(l) == 3);
phi = atan2(sqrt(3)*(l(2)+l(3)-2*l(1)),(3*(l(2)-l(3))));
end