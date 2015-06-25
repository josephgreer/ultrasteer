clear; clc; close all;
addpath('../');

%%% SO3Exp
nTests = 5;

ins = rand(3,nTests)*pi;
for i=1:nTests
    pthi = strcat('./data/SO3ExpIn', num2str(i), '.dat');
    ptho = strcat('./data/SO3ExpOut', num2str(i), '.dat');
    v = ins(:,i);
    outm = SO3Exp(v);
    save(pthi, 'v', '-ascii');
    save(ptho, 'outm', '-ascii');
    vv = SO3HatInverse(SO3Log(outm));
   
    sum(abs(vv-v))
end