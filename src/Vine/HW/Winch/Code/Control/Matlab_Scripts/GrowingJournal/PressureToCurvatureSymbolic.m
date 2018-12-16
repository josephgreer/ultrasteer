clear; clc; close all

syms l_1 l l_1eq l_eq K k_1 p_1 D tau t real

eqns = [l_1 + t*D/2 - l ==0,...
        -k_1*p_1*(l_1-l_1eq)-K*(l-l_eq) == 0,...
        D/2*k_1*p_1*(l_1-l_1eq)-tau*t == 0];
    
    
res = solve(eqns,[l t l_1])    

kk1 = 2*D*K*k_1*(l_eq-l_1eq);
kk2 = K*k_1*l_eq*D^2 + 4*k_1*l_1eq*tau;
kk3 = 4*K*l_eq*tau;

coeffs = simplify(collect(simplify(res.t/res.l),p_1))
pretty(simplify(coeffs))