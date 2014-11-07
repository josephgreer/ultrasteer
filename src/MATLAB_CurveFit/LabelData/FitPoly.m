%%%% Expected input is 
%%%% pts = 
%%%% [x1 y1 z1
%%%%  x2 y2 z2
%%%% ...
%%%%  xn yn zn]

%%% degree = degree of polynomial to fit
%%% descriptor [x;y;z] unit vector of axis x0 offset
%%% polynomial described as poly(1)x^degree+...+poly(degree)*x+poly(degree+1)

function poly = FitPoly(pts, degree, descriptor)

tvals = TvalsFromDescriptor(pts, descriptor);
Ain = [];
for i=degree:-1:0
    Ain = [Ain tvals.^i];
end
poly = [transpose(lscov(Ain, pts(:,1))); transpose(lscov(Ain, pts(:,2))); transpose(lscov(Ain, pts(:,3)))];
end