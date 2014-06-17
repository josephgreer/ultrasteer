function dd=distance(x,y,y0)

a=dot(x,x);
b=-2*dot(x,y0);
c=2*dot(x,y);
d=2*dot(y,y0);
e=dot(y,y);
f=dot(y0,y0);

dd=(b*c*d+b^2*e+c^2*f+a*(d^2-4*e*f))/(c^2-4*a*e);
