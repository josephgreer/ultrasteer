function [theta,ll,phi] = lengthsToLThetaPhiNonUniform(l,d,psis)
phi = lengthsToPhiNonUniform(l,psis);
phi = mod(phi,2*pi);
phis = psis-phi;
beta = d/3*sum(cos(phis));
alpha = sum(l)/3;
gamma = d*cos(phis(1));

ll = (gamma*alpha-beta*l(1))/(gamma-beta);
% if(abs(beta) < 1e-6)
%     theta = ll*lengthsToKappa(l,d);
% end
% theta = (l-alpha)/beta;

[~,idx] = max(cos(phis));
theta = (ll-l(idx))/(d*cos(phis(idx)));

end