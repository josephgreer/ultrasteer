%$Id: fit_ellipse.m,v 1.2 2005/12/12 22:49:44 jain Exp $
%Fit a least square ellipse equation to the points supplied from the image.
function el_eq = fit_ellipse(pts)

x = pts(:,1);
y = pts(:,2);
A = [x.*x x.*y y.*y x y];
n = cond(A);
a = mldivide(A, ones(size(x,1), 1));
el_eq = [a ; -1]';

%$Log: fit_ellipse.m,v $
%Revision 1.2  2005/12/12 22:49:44  jain
%Just testing. No change from original.
%