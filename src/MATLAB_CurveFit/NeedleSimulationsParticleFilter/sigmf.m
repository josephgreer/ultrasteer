% x = [x1 x2 ... xn]
% x_i = feature vect in R^n
% params = [B0 B1 ... B_{n}]

function y = sigmf(x, params)
if(size(params,2) == 1)
    params = params';
end
x = [ones(1,size(x,2));x];
y = 1./(1+exp(-params*x));
end