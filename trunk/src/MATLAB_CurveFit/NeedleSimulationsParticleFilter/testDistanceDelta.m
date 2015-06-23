clear; clc; close all;

M = 60;
N = 50;


xs = 100*rand(M,3)-200*ones(M,3);
ys = 100*rand(N,3)-200*ones(N,3);

D = zeros(size(xs,1), size(ys,1));

dp = zeros(1,3);


for i=1:size(D,1)
    for j=1:size(D,2)
        dp = xs(i,:)-ys(j,:);
        D(i,j) = dp*dp';
    end
end
D2 = distanceMatrix(xs,ys);

fastDiff = max(max(abs(D-D2)))
dy = 100*rand(3,1)-200*ones(3,1);

ysdy = ys+repmat(dy',N,1);

Ddy = distanceMatrix(xs,ysdy);

Ddyp = D-2*xs*dy*ones(1,N)+2*ones(M,1)*dy'*ys'+dy'*dy*ones(M,N);
max(max(abs(Ddy-Ddyp)))


