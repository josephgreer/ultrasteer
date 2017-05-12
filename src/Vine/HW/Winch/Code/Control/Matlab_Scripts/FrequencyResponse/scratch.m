clear; clc; close all;

s = tf('s');

a = 2;

input = s/(s^2+a^2);
input2 = a/(s^2+a^2);
system = 1/(s+a);

impulse(input*system);
hold on;
impulse(input);

figure;
impulse(input2*system);
hold on;
impulse(input2);

figure;
ts = linspace(0,10,1000);
y = lsim(system, sin(a*ts), ts);
plot(ts,y);
hold on;
plot(ts,sin(a*ts));

