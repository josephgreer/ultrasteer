%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This function returns the pdf of a truncated gaussian
% x = nx1 vector to look up
% mu = nx1 vector representing the mean of the truncated gaussian
% sigma = nx1 standard deviation vector. (all correlations zero)
% a = nx1 to truncate distribution to (lower limit)
% b = nx1 to truncate distribution to (upper limit)
% return values
% density value 

function d = truncatedIndependentGaussianPdf(x, mu, sigma, a, b)
Sigma = diag(sigma.^2);
num = normpdf(x, mu, sigma);
den = (normcdf(b,mu,sigma)-normcdf(a, mu, sigma)); 
d = prod(num./den);
end