%%%% Expected input is 
%%%% pts = 
%%%% [x1 y1 z1
%%%%  x2 y2 z2
%%%% ...
%%%%  xn yn zn]

%%% degree = degree of polynomial to fit
%%% descriptor = unit vector representing descriptor axis [x;y;z];
%%% polynomial described as poly(1)x^degree+...+poly(degree)*x+poly(degree+1)

function poly = FitPolyRobust(pts, degree, descriptor)
tvals = TvalsFromDescriptor(pts, descriptor);
Ain = [];
for i=degree:-1:0
    Ain = [Ain tvals.^i];
end
error('poly robust aint working right now');
polyx = robustfit(Ain, pts(:,1));
polyy = robustfit(Ain, pts(:,2));
polyz = robustfit(Ain, pts(:,3));
poly = [transpose(polyx); transpose(polyy); transpose(polyz)];
end