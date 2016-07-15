clear; clc; close all;


fname = '2.txt';
wildcard = '2

for i=2:10
    copyfile(fname, sprintf(wildcard, i));
end