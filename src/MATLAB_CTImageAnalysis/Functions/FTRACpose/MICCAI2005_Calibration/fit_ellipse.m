%Fit a least square ellipse equation to the points supplied from the image.
function el_eq = fit_ellipse(pts)

x = pts(:,1);
y = pts(:,2);
A = [x.*x x.*y y.*y x y];
n = cond(A);
a = mldivide(A, ones(size(x,1), 1));
el_eq = [a ; -1]';
