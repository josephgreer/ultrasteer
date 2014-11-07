tvals = TvalsFromDescriptor(mps(:,[1:3]), descriptor);

nframes = max(mps(:,7));
angles = [];

deriv = [];
for i=degree:-1:1
    deriv = [deriv i*model(:,degree-i+1)];
end

dderiv = [];
for i=degree-1:-1:1
    dderiv = [dderiv i*deriv(:,degree-i)];
end

tv = [min(tvals):.1:max(tvals)]';


rhos = [];
for i=1:size(tv,1)
    
    
    tt = tv(i).^([degree:-1:0]');
    
    ttt = tt([2:size(tt,1)]);
    tttt = tt([3:size(tt,1)]);
    
    
    dt = deriv*ttt;
    ddt = dderiv*tttt;
    rho = norm(dt)^3/sqrt(((norm(dt)^2)*(norm(ddt)^2)-((dt'*ddt)^2)));
    
    rhos = [rhos; rho];
end