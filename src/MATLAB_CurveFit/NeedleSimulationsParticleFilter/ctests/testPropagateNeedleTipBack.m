clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

x.q = AxisAngleToQuat(rand(3,1)*pi);
x.pos = rand(3,1)*500 - 250;
x.rho = 60+20*rand(1)-10;
x.w = 1;

nc = 50;
dthetas = rand(nc, 1)*2*pi-pi;
vs = rand(nc,1)*10-5;

params = initParamsForSimulation();
us = {};
for i=1:nc
    uc.dtheta = dthetas(i);
    uc.v = vs(i);
    us = vertcat(us, {uc});
end

params.dt = 1/10;
xs = propagateNeedleBack(x, us, params);

xxs = cell2mat(xs);


pos = [xxs.pos];
rho = [xxs.rho];
qs = [xxs.q];

Rs = zeros(9, nc);
for i=1:nc
    Rc = QuatToRotationMatrix(qs(:,i));
    Rs(:,i) = Rc(:);
end


save('./data/propagateNeedleTipBackV.dat', 'vs', '-ascii');
save('./data/propagateNeedleTipBackdThetas.dat', 'dthetas', '-ascii');
save('./data/propagateNeedleTipBackPos.dat', 'pos', '-ascii');
save('./data/propagateNeedleTipBackRhos.dat', 'rho', '-ascii');
save('./data/propagateNeedleTipBackRs.dat', 'Rs', '-ascii');