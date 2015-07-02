clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

nTests = 20;
xs = [];
answers = [];
for i=1:nTests
    x = rand*2-1;
    xs = vertcat(xs, x);
    answers = vertcat(answers, erf(x));
end

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