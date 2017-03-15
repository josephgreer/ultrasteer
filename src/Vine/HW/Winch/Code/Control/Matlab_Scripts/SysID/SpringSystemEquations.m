% x = [length of actuators] (what we are solving for) \in R^{n_actuators + 2}
% k = [spring stiffnesses of actuators; spring stiffness of main channel; torsional spring stiffness] \in R^{n_actuators+2}
% leq = equilibrium lengths of springs \in R^{n_actuators+1}
function f = SpringSystemEquations(l, k, leq, d)
l = real(l);
n_actuators = length(leq)-1;

kt = k(end);

kappa = lengthsToKappa(l,d);
% lenght of main channel
ll = mean(l);
theta = ll*kappa;

% output
f = zeros(n_actuators,1);

%l-leq
l_minus_leq = [l;ll]-leq;

% sum of forces = 0
f(1) = k(1:end-1).'*l_minus_leq;

act_thetas = actuatorThetas();

% bending plane angle
phi = lengthsToPhi(l(1:n_actuators)); %lengthsToPhiNonUniform(l(1:n_actuators),act_thetas);%   
% phi2 = lengthsToPhiNonUniform(l(1:n_actuators),act_thetas);
% display(phi)
% display(phi2)

assert(n_actuators == 3);

turn_dir = Rz(phi);
turn_dir = turn_dir(1:2,2);

f(2:3) = -kt*turn_dir*theta;

for i=1:n_actuators
    act_dir = Rz(act_thetas(i));
    act_dir = act_dir(:,1);
    r = d*act_dir;
    moment = cross(r,k(i)*l_minus_leq(i)*[0; 0; -1]);
    f(2:3) = f(2:3)+moment(1:2);
end

end