function e0 = GetE0(L0,R0,P,a,mode)
    f = @(x)(ContractionToForce(x,L0,R0,P,a,mode));
    e0 = fsolve(f, 0.3);
end