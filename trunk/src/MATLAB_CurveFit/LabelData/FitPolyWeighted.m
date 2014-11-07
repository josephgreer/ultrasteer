%%%% Expected input is 
%%%% pts = 
%%%% [x1 y1 z1
%%%%  x2 y2 z2
%%%% ...
%%%%  xn yn zn]

%%% degree = degree of polynomial to fit
%%% descriptor variable = unit vector representing axis of descriptor
%%% [x;y;z];
%%% polynomial described as poly(1)x^degree+...+poly(degree)*x+poly(degree+1)

function poly = FitPolyWeighted(pts, weights, degree, descriptor)

tvals = TvalsFromDescriptor(pts, descriptor);
Ain = [];
for i=degree:-1:0
    Ain = [Ain tvals.^i];
end
poly = [transpose(lscov(Ain, pts(:,1)), weights); lscov(robustfit(Ain, pts(:,2)),weights); transpose(lscov(Ain, pts(:,3)),weights)];
end