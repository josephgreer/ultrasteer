function D0 = GetD0(L0,R0,P,mode)
if(mode == 'pleated')
    f = @(x)(ContractionToForce(x,L0,R0,P,mode));
    e = fsolve(f, 0.3);
    D0 = L0*(1-e);
elseif(mode == 'ellipse')
end
end