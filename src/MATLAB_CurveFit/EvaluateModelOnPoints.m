function errors =  EvaluateModelOnPoints(polyX, polyY, samples)
A = [samples(:,1).^3 samples(:,1).^2 samples(:, 1).^1, samples(:, 1).^0];
x = A*polyX;
y = A*polyY;

ex = x-samples(:,2);
ey = y-samples(:,3);
errors = ex.^2+ey.^2;
end