function w = quatrot(q,v)
w = quatmult(quatmult(q,[0; v]),quatinverse(q));
assert(abs(w(1)) < 1e-6);
w = w(2:4);
end