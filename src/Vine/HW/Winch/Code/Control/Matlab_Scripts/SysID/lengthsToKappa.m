function kappa = lengthsToKappa(l, d)
assert(length(l) == 3);

phi = lengthsToPhi(l);
ll = sum(l)/length(l);
phis = actuatorThetas()-phi;

% kappa = (ll-l(1))/(ll*d)*cos(phis(1));
kappa = 2*sqrt(l.'*l-l(1)*l(2)-l(1)*l(3)-l(2)*l(3))/(d*sum(l));
end