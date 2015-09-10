clear; clc; close all;

rng(1);
nTrials = 100;
for i=1:nTrials
    bx = rand(3,1); bx = bx/norm(bx);
    by = rand(3,1); by = by/norm(by);
    bz = cross(bx,by);
    ul = rand(3,1)*100-ones(3,1)*50;
    
    uv = rand(2,1)*10-ones(2,1)*5;
    
    dist = rand*100-50;
    
    point = ul+[bx by]*uv+dist*bz;
    
    A = [bx by bz];
    
    data = inv(A)*(point-ul);
    
    display(sprintf('u = %f v = %f d = %f', data(1), data(2), data(3)));
    display(sprintf('true: u = %f v = %f d = %f', uv(1), uv(2), dist));
end