function [n,x,stp]=sm_hist(e)

emin=min(e);
emax=max(e);
emean=mean(e);
estd=std(e);

stp=estd/2;

x=emean;
k=1;
while emean-k*stp-stp/2>emin
    x=[emean-k*stp,x];
    k=k+1;
end
x=[emean-k*stp,x];
k=1;
while emean+k*stp+stp/2<emax
    x=[x,emean+k*stp];
    k=k+1;
end
x=[x,emean+k*stp];

%figure;
n=hist(e,x);
%plot(x,n,'r');
