clear; clc; close all;

log = load('log'); log = log.log;
log_pinv = load('log_pinv'); log_pinv = log_pinv.log;

plot(log.errors(:,2));
hold on;
plot(log_pinv.errors(:,2));