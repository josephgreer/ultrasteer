function model = FitModel(samples, weights)
A = [samples(:,1).^3 samples(:,1).^2 samples(:, 1).^1, samples(:, 1).^0];
bx = samples(:,2);
by = samples(:,3);
w = max(weights,0);
X = lscov(A, bx, w);
Y = lscov(A, by, w);
model = [X Y];
end