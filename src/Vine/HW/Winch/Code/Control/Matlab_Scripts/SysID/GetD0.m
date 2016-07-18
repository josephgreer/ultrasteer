function D0 = GetD0(L0,R0,P,a,mode)
    f = @(x)(ContractionToForce(x,L0,R0,P,a,mode));
    e = fsolve(f, 0.3);
    D0 = L0*(1-e);
end