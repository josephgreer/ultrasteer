function [phir, m] = GetPhiRandM(e,L0,R0,P,a,guess)
f = @(x)([(ellipticE(x(1),x(2))-0.5*ellipticF(x(1),x(2)))/(sqrt(x(2))*cos(x(1)))-L0/(2*R0)*(1-e);...
    ellipticF(x(1),x(2))/(sqrt(x(2))*cos(x(1)))-L0/R0*(1+a/(2*x(2)*cos(x(1))^2))]);

if(~exist('guess'))
    guess = [1;1];
end

phirm = fsolve(f,ones(2,1));
phir = phirm(1);
m = phirm(2);
end