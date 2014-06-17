function pnew=sm_regmapping(p,regtr)
%registration;

[N,tmp]=size(p);
ptmp=regtr*[p,ones(N,1)]';
pnew=ptmp(1:3,:)';
