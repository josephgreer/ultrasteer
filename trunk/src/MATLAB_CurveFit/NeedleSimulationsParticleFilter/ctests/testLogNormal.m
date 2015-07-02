clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

nTests = 20;
mus = [];
sigmas = [];
xs = [];
answers = [];
for i=1:nTests
    mu = rand*5;
    sigma = abs(rand*10);
    x = exp(rand*2-1+mu);
    
    mus = vertcat(mus,mu);
    sigmas = vertcat(sigmas,sigma);
    xs = vertcat(xs, x);
    answers = vertcat(answers, lognpdf(x,mu,sigma));
end

str = 'f64 mu[] = {';
for i=1:nTests
    str = strcat(str, sprintf('%.8f, ', mus(i)));
end
str = strcat(str, '};');
display(str);

str = 'f64 sigma[] = {';
for i=1:nTests
    str = strcat(str, sprintf('%.8f, ', sigmas(i)));
end
str = strcat(str, '};');
display(str);

str = 'f64 x[] = {';
for i=1:nTests
    str = strcat(str, sprintf('%.8f, ', xs(i)));
end
str = strcat(str, '};');
display(str);

str = 'f64 answer[] = {';
for i=1:nTests
    str = strcat(str, sprintf('%.8f, ', answers(i)));
end
str = strcat(str, '};');
display(str);