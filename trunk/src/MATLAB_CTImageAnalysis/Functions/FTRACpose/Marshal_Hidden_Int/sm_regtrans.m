function tr=sm_regtrans(p1,p2)
%get the registration transform between two set of points;

[Na,tmp]=size(p1);
a=sum(p1)/Na;
[Nb,tmp]=size(p2);
b=sum(p2)/Nb;

pa=p1-repmat(a,Na,1);
pb=p2-repmat(b,Nb,1);

N=min(Na,Nb);
H=zeros(3);
for i=1:N
    H=H+pa(i,:)'*pb(i,:);
end

[U,S,V]=svd(H);
R=V*U';

p=b'-R*a';

tr=[R,p;0,0,0,1];
