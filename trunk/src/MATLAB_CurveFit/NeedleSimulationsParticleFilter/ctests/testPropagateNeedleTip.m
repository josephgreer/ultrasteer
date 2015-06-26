clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

nTests = 5;
for i=1:5
   x.q = AxisAngleToQuat(rand(3,1)*pi);
   x.rho = 60+rand(1)*50-25;
   x.pos = rand(3,1)*1000 - 500*ones(3,1);
   
   Rin = QuatToRotationMatrix(x.q);
   posIn = x.pos;
   rhoIn = x.rho;
   
   display(sprintf('f64 Rin%d[] = {%.8f, %.8f, %.8f,\n %.8f, %.8f, %.8f,\n %.8f, %.8f, %.8f};\n',...
   i, Rin(1,1), Rin(1,2), Rin(1,3),...
   Rin(2,1), Rin(2,2), Rin(2,3),...
   Rin(3,1), Rin(3,2), Rin(3,3)));

   display(sprintf('f64 posin%d[] = {%.8f,\n %.8f,\n %.8f};\n',...
   i, posIn(1), posIn(2), posIn(3)));

   display(sprintf('f64 rhoin%d = %.8f;\n', i, rhoIn));
   
   params.dt = rand(1);
   u.dtheta = rand(1)*2*pi;
   u.v = rand(1)*50;
   
   display(sprintf('f64 dt%d = %.8f;\n', i, params.dt));
   display(sprintf('f64 dtheta%d = %.8f;\n', i, u.dtheta));
   display(sprintf('f64 v%d = %.8f;\n', i, u.v));
   
   x = propagateNeedleTip(x, u, params);
   Rout = QuatToRotationMatrix(x.q);
   posOut = x.pos;
   rhoOut = x.rho;
   
   display(sprintf('f64 Rout%d[] = {%.8f, %.8f, %.8f,\n %.8f, %.8f, %.8f,\n %.8f, %.8f, %.8f};\n',...
   i, Rout(1,1), Rout(1,2), Rout(1,3),...
   Rout(2,1), Rout(2,2), Rout(2,3),...
   Rout(3,1), Rout(3,2), Rout(3,3)));

   display(sprintf('f64 posout%d[] = {%.8f,\n %.8f,\n %.8f};\n',...
   i, posOut(1), posOut(2), posOut(3)));

   display(sprintf('f64 rhoout%d = %.8f;\n', i, rhoOut));
end