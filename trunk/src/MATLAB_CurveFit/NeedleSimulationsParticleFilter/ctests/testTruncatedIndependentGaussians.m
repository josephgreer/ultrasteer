clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

nTests = 20;
mus = [];
sigmas = [];
as = [];
bs = [];
xs = [];
answers = [];
for i=1:nTests
    mu = rand(2,1)*5;
    sigma = abs(rand(2,1)*10);
    x = rand(2,1)*2-1+mu;
    a = rand(2,1)*8-7+mu;
    b = rand(2,1)*20+a;
    
    mus = horzcat(mus,mu);
    sigmas = horzcat(sigmas,sigma);
    xs = horzcat(xs, x);
    as = horzcat(as, a);
    bs = horzcat(bs, b);
    answers = horzcat(answers, truncatedIndependentGaussianPdf(x,mu,sigma,a,b));
end

str = sprintf('f64 mu[%d][%d] = {', nTests,2);
for i=1:nTests
    str = strcat(str, sprintf('{%.8f, %.8f}, ', mus(1, i), mus(2,i)));
end
str = strcat(str, '};');
display(str);

str = sprintf('f64 sigma[%d][%d] = {', nTests,2);
for i=1:nTests
    str = strcat(str, sprintf('{%.8f, %.8f}, ', sigmas(1, i), sigmas(2,i)));
end
str = strcat(str, '};');
display(str);

str = sprintf('f64 x[%d][%d] = {', nTests,2);
for i=1:nTests
    str = strcat(str, sprintf('{%.8f, %.8f}, ', xs(1, i), xs(2,i)));
end
str = strcat(str, '};');
display(str);

str = sprintf('f64 a[%d][%d] = {', nTests,2);
for i=1:nTests
    str = strcat(str, sprintf('{%.8f, %.8f}, ', as(1, i), as(2,i)));
end
str = strcat(str, '};');
display(str);

str = sprintf('f64 b[%d][%d] = {', nTests,2);
for i=1:nTests
    str = strcat(str, sprintf('{%.8f, %.8f}, ', bs(1, i), bs(2,i)));
end
str = strcat(str, '};');
display(str);


str = 'f64 answer[] = {';
for i=1:nTests
    str = strcat(str, sprintf('%.8f, ', answers(i)));
end
str = strcat(str, '};');
display(str);