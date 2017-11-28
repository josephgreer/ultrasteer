function values = CheckAngleBetween(a0, a1, thetas)
deltas0 = mod((thetas-a0+3*pi),2*pi)-pi;
deltas1 = mod((thetas-a1+3*pi),2*pi)-pi;

delta01 = mod(a1-a0+3*pi,2*pi)-pi;

values = (sign(deltas0) ~= sign(deltas1)) & (sign(delta01) == sign(deltas0));
end