% x = [l_1;...l_n_act] (what we are solving for) \in R^{n_actuators + 2}
% k = [spring stiffnesses of actuators; spring stiffness of main channel; torsional spring stiffness] \in R^{n_actuators+2}
% leq = equilibrium lengths of springs \in R^{n_actuators+1}
function f = FullSpringSystemEquations(x, k, leq, d, psis, torGamma)
x = real(x);

n_actuators = length(leq)-1;

kt = k(end);

l = x(1:n_actuators); 

theta = x(n_actuators+1);
ll = x(n_actuators+2);
phi = x(n_actuators+3);

% output
f = zeros(2*n_actuators,1);

%l-leq
l_minus_leq = [l;ll]-leq;

% sum of forces = 0
f(1) = k(1:end-1).'*l_minus_leq;

assert(n_actuators == 3);

turn_dir = Rz(phi);
turn_dir = turn_dir(1:2,2);

f(2:3) = -(kt*theta)^torGamma*turn_dir;

for i=1:n_actuators
    act_dir = Rz(psis(i)) ;
    act_dir = act_dir(:,1);
    r = d*act_dir;
    moment = cross(r,k(i)*l_minus_leq(i)*[0; 0; -1]);
    f(2:3) = f(2:3)+moment(1:2);
end

phis = psis-phi;
f(4:6) = l+theta*d*cos(phis)-ll;


end